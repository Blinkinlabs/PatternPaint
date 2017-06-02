#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "colormode.h"
#include "exponentialbrightness.h"
#include "systeminformation.h"
#include "aboutpatternpaint.h"
#include "sceneconfiguration.h"
#include "colorchooser.h"
#include "patternwriter.h"
#include "debuglog.h"
#include "firmwaremanager.h"
#include "firmwarestore.h"

#include "blinkytape.h"
#include "blinkytapeuploader.h"
#include "welcomescreen.h"

#include "pencilinstrument.h"
#include "lineinstrument.h"
#include "colorpickerinstrument.h"
#include "sprayinstrument.h"
#include "fillinstrument.h"

#include "intervalfilter.h"
#include "pattern.h"
#include "patterncollectiondelegate.h"

#include "patternframemodel.h"
#include "patterndelegate.h"
#include "patterncollection.h"
#include "fixture.h"
#include "preferences.h"
#include "defaults.h"
#include "firmwarestore.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QtWidgets>
#include <QStandardPaths>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    colorChooser(this),
#if defined(Q_OS_MACX)
    closeEventIntervalFilter(200),
#endif
    firstLoad(true)
{
    setupUi(this);

#if defined(Q_OS_MACX)
    CocoaInitializer cocoaInitiarizer;  // TODO: We only need to call this temporarily, right?

    appNap = NULL;
#endif

#if defined(Q_OS_LINUX)
    // If we're on linux, we probably don't have a desktop icon registered,
    // so set one manually
    setWindowIcon(QIcon(":/resources/images/patternpaint.ico"));
#endif

    // Build the new pattern drop-down menu
    // Open question: Why doesn't this->menuNew work?
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
    undoAction->setEnabled(false);
    undoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Z")));
    undoAction->setIcon(QIcon(":/icons/images/icons/Undo-100.png"));
    undoAction->setIconVisibleInMenu(false);
    menuEdit->addAction(undoAction);
    instrumentToolbar->insertAction(actionPen, undoAction);


    QAction *redoAction = undoGroup.createRedoAction(this, tr("&Redo"));
    redoAction->setEnabled(false);
    redoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Y")));
    redoAction->setIcon(QIcon(":/icons/images/icons/Redo-100.png"));
    redoAction->setIconVisibleInMenu(false);
    menuEdit->addAction(redoAction);
    instrumentToolbar->insertAction(actionPen, redoAction);

    // instruments
    ColorpickerInstrument *cpi = new ColorpickerInstrument(&instrumentConfiguration, this);
    connect(cpi, SIGNAL(pickedColor(QColor)), SLOT(on_colorPicked(QColor)));

    connect(actionPen, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));
    connect(actionLine, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));
    connect(actionSpray, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));
    connect(actionPipette, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));
    connect(actionFill, SIGNAL(triggered(bool)), SLOT(on_instrumentSelected(bool)));

    actionPen->setData(QVariant::fromValue(new PencilInstrument(&instrumentConfiguration, this)));
    actionLine->setData(QVariant::fromValue(new LineInstrument(&instrumentConfiguration, this)));
    actionSpray->setData(QVariant::fromValue(new SprayInstrument(&instrumentConfiguration, this)));
    actionPipette->setData(QVariant::fromValue(cpi));
    actionFill->setData(QVariant::fromValue(new FillInstrument(&instrumentConfiguration, this)));

    instrumentToolbar->addWidget(&colorChooser);
    instrumentConfiguration.setToolColor(COLOR_TOOL_DEFAULT);
    colorChooser.setColor(COLOR_TOOL_DEFAULT);

    QSpinBox *penSizeSpin = new QSpinBox(this);
    penSizeSpin->setRange(DRAWING_SIZE_MINIMUM_VALUE, DRAWING_SIZE_MAXIMUM_VALUE);
    penSizeSpin->setValue(DRAWING_SIZE_DEFAULT_VALUE);
    penSizeSpin->setToolTip(tr("Pen size"));
    instrumentToolbar->addWidget(penSizeSpin);

    // tools
    frameIndexWidget.setMaximumWidth(30);
    frameIndexWidget.setMinimumWidth(30);
    frameIndexWidget.setValidator(new QIntValidator(1, std::numeric_limits<int>::max(), this));
    frameIndexWidget.setToolTip(tr("Current frame"));
    playbackToolbar->insertWidget(actionStepForward, &frameIndexWidget);
    connect(&frameIndexWidget, SIGNAL(textEdited(QString)), this, SLOT(frameIndexWidget_valueChanged(QString)));

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
    connect(this, SIGNAL(patternStatusChanged(bool)),
            &frameIndexWidget, SLOT(setEnabled(bool)));

    mode = Disconnected;

    // Our pattern editor wants to get some notifications
    connect(&colorChooser, SIGNAL(sendColor(QColor)),
            &instrumentConfiguration, SLOT(setToolColor(QColor)));
    connect(penSizeSpin, SIGNAL(valueChanged(int)),
            &instrumentConfiguration, SLOT(setToolSize(int)));

    instrumentConfiguration.setToolSize(DRAWING_SIZE_MINIMUM_VALUE);

    connect(actionZoomIn, SIGNAL(triggered(bool)), frameEditor, SLOT(zoomIn()));
    connect(actionZoomOut, SIGNAL(triggered(bool)), frameEditor, SLOT(zoomOut()));

    connect(actionFitToHeight, SIGNAL(triggered(bool)), frameEditor, SLOT(setFitToHeight(bool)));
    connect(actionFitToWidth, SIGNAL(triggered(bool)), frameEditor, SLOT(setFitToWidth(bool)));
    connect(actionFitToScreen, SIGNAL(triggered(bool)), frameEditor, SLOT(setFitToScreen(bool)));

    connect(frameEditor, SIGNAL(dataEdited(int, const QImage)), this, SLOT(on_frameDataEdited(int, const QImage)));

    connect(frameEditor, SIGNAL(fitToHeightChanged(bool)), actionFitToHeight, SLOT(setChecked(bool)));
    connect(frameEditor, SIGNAL(fitToWidthChanged(bool)), actionFitToWidth, SLOT(setChecked(bool)));
    connect(frameEditor, SIGNAL(fitToScreenChanged(bool)), actionFitToScreen, SLOT(setChecked(bool)));

    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionZoomIn, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionZoomOut, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionFitToHeight, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionFitToWidth, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(patternStatusChanged(bool)),
            actionFitToScreen, SLOT(setEnabled(bool)));

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

    // Buld the scene
    SceneTemplate scene;

    QSize fixtureSize = settings.value("Fixture/DisplaySize",
                                        QSize(DEFAULT_FIXTURE_WIDTH,
                                              DEFAULT_FIXTURE_HEIGHT)).toSize();

    scene.firmwareName = settings.value("BlinkyTape/firmwareName", BLINKYTAPE_DEFAULT_FIRMWARE_NAME).toString();

    scene.fixtureType = settings.value("Fixture/Type", DEFAULT_FIXTURE_TYPE).toString();
    scene.colorMode = (ColorMode)settings.value("Fixture/ColorOrder", RGB).toInt();
    scene.size = fixtureSize;

    applyScene(scene);

    patternCollectionListView->setItemDelegate(new PatternCollectionDelegate(this));
    patternCollectionListView->setModel(patternCollection.getModel());

    connect(patternCollectionListView->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this,
            SLOT(on_patternCollectionCurrentChanged(const QModelIndex &, const QModelIndex &)));

    timeline->setItemDelegate(new PatternDelegate(this));

    connect(actionOutput_Preview,SIGNAL(triggered(bool)),outputPreviewFrame,SLOT(setVisible(bool)));
    this->actionOutput_Preview->setChecked(settings.value("MainWindow/outputPreview", true).toBool());
    outputPreviewFrame->setVisible(settings.value("MainWindow/outputPreview", true).toBool());

    QList<QToolBar *> toolbars = this->findChildren<QToolBar *>();
    for (QToolBar *toolbar : toolbars) {
        QAction *action = new QAction(this);
        action->setText(toolbar->windowTitle());
        action->setCheckable(true);
        connect(action,SIGNAL(triggered(bool)),toolbar,SLOT(setVisible(bool)));
        connect(toolbar,SIGNAL(visibilityChanged(bool)),action,SLOT(setChecked(bool)));
        this->menuToolbars->addAction(action);
    }

    connect(this, SIGNAL(windowLoaded()), this, SLOT(on_windowLoaded()),
            Qt::ConnectionType(Qt::QueuedConnection));

    // Refresh the display for no pattern selected
    on_patternCollectionCurrentChanged(QModelIndex(), QModelIndex());

    autoUpdater = NULL;

#if defined(DISABLE_UPDATE_CHECKS)

#warning Debug mode detected, update checking mechanism disabled

#else

#if defined(Q_OS_MACX)
    autoUpdater = new SparkleAutoUpdater(OSX_RELEASE_APPCAST_URL);
#endif // Q_OS__MACX

#if defined(Q_OS_WIN)
    autoUpdater = new WinSparkleAutoUpdater(WINDOWS_RELEASE_APPCAST_URL);
#endif // Q_OS_WIN

#endif  // DISABLE_UPDATE_CHECKS
}


void MainWindow::on_windowLoaded()
{
    // Events that should only be kicked off after the window has been displayed
    if (autoUpdater)
        autoUpdater->init();

    QSettings settings;
    if (settings.value("WelcomeScreen/showAtStartup", true).toBool())
        on_actionWelcome_triggered();
}

void MainWindow::populateExamplesMenu(QString directory, QMenu *menu)
{
    QDir examplesDir(directory);
    QFileInfoList examplesList = examplesDir.entryInfoList();

    for (QFileInfo fileInfo : examplesList) {
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
    // stop the app nap inhibitor
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

    setFrameIndex((getCurrentFrameIndex()+1)%getFrameCount());
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

void MainWindow::frameIndexWidget_valueChanged(QString value)
{
    setFrameIndex(value.toInt() - 1);
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

void MainWindow::on_actionSave_as_triggered()
{
    if (patternCollection.isEmpty())
        return;
    savePatternAs(patternCollection.at(getCurrentPatternIndex()));
}

void MainWindow::on_actionSave_triggered()
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

    PatternWriter patternWriter(*(patternCollection.at(getCurrentPatternIndex())),
                                *fixture,
                                PatternWriter::RGB565_RLE);

    // Attempt to open the specified file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        showError(tr("Error, cannot write file %1.")
                  .arg(fileName));
        return;
    }

    QTextStream ts(&file);
    ts << patternWriter.getDataAsHeader();
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
    SystemInformation *info = new SystemInformation(this);
    info->show();
}

void MainWindow::on_uploaderFinished(bool result)
{
    mode = Disconnected;

    qDebug() << "Uploader finished! Result:" << result;
    if (!result)
        showError("Error updating blinky- please try again.");
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


void MainWindow::on_actionFirmware_Manager_triggered()
{
    FirmwareManager *firmwareManager = new FirmwareManager(this);
    firmwareManager->show();
}


void MainWindow::on_actionRestore_firmware_triggered()
{
    // If the controller doesn't exist, create a new uploader based on the blinkytape
    // TODO: Replace this with a generic 'bootloader' search
    if (controller.isNull()) {
        QPointer<BlinkyUploader> uploader;
        uploader = new BlinkyTapeUploader(this);

        QProgressDialog* dialog = makeProgressDialog(uploader);

        dialog->setWindowTitle("Firmware reset");
        dialog->setLabelText(
            "Searching for a BlinkyTape bootloader...\n"
            "\n"
            "Please connect your blinkytape to the computer via USB,\n"
            "then perform the reset trick. As soon as the device\n"
            "restarts, the progress bar should start moving and the\n"
            "firmware will be restored.");
        dialog->show();

        if (!uploader->restoreFirmware(-1)) {
            // TODO: this might show an error message twice if the upload fails.
            showError(uploader->getErrorString());
            dialog->close();
            return;
        }
    }

    // Otherwise, create it from the controller
    else {
        QPointer<FirmwareLoader> loader;

        if (!controller->getFirmwareLoader(loader)) {
            showError("Firmware update not supported for this controller type!");
            return;
        }

//        if (loader.isNull())
//            return;

        QProgressDialog* dialog = makeProgressDialog(loader);

        dialog->setWindowTitle("Firmware reset");
        dialog->setLabelText("Loading new firmware onto Blinky");
        dialog->show();

        if (!loader->updateFirmware(*controller)) {
            // TODO: this might show an error message twice if the upload fails.
            showError(loader->getErrorString());
            dialog->close();
            return;
        }
    }
    mode = Uploading;
}

void MainWindow::on_actionSave_to_Blinky_triggered()
{

    if (patternCollection.isEmpty())
        return;

    if (controller.isNull())
        return;

    QPointer<BlinkyUploader> uploader;

    if (!controller->getPatternUploader(uploader)) {
        showError("Upload failed: Upload to this controller type not (yet) supported.");
        return;
    }

//    if (uploader.isNull())
//        return;

    if (uploader->getSupportedEncodings().count() == 0) {
        showError("Upload failed: Controller does not support any encodings.");
        return;
    }

    QList<PatternWriter> patternWriters;

    for (Pattern* pattern : patternCollection.patterns()) {
        PatternWriter patternWriter(*pattern,
                                    *fixture,
                                    uploader->getSupportedEncodings().front());
        patternWriters.append(patternWriter);
    }

    QProgressDialog* dialog = makeProgressDialog(uploader);

    dialog->setWindowTitle(tr("Blinky exporter"));
    dialog->setLabelText(tr("Saving to Blinky..."));
    dialog->show();

    if (!uploader->storePatterns(*controller, patternWriters)) {
        dialog->close();    // TODO: Does the dialog get deleted by this?

        showError(uploader->getErrorString());
        return;
    }
    mode = Uploading;
}

QProgressDialog* MainWindow::makeProgressDialog(BlinkyUploader *uploader) {
    QProgressDialog *dialog = new QProgressDialog(this);

    // Pre-set the upload progress dialog
    dialog->setMinimum(0);
    dialog->setMaximum(100);
    dialog->setWindowModality(Qt::WindowModal);

    connect(uploader, SIGNAL(finished(bool)),
            this, SLOT(on_uploaderFinished(bool)));

    connect(dialog, SIGNAL(canceled()),
            uploader, SLOT(cancel()));

    connect(uploader, SIGNAL(progressChanged(int)),
            dialog, SLOT(setValue(int)));

    connect(uploader, SIGNAL(setText(QString)),
            dialog, SLOT(setLabelText(QString)));

    connect(uploader, SIGNAL(finished(bool)),
            dialog, SLOT(accept()));

    return dialog;
}

QProgressDialog* MainWindow::makeProgressDialog(FirmwareLoader *uploader) {
    QProgressDialog *dialog = new QProgressDialog(this);

    // Pre-set the upload progress dialog
    dialog->setMinimum(0);
    dialog->setMaximum(100);
    dialog->setWindowModality(Qt::WindowModal);

    connect(uploader, SIGNAL(finished(bool)),
            this, SLOT(on_uploaderFinished(bool)));

    connect(dialog, SIGNAL(canceled()),
            uploader, SLOT(cancel()));

    connect(uploader, SIGNAL(progressChanged(int)),
            dialog, SLOT(setValue(int)));

    connect(uploader, SIGNAL(setText(QString)),
            dialog, SLOT(setLabelText(QString)));

    connect(uploader, SIGNAL(finished(bool)),
            dialog, SLOT(accept()));

    return dialog;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
#if defined(Q_OS_MACX)
    // Workaround for issue #114, multile close events are sent when closing from the dock
    if (!closeEventIntervalFilter.check()) {
        event->ignore();
        return;
    }
#endif

    QList<Pattern *> unsavedPatterns;

    for (Pattern* pattern : patternCollection.patterns())
        if (pattern->getModified())
            unsavedPatterns.append(pattern);

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

    settings.setValue("MainWindow/outputPreview", this->actionOutput_Preview->isChecked());

#if defined(Q_OS_MACX)    // Workaround for issue #114, multile close events are sent when closing from the dock
    closeEventIntervalFilter.force();
#endif

    QMainWindow::closeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    if(firstLoad) {
        firstLoad = false;
        emit windowLoaded();
    }
}

void MainWindow::on_instrumentSelected(bool)
{
    QAction *act = static_cast<QAction *>(sender());
    Q_ASSERT(act != NULL);
    for (QAction *a : instrumentToolbar->actions())
        a->setChecked(false);

    act->setChecked(true);
    frameEditor->setInstrument(qvariant_cast<AbstractInstrument *>(act->data()));
}

void MainWindow::on_colorPicked(QColor color)
{
    colorChooser.setColor(color);
    instrumentConfiguration.setToolColor(color);
}

bool MainWindow::promptForSave(Pattern *pattern)
{
    if (!pattern->getModified())
        return true;

    QList<Pattern *> patterns;
    patterns.append(pattern);
    return promptForSave(patterns);
}

bool MainWindow::promptForSave(QList<Pattern *> patterns)
{
    if(patterns.count() == 0)
        return true;

    QString messageText;

    if(patterns.count() == 1) {
        messageText += tr("The pattern %1 has been modified.")
                .arg(patterns.first()->getName());
    }
    else {
        messageText += tr("The following patterns have been modified:\n");

        for (Pattern *pattern : patterns)
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
        for (Pattern *pattern : patterns)
            if (!savePattern(pattern))
                return false;
        return true;
    }

    if (ans == QMessageBox::Discard)
        return true;

    return false;
}

void MainWindow::applyScene(const SceneTemplate &scene)
{
    // Test if any patterns need to be resized
    QList<Pattern *> needToResize;

    for (Pattern* pattern : patternCollection.patterns())
        if (pattern->getFrameSize() != scene.size)
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
            for (Pattern* pattern : needToResize)
                pattern->resize(scene.size, false);
        } else if (ans == QMessageBox::Cancel) {
            return;
        }
    }

    // Apply the new settings
    fixture = Fixture::makeFixture(scene.fixtureType, scene.size);

    fixture->setColorMode(scene.colorMode);
    fixture->setBrightnessModel(new ExponentialBrightness(1.8,1.8,2.1));

    // Wire in the fixture
    frameEditor->setFixture(fixture);
    outputPreview->setFixture(fixture);

    // If the new scene includes examples, load them as well
    // TODO: Separate this out into a different process?
    if (!scene.examples.isEmpty()) {
        QDir examplesDir(scene.examples);
        QFileInfoList examplesList = examplesDir.entryInfoList();

        for (QFileInfo fileinfo : examplesList) {
            if (!fileinfo.isDir()) {
                Pattern::PatternType type = Pattern::Scrolling;
                if (fileinfo.fileName().endsWith(".frames.png"))
                    type = Pattern::FrameBased;

                loadPattern(type,
                            scene.examples + "/" + fileinfo.fileName());
            }
        }

        patternCollectionListView->setCurrentIndex(patternCollectionListView->model()->index(0, 0));
    }

    // Finally, save the settings
    QSettings settings;
    settings.setValue("Fixture/Type", scene.fixtureType);
    settings.setValue("Fixture/DisplaySize", scene.size);
    settings.setValue("Fixture/ColorOrder", scene.colorMode);
    settings.setValue("BlinkyTape/firmwareName", scene.firmwareName);
}

bool MainWindow::loadPattern(Pattern::PatternType type, const QString fileName)
{
    QSettings settings;
    int frameCount = settings.value("Options/FrameCount", DEFAULT_FRAME_COUNT).toUInt();

    QSize displaySize(fixture->getExtents().width(), fixture->getExtents().height());

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

void MainWindow::setFrameIndex(int newIndex)
{
    if (patternCollection.isEmpty())
        return;

    // TODO: Test if the current pattern index is valid

    // TODO: Detect if we changed frames and only continue if it's a new frame...

    if (newIndex > getFrameCount())
        newIndex = getFrameCount() - 1;

    if (newIndex < 0)
        newIndex = 0;

    timeline->setCurrentIndex(timeline->model()->index(newIndex, 0));
    frameIndexWidget.setText(QString::number(newIndex+1));
}

void MainWindow::updateBlinky(const QImage &frame)
{
    if (controller.isNull())
        return;

    QList<QColor> colorStream = fixture->getColorStream(frame);
    QByteArray data;

    // TODO: This should be in a seperate preview output class?
    for (QColor color : colorStream)
        data.append(colorToBytes(fixture->getColorMode(), color));

    controller->sendUpdate(data);
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
    // TODO: we're going to have to unload our references, but for now skip that.
    if (!current.isValid()) {

        timeline->setModel(NULL);
        timeline->setVisible(false);

        undoGroup.setActiveStack(NULL);

        setPatternName("()");
        setPatternModified(false);
        editImageChanged(0, QImage());
        frameImageChanged(QImage());
        frameEditor->setShowPlaybakIndicator(false);

        patternSpeed->setValue(1);
        frameIndexWidget.setText("");

        actionSave_to_Blinky->setEnabled(false);

        emit(patternStatusChanged(false));
        return;
    }

    int previousFrameIndex = getCurrentFrameIndex();
    Pattern *newpattern = patternCollection.at(current.row());

    timeline->setModel(newpattern->getModel());
    timeline->setVisible(newpattern->hasTimeline());

    undoGroup.setActiveStack(newpattern->getUndoStack());

    setFrameIndex(previousFrameIndex);
    setPatternName(newpattern->getName());
    setPatternModified(newpattern->getModified());
    editImageChanged(getCurrentFrameIndex(), newpattern->getEditImage(getCurrentFrameIndex()));
    frameImageChanged(newpattern->getFrameImage(getCurrentFrameIndex()));
    frameEditor->setShowPlaybakIndicator(newpattern->hasPlaybackIndicator());

    patternSpeed->setValue(newpattern->getFrameSpeed());

    actionSave_to_Blinky->setEnabled(mode == Connected);

    if (timelineSelectedChangedConnection)
        QObject::disconnect(timelineSelectedChangedConnection);

    if (patternDataChangedConnection)
        QObject::disconnect(patternDataChangedConnection);


    timelineSelectedChangedConnection =
    connect(timeline->selectionModel(),
            SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(on_timelineSelectedChanged(const QModelIndex &, const QModelIndex &)));

    patternDataChangedConnection =
    connect(timeline->model(),
            SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)),
            this,
            SLOT(on_PatternDataChanged(const QModelIndex &, const QModelIndex &,
                                       const QVector<int> &)));

    emit(patternStatusChanged(true));
}

void MainWindow::on_timelineSelectedChanged(const QModelIndex &current, const QModelIndex &)
{
    frameIndexWidget.setText(QString::number(current.row() + 1));

    editImageChanged(current.row(),
                   patternCollection.at(getCurrentPatternIndex())->getEditImage(current.row()));
    frameImageChanged(patternCollection.at(getCurrentPatternIndex())->getFrameImage(current.row()));
}

void MainWindow::on_PatternDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                       const QVector<int> &roles)
{
    int currentIndex = getCurrentFrameIndex();

    for (int role : roles) {
        if (role == PatternModel::FileName) {
            setPatternName(patternCollection.at(getCurrentPatternIndex())->getName());
        }
        else if (role == PatternModel::Modified) {
            setPatternModified(patternCollection.at(getCurrentPatternIndex())->getModified());
        }
        else if (role == PatternModel::FrameImage) {
            // If the current selection changed, refresh so that the FrameEditor contents will be redrawn
            if (currentIndex < topLeft.row() || currentIndex > bottomRight.row())
                continue;

            editImageChanged(getCurrentFrameIndex(),
                           patternCollection.at(getCurrentPatternIndex())->getEditImage(
                               getCurrentFrameIndex()));
            frameImageChanged(patternCollection.at(getCurrentPatternIndex())->getFrameImage(
                               getCurrentFrameIndex()));
        }
        else if (role == PatternModel::FrameSpeed) {
            patternSpeed->setValue(patternCollection.at(getCurrentPatternIndex())->getFrameSpeed());
        }
    }
}

void MainWindow::editImageChanged(int index, const QImage &data)
{
    frameEditor->setEditImage(index, data);
}

void MainWindow::frameImageChanged(const QImage &data)
{
    outputPreview->setFrameImage(data);

    updateBlinky(data);
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

    setFrameIndex((getCurrentFrameIndex()+1)%getFrameCount());
}

void MainWindow::on_actionStepBackward_triggered()
{
    if (patternCollection.isEmpty())
        return;

    setFrameIndex(getCurrentFrameIndex() <= 0 ? getFrameCount()-1 : getCurrentFrameIndex()-1);
}

void MainWindow::on_actionAddFrame_triggered()
{
    if (patternCollection.isEmpty())
        return;

    patternCollection.at(getCurrentPatternIndex())->addFrame(getCurrentFrameIndex());
    setFrameIndex(getCurrentFrameIndex()-1);
}

void MainWindow::on_actionDeleteFrame_triggered()
{
    if (patternCollection.isEmpty())
        return;

    patternCollection.at(getCurrentPatternIndex())->deleteFrame(getCurrentFrameIndex());
    setFrameIndex(getCurrentFrameIndex());
}

void MainWindow::setPatternModified(bool modified)
{
    actionSave->setEnabled(modified);
}

void MainWindow::on_ExampleSelected(QAction *action)
{
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

void MainWindow::on_actionConfigure_Scene_triggered()
{
    SceneConfiguration sceneConfiguration(this);

    // TODO: Keep a persistant scene around that's loaded/saved with program start
    SceneTemplate sceneTemplate;

    if (!controller.isNull()) {
        sceneTemplate.controllerType = controller->getName();
    }

    if (!fixture.isNull()) {
        sceneTemplate.fixtureType = fixture->getName();
        sceneTemplate.colorMode = fixture->getColorMode();
        sceneTemplate.size = QSize(fixture->getExtents().width(),
                                   fixture->getExtents().height());
    }

    QSettings settings;

    // TODO: roll this into the persistant sceneTemplate
    sceneTemplate.firmwareName = settings.value("BlinkyTape/firmwareName", BLINKYTAPE_DEFAULT_FIRMWARE_NAME).toString();


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

    preferences->setUpdater(autoUpdater);

    preferences->show();
}

void MainWindow::on_actionWelcome_triggered()
{
    WelcomeScreen *welcomeScreen = new WelcomeScreen(this);

    connect(welcomeScreen, SIGNAL(sceneSelected(SceneTemplate)),
            this, SLOT(applyScene(SceneTemplate)));

    welcomeScreen->show();
}

void MainWindow::on_actionSave_All_triggered()
{
    for (Pattern *pattern : patternCollection.patterns())
        if (!savePattern(pattern))
            return;
}

void MainWindow::on_actionClose_All_triggered()
{
    QList<Pattern *> unsavedPatterns;

    for (Pattern* pattern : patternCollection.patterns()) {
        if (pattern->getModified())
            unsavedPatterns.append(pattern);
    }

    if (!promptForSave(unsavedPatterns))
        return;

    patternCollection.clear();
}

void MainWindow::on_actionDebug_Log_triggered()
{
    DebugLog* debugLog = new DebugLog(this);
    debugLog->show();
}
