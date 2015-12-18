#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "colormode.h"
#include "brightnessmodel.h"
#include "systeminformation.h"
#include "aboutpatternpaint.h"
#include "sceneconfiguration.h"
#include "colorchooser.h"
#include "patternwriter.h"

#include "blinkytape.h"
#include "blinkytapeuploader.h"
#include "welcomescreen.h"

#include "pencilinstrument.h"
#include "lineinstrument.h"
#include "colorpickerinstrument.h"
#include "sprayinstrument.h"
#include "fillinstrument.h"

#include "eventratelimiter.h"
#include "pattern.h"
#include "patterncollectiondelegate.h"

#include "patternframemodel.h"
#include "patterndelegate.h"
#include "patterncollection.h"
#include "fixture.h"
#include "matrixfixture.h"
#include "preferences.h"
#include "defaults.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QtWidgets>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    colorChooser(COLOR_TOOL_DEFAULT, this),
    progressDialog(this)
{
    setupUi(this);

#if defined(Q_OS_MACX)
    appNap = NULL;
#endif

#if defined(Q_OS_LINUX)
    // If we're on linux, we probably don't have a desktop icon registered,
    // so set one manually
    setWindowIcon(QIcon(":/resources/images/patternpaint.ico"));
#endif

    // Build the new pattern drop-down menu
    QMenu *menu = new QMenu(this);
    menu->addAction(this->actionNew_FramePattern);
    menu->addAction(this->actionNew_ScrollingPattern);

    QToolButton *toolButton = new QToolButton();
    toolButton->setIcon(QIcon(":/icons/images/icons/Create New-100.png"));
    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    fileToolbar->insertWidget(this->actionClose, toolButton);

    // prepare undo/redo
    menuEdit->addSeparator();

    QAction *undoAction = undoGroup.createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Z")));
    undoAction->setEnabled(false);
    menuEdit->addAction(undoAction);

    QAction *redoAction = undoGroup.createRedoAction(this, tr("&Redo"));
    redoAction->setEnabled(false);
    redoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Y")));
    menuEdit->addAction(redoAction);

    // instruments
    ColorpickerInstrument *cpi = new ColorpickerInstrument(this);
    connect(cpi, SIGNAL(pickedColor(QColor)), SLOT(on_colorPicked(QColor)));

    connect(actionPen, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));
    connect(actionLine, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));
    connect(actionSpray, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));
    connect(actionPipette, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));
    connect(actionFill, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));

    actionPen->setData(QVariant::fromValue(new PencilInstrument(this)));
    actionLine->setData(QVariant::fromValue(new LineInstrument(this)));
    actionSpray->setData(QVariant::fromValue(new SprayInstrument(this)));
    actionPipette->setData(QVariant::fromValue(cpi));
    actionFill->setData(QVariant::fromValue(new FillInstrument(this)));

    instrumentToolbar->addWidget(&colorChooser);
    frameEditor->setToolColor(COLOR_TOOL_DEFAULT);

    QSpinBox *penSizeSpin = new QSpinBox(this);
    penSizeSpin->setRange(DRAWING_SIZE_MINIMUM_VALUE, DRAWING_SIZE_MAXIMUM_VALUE);
    penSizeSpin->setValue(DRAWING_SIZE_DEFAULT_VALUE);
    penSizeSpin->setToolTip(tr("Pen size"));
    instrumentToolbar->addWidget(penSizeSpin);

    // tools
    pFrame.setMaximumWidth(30);
    pFrame.setMinimumWidth(30);
    pFrame.setValidator(new QIntValidator(1, std::numeric_limits<int>::max(), this));
    pFrame.setToolTip(tr("Current frame"));
    playbackToolbar->insertWidget(actionStepForward, &pFrame);
    connect(&pFrame, SIGNAL(textEdited(QString)), this, SLOT(frameIndex_valueChanged(QString)));
    frameIndex_valueChanged("1");

    // Pattern info
    patternSpeed->setRange(PATTERN_SPEED_MINIMUM_VALUE, PATTERN_SPEED_MAXIMUM_VALUE);
// patternSpeed->setValue(PATTERN_SPEED_MINIMUM_VALUE);
    connect(patternSpeed, SIGNAL(valueChanged(int)), this, SLOT(patternSpeed_valueChanged(int)));

    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionClose, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionFlip_Horizontal, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionFlip_Vertical, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionClear_Pattern, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionAddFrame, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionDeleteFrame, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionPlay, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionStepForward, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionStepBackward, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            patternSpeed, SLOT(setEnabled(bool)));

    mode = Disconnected;

    // Our pattern editor wants to get some notifications
    connect(&colorChooser, SIGNAL(sendColor(QColor)),
            frameEditor, SLOT(setToolColor(QColor)));
    connect(penSizeSpin, SIGNAL(valueChanged(int)),
            frameEditor, SLOT(setToolSize(int)));

    connect(frameEditor, SIGNAL(dataEdited(int, const QImage)),
            this, SLOT(on_frameDataEdited(int, const QImage)));

    frameEditor->setToolSize(DRAWING_SIZE_MINIMUM_VALUE);

    // Pre-set the upload progress dialog
    progressDialog.setMinimum(0);
    progressDialog.setMaximum(1000);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setAutoClose(false);
    progressDialog.setAutoReset(false);

    // Workaround for bug in QT 5.5.1
#if defined(PROGRESS_DIALOG_WORKAROUND)
    progressDialog.reset();
    progressDialog.hide();
#endif

    // The draw timer tells the pattern to advance
    connect(&drawTimer, SIGNAL(timeout()), this, SLOT(drawTimer_timeout()));

    // Start a scanner to connect to a Blinky automatically
    connect(&connectionScannerTimer, SIGNAL(timeout()), this,
            SLOT(connectionScannerTimer_timeout()));
    connectionScannerTimer.setInterval(CONNECTION_SCANNER_INTERVAL);
    connectionScannerTimer.start();

    actionPen->setChecked(true);
    frameEditor->setInstrument(qvariant_cast<AbstractInstrument *>(actionPen->data()));

    QSettings settings;

    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/state").toByteArray());

    // Fill the examples menu using the examples resource
    populateExamplesMenu(":/examples", menuExamples);

    fixture
        = new MatrixFixture(settings.value("Fixture/DisplaySize",
                                           QSize(DEFAULT_FIXTURE_WIDTH,
                                                 DEFAULT_FIXTURE_HEIGHT)).toSize(),
                            (ColorMode)settings.value("Fixture/ColorOrder", RGB).toInt(),
                            new ExponentialBrightness(1.8, 1.8, 2.1));
    frameEditor->setFixture(fixture);

    patternCollectionListView->setItemDelegate(new PatternCollectionDelegate(this));
    patternCollectionListView->setModel(patternCollection.getModel());

    connect(patternCollectionListView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this,
            SLOT(on_patternCollectionCurrentChanged(const QModelIndex &, const QModelIndex &)));

    timeline->setItemDelegate(new PatternDelegate(this));

    if (settings.value("WelcomeScreen/showAtStartup", true).toBool())
        connect(this, SIGNAL(windowLoaded()), this, SLOT(on_actionWelcome_triggered()));

    // Refresh the display for no pattern selected
    on_patternCollectionCurrentChanged(QModelIndex(), QModelIndex());
}

void MainWindow::populateExamplesMenu(QString directory, QMenu *menu)
{
    QDir examplesDir(directory);
    QFileInfoList examplesList = examplesDir.entryInfoList();

    foreach(QFileInfo fileInfo, examplesList) {
        // If we found a directory, create a submenu and call ourselves again to populate it
        if (fileInfo.isDir()) {
            QMenu *submenu = new QMenu(this);
            submenu->setTitle(fileInfo.fileName());
            menu->addMenu(submenu);
            connect(submenu, SIGNAL(triggered(QAction *)),
                    this, SLOT(on_ExampleSelected(QAction *)), Qt::UniqueConnection);

            populateExamplesMenu(directory + "/" + fileInfo.fileName(), submenu);
        }
        // Otherwise this is a file, so add it to the examples menu.
        else {
            QAction *action = new QAction(fileInfo.baseName(), this);
            action->setObjectName(directory + "/" + fileInfo.fileName());
            menu->addAction(action);
        }
    }
}

MainWindow::~MainWindow()
{
#if defined(Q_OS_MACX)
    // start the app nap inhibitor
    if (appNap != NULL) {
        delete appNap;
        appNap = NULL;
    }
#endif
}

int MainWindow::getCurrentPatternIndex()
{
    if (!patternCollectionListView->currentIndex().isValid())
        return 0;

    return patternCollectionListView->currentIndex().row();
}

int MainWindow::getPatternCount()
{
    if (patternCollectionListView->model() == NULL)
        return 0;

    return patternCollectionListView->model()->rowCount();
}

int MainWindow::getCurrentFrameIndex()
{
    if (!timeline->currentIndex().isValid())
        return 0;

    return timeline->currentIndex().row();
}

int MainWindow::getFrameCount()
{
    if (timeline->model() == NULL)
        return 0;

    return timeline->model()->rowCount();
}

void MainWindow::drawTimer_timeout()
{
    if (getFrameCount() == 0)
        return;

    setNewFrame((getCurrentFrameIndex()+1)%getFrameCount());
}

void MainWindow::connectionScannerTimer_timeout()
{
    // If we are already connected, disregard.
    if ((!controller.isNull()) || mode == Uploading)
        return;

    // Look for controllers
    QList<QPointer<ControllerInfo> > controllerInfos = BlinkyController::probe();

    if (controllerInfos.count() > 0) {
        // TODO: Try another one if this one fails?
        qDebug() << "Attempting to connect to controller at:" << controllerInfos.front()->resourceName();

        controller = controllerInfos.front()->createController();

        // Modify our UI when the tape connection status changes
        connect(controller, SIGNAL(connectionStatusChanged(bool)),
                this, SLOT(on_blinkyConnectionStatusChanged(bool)));

        controller->open();

        return;
    }
}

void MainWindow::patternSpeed_valueChanged(int value)
{
    if(patternCollection.at(getCurrentPatternIndex())->getFrameSpeed() != value)
        patternCollection.at(getCurrentPatternIndex())->setFrameSpeed(value);

    drawTimer.setInterval(1000/value);
}

void MainWindow::frameIndex_valueChanged(QString value)
{
    setNewFrame(value.toInt() - 1);
}

void MainWindow::on_actionPlay_triggered()
{
    if (drawTimer.isActive())
        stopPlayback();
    else
        startPlayback();
}

void MainWindow::startPlayback()
{
    drawTimer.start();
    actionPlay->setText(tr("Pause"));
    actionPlay->setIcon(QIcon(":/icons/images/icons/Pause-100.png"));
}

void MainWindow::stopPlayback()
{
    drawTimer.stop();
    actionPlay->setText(tr("Play"));
    actionPlay->setIcon(QIcon(":/icons/images/icons/Play-100.png"));
}

bool MainWindow::savePatternAs(Pattern *item)
{
    QSettings settings;
    QString lastDirectory = settings.value("File/SaveDirectory").toString();

    QDir dir(lastDirectory);
    if (!dir.isReadable())
        lastDirectory = QDir::homePath();

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Pattern"), lastDirectory,
                                                    tr("Pattern Files (*.png *.jpg *.bmp)"));

    if (fileName.length() == 0)
        return false;

    QFileInfo fileInfo(fileName);
    settings.setValue("File/SaveDirectory", fileInfo.absolutePath());

    if (!item->saveAs(fileInfo.absoluteFilePath()))
        showError(tr("Error saving pattern %1. Try saving it somewhere else?")
                  .arg(fileInfo.absolutePath()));

    return !item->getModified();
}

bool MainWindow::savePattern(Pattern *item)
{
    if (!item->hasFilename()) {
        return savePatternAs(item);
    } else {
        if (!item->save()) {
            showError(tr("Error saving pattern %1. Try saving it somewhere else?")
                      .arg(item->getName()));
        }
        return !item->getModified();
    }
}

void MainWindow::on_actionSave_File_as_triggered()
{
    if (patternCollection.isEmpty())
        return;
    savePatternAs(patternCollection.at(getCurrentPatternIndex()));
}

void MainWindow::on_actionSave_File_triggered()
{
    if (patternCollection.isEmpty())
        return;
    savePattern(patternCollection.at(getCurrentPatternIndex()));
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionExport_pattern_for_Arduino_triggered()
{
    // TODO: Merge this with save

    if (patternCollection.isEmpty())
        return;

    QSettings settings;
    QString lastDirectory = settings.value("File/ExportArduinoDirectory").toString();

    QDir dir(lastDirectory);
    if (!dir.isReadable())
        lastDirectory = QDir::homePath();

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr(
                                                        "Save Pattern for Arduino"), lastDirectory,
                                                    tr("Header File (*.h)"));

    if (fileName.length() == 0)
        return;

    QFileInfo fileInfo(fileName);
    settings.setValue("File/ExportArduinoDirectory", fileInfo.absolutePath());

    // Note: Converting frameRate to frame delay here.
    PatternWriter patternWriter(patternCollection.at(getCurrentPatternIndex()),
                                PatternWriter::RGB24,
                                fixture);

    // Attempt to open the specified file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        showError(tr("Error, cannot write file %1.")
                  .arg(fileName));
        return;
    }

    QTextStream ts(&file);
    ts << patternWriter.getHeader();
    file.close();
}

void MainWindow::on_blinkyConnectionStatusChanged(bool connected)
{
    qDebug() << "status changed, connected=" << connected;

    if (connected) {
        mode = Connected;
        startPlayback();

#if defined(Q_OS_MACX)
        // start the app nap inhibitor
        if (appNap == NULL)
            appNap = new CAppNapInhibitor("Interaction with hardware");

#endif
    } else {
        mode = Disconnected;
        stopPlayback();

        // TODO: Does this delete the serial object reliably?
        controller.clear();

        connectionScannerTimer.start();

#if defined(Q_OS_MACX)
        // start the app nap inhibitor
        if (appNap != NULL) {
            delete appNap;
            appNap = NULL;
        }
#endif
    }

    actionSave_to_Blinky->setEnabled(mode == Connected && !patternCollection.isEmpty());
}

void MainWindow::on_actionAbout_triggered()
{
    // TODO: store this somewhere, for later disposal.
    AboutPatternPaint *info = new AboutPatternPaint(this);
    info->show();
}

void MainWindow::on_actionSystem_Information_triggered()
{
    // TODO: store this somewhere, for later disposal.
    SystemInformation *info = new SystemInformation(this);
    info->show();
}

void MainWindow::on_uploaderProgressChanged(float progressValue)
{
    if (progressDialog.isHidden()) {
        qDebug()
            << "Got a progress event while the progress dialog is hidden, event order problem?";
        return;
    }

    int newValue = progressDialog.maximum()*progressValue;

    progressDialog.setValue(newValue);
}

void MainWindow::on_uploaderFinished(bool result)
{
    mode = Disconnected;
    uploader.clear();

    progressDialog.hide();

    qDebug() << "Uploader finished! Result:" << result;
    if (!result) {
        // TODO: allow restarting the upload process if it failed
        QMessageBox msgBox(this);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setText("Error updating blinky- please try again.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::on_actionVisit_the_Blinkinlabs_forum_triggered()
{
    QDesktopServices::openUrl(QUrl("http://forums.blinkinlabs.com/", QUrl::TolerantMode));
}

void MainWindow::on_actionTroubleshooting_tips_triggered()
{
    QDesktopServices::openUrl(QUrl("http://blinkinlabs.com/blinkytape/docs/troubleshooting/",
                                   QUrl::TolerantMode));
}

void MainWindow::on_actionFlip_Horizontal_triggered()
{
    if (patternCollection.isEmpty())
        return;

    QImage frame = patternCollection.at(getCurrentPatternIndex())->getEditImage(
        getCurrentFrameIndex());
    frame = frame.mirrored(true, false);
    patternCollection.at(getCurrentPatternIndex())->setEditImage(
        getCurrentFrameIndex(), frame);
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    if (patternCollection.isEmpty())
        return;

    QImage frame = patternCollection.at(getCurrentPatternIndex())->getEditImage(
        getCurrentFrameIndex());
    frame = frame.mirrored(false, true);
    patternCollection.at(getCurrentPatternIndex())->setEditImage(
        getCurrentFrameIndex(), frame);
}

void MainWindow::on_actionClear_Pattern_triggered()
{
    if (patternCollection.isEmpty())
        return;

    QImage frame = patternCollection.at(getCurrentPatternIndex())->getEditImage(
        getCurrentFrameIndex());
    frame.fill(COLOR_CANVAS_DEFAULT);
    patternCollection.at(getCurrentPatternIndex())->setEditImage(
        getCurrentFrameIndex(), frame);
}

void MainWindow::showError(QString errorMessage)
{
    QMessageBox box(this);
    box.setWindowModality(Qt::WindowModal);
    box.setText(errorMessage);
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}

void MainWindow::on_actionRestore_firmware_triggered()
{
    // If the controller doesn't exist, create a new uploader based on the blinkytape
    if (controller.isNull()) {
        uploader = QPointer<BlinkyUploader>(new BlinkyTapeUploader(this));

        connectUploader();

        if (!uploader->upgradeFirmware(-1)) {
            showError(uploader->getErrorString());
            return;
        }

        progressDialog.setWindowTitle("Firmware reset");
        progressDialog.setLabelText(
            "Searching for a BlinkyTape bootloader...\n"
            "\n"
            "Please connect your blinkytape to the computer via USB,\n"
            "then perform the reset trick. As soon as the device\n"
            "restarts, the progress bar should start moving and the\n"
            "firmware will be restored.");
    }
    // Otherwise just grab it
    else {
        if (!controller->getUploader(uploader))
            return;

        if (uploader.isNull())
            return;

        connectUploader();

        if (!uploader->upgradeFirmware(*controller)) {
            showError(uploader->getErrorString());
            return;
        }

        progressDialog.setWindowTitle("Firmware reset");
        progressDialog.setLabelText("Loading new firmware onto Blinky");
    }

    mode = Uploading;

    progressDialog.setValue(progressDialog.minimum());
    progressDialog.show();
}

void MainWindow::on_actionSave_to_Blinky_triggered()
{
    if (patternCollection.isEmpty())
        return;

    if (controller.isNull())
        return;

    if (!controller->getUploader(uploader))
        return;

    if (uploader.isNull())
        return;

    if (uploader->getSupportedEncodings().count() == 0)
        return;

    QList<PatternWriter> patternWriters;

    foreach(Pattern* pattern, patternCollection.patterns()) {
        PatternWriter patternWriter(pattern,
                                    uploader->getSupportedEncodings().front(),
                                    fixture);
        patternWriters.append(patternWriter);
    }

    connectUploader();

    if (!uploader->startUpload(*controller, patternWriters)) {
        showError(uploader->getErrorString());
        return;
    }
    mode = Uploading;

    progressDialog.setWindowTitle(tr("Blinky exporter"));
    progressDialog.setLabelText(tr("Saving to Blinky..."));

    progressDialog.setValue(progressDialog.minimum());
    progressDialog.show();
}

void MainWindow::on_actionAddress_programmer_triggered()
{
    AddressProgrammer programmer(this);
    programmer.setWindowModality(Qt::WindowModal);
    programmer.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#if defined(Q_OS_MACX)    // Workaround for issue #114, multile close events are sent when closing from the dock
    static intervalFilter rateLimiter(200);
    if (!rateLimiter.check()) {
        event->ignore();
        return;
    }
#endif

    QList<Pattern *> unsavedPatterns;

    foreach(Pattern* pattern, patternCollection.patterns()) {
        if (pattern->getModified() == true)
            unsavedPatterns.append(pattern);
    }

    // If we only have one pattern, show the regular prompt for save dialog
    if (!promptForSave(unsavedPatterns)) {
        event->ignore();
        return;
    }

    // If we are connected to a blinky, try to reset it so that it will start playing back its pattern
    if (!controller.isNull()) {
        qDebug() << "Attempting to reset blinky";
        controller->reset();
    }

    QSettings settings;
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/state", saveState());

#if defined(Q_OS_MACX)    // Workaround for issue #114, multile close events are sent when closing from the dock
    rateLimiter.force();
#endif

    QMainWindow::closeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    static bool firstLoad = true;
    if(firstLoad) {
        firstLoad = false;
        emit windowLoaded();
    }
}

void MainWindow::on_instrumentSelected(bool)
{
    QAction *act = static_cast<QAction *>(sender());
    Q_ASSERT(act != NULL);
    foreach (QAction *a, instrumentToolbar->actions())
        a->setChecked(false);

    act->setChecked(true);
    frameEditor->setInstrument(qvariant_cast<AbstractInstrument *>(act->data()));
}

void MainWindow::on_colorPicked(QColor color)
{
    colorChooser.setColor(color);
    frameEditor->setToolColor(color);
}

bool MainWindow::promptForSave(Pattern *item)
{
    QList<Pattern *> patterns;
    patterns.append(item);
    return promptForSave(patterns);
}

bool MainWindow::promptForSave(QList<Pattern *> patterns)
{
    QString messageText;

    if(patterns.count() == 1) {
        messageText += tr("The pattern %1 has been modified.")
                .arg(patterns.first()->getName());
    }
    else {
        messageText += tr("The following patterns have been modified:\n");

        foreach(Pattern *pattern, patterns)
            messageText += pattern->getName() + "\n";
    }

    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setText(messageText);
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ans = msgBox.exec();

    if (ans == QMessageBox::Save) {
        foreach(Pattern *pattern, patterns)
            if (!savePattern(pattern))
                return false;
        return true;
    }

    if (ans == QMessageBox::Discard)
        return true;

    return false;
}

void MainWindow::applyScene(SceneTemplate sceneTemplate)
{
    QSize newDisplaySize(sceneTemplate.width, sceneTemplate.height);
    ColorMode newColorMode = sceneTemplate.colorMode;

    // Test if any patterns need to be resized
    QList<Pattern *> needToResize;

    foreach(Pattern* pattern, patternCollection.patterns())
        if (pattern->getFrameSize() != newDisplaySize)
            needToResize.append(pattern);

    if (needToResize.count() > 0) {
        QString messageText = tr("Some patterns have a different size than the fixture. Rescale "
                                 " them to to fit?");

        QMessageBox msgBox(this);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setText(messageText);
        msgBox.setInformativeText(tr("Resize patterns?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ans = msgBox.exec();

        if (ans == QMessageBox::Yes) {
            foreach(Pattern* pattern, needToResize) {
                pattern->resize(newDisplaySize, false);
            }
        } else if (ans == QMessageBox::Cancel) {
            return;
        }
    }

    // Finally, apply the new settings
    QSettings settings;
    settings.setValue("Fixture/DisplaySize", newDisplaySize);
    settings.setValue("Fixture/ColorOrder", newColorMode);

    fixture->setSize(newDisplaySize);
    fixture->setColorMode(newColorMode);

    // If the new scene includes examples, load them as well
    // TODO: Separate this out into a different process?
    if (!sceneTemplate.examples.isEmpty()) {
        QDir examplesDir(sceneTemplate.examples);
        QFileInfoList examplesList = examplesDir.entryInfoList();

        foreach(QFileInfo fileinfo, examplesList) {
            if (!fileinfo.isDir()) {
                Pattern::PatternType type = Pattern::Scrolling;
                if (fileinfo.fileName().endsWith(".frames.png"))
                    type = Pattern::FrameBased;

                loadPattern(type,
                            sceneTemplate.examples + "/" + fileinfo.fileName());
            }
        }

        patternCollectionListView->setCurrentIndex(patternCollectionListView->model()->index(0, 0));
    }
}

void MainWindow::connectUploader()
{
    connect(uploader, SIGNAL(progressChanged(float)),
            this, SLOT(on_uploaderProgressChanged(float)));
    connect(uploader, SIGNAL(finished(bool)),
            this, SLOT(on_uploaderFinished(bool)));
    connect(&progressDialog, SIGNAL(canceled()),
            uploader, SLOT(cancel()));
}

bool MainWindow::loadPattern(Pattern::PatternType type, const QString fileName)
{
    QSettings settings;
    int frameCount = settings.value("Options/FrameCount", DEFAULT_FRAME_COUNT).toUInt();

    QSize displaySize;

    displaySize = fixture->getSize();

    Pattern *pattern = new Pattern(type, displaySize, frameCount);

    // TODO: Fix load procedure fit the pattern type
    if (!fileName.isEmpty())
        if (!pattern->load(fileName))
            return false;

    int newPosition = 0;
    if (getPatternCount() > 0)
        newPosition = getCurrentPatternIndex()+1;

    patternCollection.add(pattern, newPosition);
    patternCollectionListView->setCurrentIndex(patternCollectionListView->model()->index(newPosition,
                                                                                         0));

    return true;
}

void MainWindow::setNewFrame(int newFrame)
{
    if (patternCollection.isEmpty())
        return;

    // TODO: Detect if we changed frames and only continue if it's a new frame...

    if (newFrame > getFrameCount())
        newFrame = getFrameCount() - 1;

    if (newFrame < 0)
        newFrame = 0;

    timeline->setCurrentIndex(timeline->model()->index(newFrame, 0));

    pFrame.setText(QString::number(getCurrentFrameIndex()+1));

    setPatternData(getCurrentFrameIndex(),
                   patternCollection.at(getCurrentPatternIndex())->getEditImage(newFrame));
}

void MainWindow::updateBlinky()
{
    if (controller.isNull())
        return;

    if (patternCollection.isEmpty())
        return;

    QImage frame = patternCollection.at(getCurrentPatternIndex())->getFrameImage(
        getCurrentFrameIndex());

    QList<QColor> pixels = fixture->getColorStreamForFrame(frame);

    // TODO:
    QByteArray ledData;
    for (int i = 0; i < pixels.size(); i++) {
        switch (fixture->getColorMode()) {
        case RGB:
            ledData.append(pixels[i].red());
            ledData.append(pixels[i].green());
            ledData.append(pixels[i].blue());
            break;
        case GRB:
            ledData.append(pixels[i].green());
            ledData.append(pixels[i].red());
            ledData.append(pixels[i].blue());
            break;
        case COLOR_MODE_COUNT:
        default:
            break;
        }
    }

    controller->sendUpdate(ledData);
}

void MainWindow::on_actionClose_triggered()
{
    if (patternCollection.isEmpty())
        return;

    if (!promptForSave(patternCollection.at(getCurrentPatternIndex())))
        return;

    // TODO: remove the undo stack from the undo group?
    patternCollection.remove(getCurrentPatternIndex());
}

void MainWindow::on_patternCollectionCurrentChanged(const QModelIndex &current, const QModelIndex &)
{
    emit(patternStatusChanged(current.isValid()));

    on_patternSizeUpdated();

    // TODO: we're going to have to unload our references, but for now skip that.
    if (!current.isValid()) {
        undoGroup.setActiveStack(NULL);
        timeline->setModel(NULL);

        setPatternName("()");
        setPatternModified(false);
        setPatternData(0, QImage());
        frameEditor->setShowPlaybakIndicator(false);
        timeline->setVisible(false);
        patternSpeed->setValue(1);

        actionSave_to_Blinky->setEnabled(false);

        return;
    }

    Pattern *newpattern = patternCollection.at(current.row());

    undoGroup.setActiveStack(newpattern->getUndoStack());
    timeline->setModel(newpattern->getModel());

    setPatternName(newpattern->getName());
    setPatternModified(newpattern->getModified());
    setPatternData(getCurrentFrameIndex(), newpattern->getEditImage(getCurrentPatternIndex()));
    frameEditor->setShowPlaybakIndicator(newpattern->hasPlaybackIndicator());
    timeline->setVisible(newpattern->hasTimeline());
    patternSpeed->setValue(newpattern->getFrameSpeed());

    actionSave_to_Blinky->setEnabled(mode == Connected);

    // TODO: Should we unregister these eventually?
    connect(timeline->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(on_timelineSelectedChanged(const QModelIndex &, const QModelIndex &)));
    connect(timeline->model(),
            SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)),
            this,
            SLOT(on_PatternDataChanged(const QModelIndex &, const QModelIndex &,
                                       const QVector<int> &)));
}

void MainWindow::on_timelineSelectedChanged(const QModelIndex &current, const QModelIndex &)
{
    setNewFrame(current.row());
}

void MainWindow::on_PatternDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                       const QVector<int> &roles)
{
    int currentIndex = getCurrentFrameIndex();

    foreach (int role, roles) {
        if (role == PatternModel::FileName) {
            setPatternName(patternCollection.at(getCurrentPatternIndex())->getName());
        } else if (role == PatternModel::Modified) {
            setPatternModified(patternCollection.at(getCurrentPatternIndex())->getModified());
        } else if (role == PatternModel::FrameImage) {
            // If the current selection changed, refresh so that the FrameEditor contents will be redrawn
            if (currentIndex >= topLeft.row() && currentIndex <= bottomRight.row()) {
                setPatternData(getCurrentFrameIndex(),
                               patternCollection.at(getCurrentPatternIndex())->getEditImage(
                                   getCurrentFrameIndex()));
            }
        }
    }
}

void MainWindow::setPatternData(int index, QImage data)
{
    frameEditor->setFrameData(index, data);

    updateBlinky();
}

void MainWindow::on_patternSizeUpdated()
{
    if (patternCollection.isEmpty()) {
        setPatternData(0, QImage());
        return;
    }

    setPatternData(getCurrentFrameIndex(),
                   patternCollection.at(getCurrentPatternIndex())->getEditImage(
                       getCurrentFrameIndex()));

    // And kick the scroll area so that it will size itself
    // scrollArea->resize(scrollArea->width()+1, scrollArea->height());
    // this->patternsplitter->refresh();
}

void MainWindow::on_frameDataEdited(int index, QImage update)
{
    if (patternCollection.isEmpty())
        return;

    patternCollection.at(getCurrentPatternIndex())->setEditImage(index, update);
}

void MainWindow::setPatternName(QString name)
{
    patternName->setText(name);
}

void MainWindow::on_actionStepForward_triggered()
{
    if (patternCollection.isEmpty())
        return;

    setNewFrame((getCurrentFrameIndex()+1)%getFrameCount());
}

void MainWindow::on_actionStepBackward_triggered()
{
    if (patternCollection.isEmpty())
        return;

    setNewFrame(getCurrentFrameIndex() <= 0 ? getFrameCount()-1 : getCurrentFrameIndex()-1);
}

void MainWindow::on_actionAddFrame_triggered()
{
    if (patternCollection.isEmpty())
        return;

    patternCollection.at(getCurrentPatternIndex())->addFrame(getCurrentFrameIndex());
    setNewFrame(getCurrentFrameIndex()-1);
}

void MainWindow::on_actionDeleteFrame_triggered()
{
    if (patternCollection.isEmpty())
        return;

    patternCollection.at(getCurrentPatternIndex())->deleteFrame(getCurrentFrameIndex());
    setNewFrame(getCurrentFrameIndex());
}

void MainWindow::setPatternModified(bool modified)
{
    actionSave_File->setEnabled(modified);
}

void MainWindow::on_ExampleSelected(QAction *action)
{
    qDebug() << "Example selected:" << action->objectName();

    Pattern::PatternType type = Pattern::Scrolling;
    if (action->objectName().endsWith(".frames.png"))
        type = Pattern::FrameBased;

    if (!loadPattern(type, action->objectName())) {
        showError("Could not open file "
                  + action->objectName()
                  + ". Perhaps it has a formatting problem?");
        return;
    }
}

void MainWindow::on_actionNew_ScrollingPattern_triggered()
{
    loadPattern(Pattern::Scrolling, QString());
}

void MainWindow::on_actionNew_FramePattern_triggered()
{
    loadPattern(Pattern::FrameBased, QString());
}

void MainWindow::on_actionConfigure_Fixture_triggered()
{
    SceneConfiguration sceneConfiguration(this);

    // To do: save the controller and fixture types so that we can recall them here.
    SceneTemplate sceneTemplate;
    sceneTemplate.name = "???";
    sceneTemplate.photo = "";
    sceneTemplate.examples = "";
    if (!controller.isNull())
        sceneTemplate.controllerType = controller->getName();
    if (!fixture.isNull()) {
        sceneTemplate.fixtureType = fixture->getName();
        sceneTemplate.colorMode = fixture->getColorMode();
        sceneTemplate.height = fixture->getSize().height();
        sceneTemplate.width = fixture->getSize().width();
    }

    sceneConfiguration.setSceneTemplate(sceneTemplate);
    sceneConfiguration.exec();

    if (sceneConfiguration.result() != QDialog::Accepted)
        return;

    applyScene(sceneConfiguration.getSceneTemplate());
}

void MainWindow::openPattern(Pattern::PatternType type)
{
    QSettings settings;
    QString lastDirectory = settings.value("File/LoadDirectory").toString();

    QDir dir(lastDirectory);
    if (!dir.isReadable())
        lastDirectory = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Pattern"), lastDirectory,
                                                    tr("Pattern Files (*.png *.jpg *.bmp *.gif)"));

    if (fileName.length() == 0)
        return;

    QFileInfo fileInfo(fileName);
    settings.setValue("File/LoadDirectory", fileInfo.absolutePath());

    // TODO: choose scrolling/frame-based
    if (!loadPattern(type, fileName)) {
        showError(tr("Could not open file %1. Perhaps it has a formatting problem?")
                  .arg(fileName));
        return;
    }
}

void MainWindow::on_actionOpen_Scrolling_Pattern_triggered()
{
    openPattern(Pattern::Scrolling);
}

void MainWindow::on_actionOpen_Frame_based_Pattern_triggered()
{
    openPattern(Pattern::FrameBased);
}

void MainWindow::on_actionPreferences_triggered()
{
    Preferences *preferences = new Preferences(this);
    preferences->show();
}

void MainWindow::on_actionWelcome_triggered()
{
    WelcomeScreen *welcomeScreen = new WelcomeScreen(this);

    connect(welcomeScreen, SIGNAL(sceneSelected(SceneTemplate)),
            this, SLOT(applyScene(SceneTemplate)));

    welcomeScreen->setAttribute(Qt::WA_DeleteOnClose, true);
    welcomeScreen->show();
}
