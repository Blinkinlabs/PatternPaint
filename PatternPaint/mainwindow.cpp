#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "colormodel.h"
#include "systeminformation.h"
#include "aboutpatternpaint.h"
#include "fixturesettings.h"
#include "colorchooser.h"
#include "blinkytape.h"
#include "patternwriter.h"

#include "pencilinstrument.h"
#include "lineinstrument.h"
#include "colorpickerinstrument.h"
#include "sprayinstrument.h"
#include "fillinstrument.h"
#include "pattern.h"
#include "patterncollectiondelegate.h"

#include "patternframemodel.h"
#include "patterndelegate.h"
#include "patternCollection.h"
#include "fixture.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QtWidgets>

#include <blinkytapeuploader.h>


#define PATTERN_SPEED_MINIMUM_VALUE 1
#define PATTERN_SPEED_MAXIMUM_VALUE 100
#define PATTERN_SPEED_DEFAULT_VALUE 20

#define DRAWING_SIZE_MINIMUM_VALUE 1
#define DRAWING_SIZE_MAXIMUM_VALUE 20
#define DRAWING_SIZE_DEFAULT_VALUE 1

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)
#define COLOR_TOOL_DEFAULT    QColor(255,255,255,255)

#define DEFAULT_DISPLAY_WIDTH 1
#define DEFAULT_DISPLAY_HEIGHT 60
#define DEFAULT_FRAME_COUNT 8

#define MIN_TIMER_INTERVAL 5  // minimum interval to wait between blinkytape updates

#define CONNECTION_SCANNER_INTERVAL 1000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    colorChooser(COLOR_TOOL_DEFAULT, this),
    progressDialog(this)
{
    setupUi(this);

#if defined(Q_OS_MAC)
    appNap = NULL;
#endif

#if defined(Q_OS_LINUX)
    // If we're on linux, we probably don't have a desktop icon registered,
    // so set one manually
    setWindowIcon(QIcon(":/resources/images/patternpaint.ico"));
#endif

    // prepare undo/redo
    menuEdit->addSeparator();

    QAction* undoAction = undoGroup.createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Z")));
    undoAction->setEnabled(false);
    menuEdit->addAction(undoAction);

    QAction* redoAction = undoGroup.createRedoAction(this, tr("&Redo"));
    redoAction->setEnabled(false);
    redoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Y")));
    menuEdit->addAction(redoAction);

    // instruments
    ColorpickerInstrument* cpi = new ColorpickerInstrument(this);
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
    pSpeed.setRange(PATTERN_SPEED_MINIMUM_VALUE, PATTERN_SPEED_MAXIMUM_VALUE);
    pSpeed.setValue(PATTERN_SPEED_DEFAULT_VALUE);
    pSpeed.setToolTip(tr("Pattern speed"));
    playbackToolbar->addWidget(&pSpeed);
    connect(&pSpeed, SIGNAL(valueChanged(int)), this, SLOT(patternSpeed_valueChanged(int)));
    patternSpeed_valueChanged(PATTERN_SPEED_DEFAULT_VALUE);

    pFrame.setMaximumWidth(30);
    pFrame.setMinimumWidth(30);
    pFrame.setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
    pFrame.setToolTip(tr("Current frame"));
    playbackToolbar->insertWidget(actionStepForward, &pFrame);
    connect(&pFrame, SIGNAL(textEdited(QString)), this, SLOT(frameIndex_valueChanged(QString)));
    frameIndex_valueChanged("1");


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
            &pSpeed, SLOT(setEnabled(bool)));

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
    progressDialog.setMaximum(150);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setAutoClose(false);

    // The draw timer tells the pattern to advance
    connect(&drawTimer, SIGNAL(timeout()), this, SLOT(drawTimer_timeout()));

    // Start a scanner to connect to a Blinky automatically
    connect(&connectionScannerTimer, SIGNAL(timeout()), this, SLOT(connectionScannerTimer_timeout()));
    connectionScannerTimer.setInterval(CONNECTION_SCANNER_INTERVAL);
    connectionScannerTimer.start();

    actionPen->setChecked(true);
    frameEditor->setInstrument(qvariant_cast<AbstractInstrument*>(actionPen->data()));

    QSettings settings;

    resize(settings.value("MainWindow/size", QSize(880, 450)).toSize());
    move(settings.value("MainWindow/pos", QPoint(100, 100)).toPoint());

    colorMode = settings.value("Options/ColorOrder", PatternWriter::RGB).value<PatternWriter::ColorMode>();

    // Fill the examples menu using the examples resource
    populateExamplesMenu(":/examples", menuExamples);
    connect(menuExamples, SIGNAL(triggered(QAction *)),
            this, SLOT(on_ExampleSelected(QAction *)), Qt::UniqueConnection);


    patternCollectionListView->setItemDelegate(new PatternCollectionDelegate(this));
    patternCollectionListView->setModel(patternCollection.getModel());

    connect(patternCollectionListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(on_patternCollectionCurrentChanged(const QModelIndex &, const QModelIndex &)));

    timeline->setItemDelegate(new PatternDelegate(this));

    // Create a pattern
    on_actionNew_triggered();
}

void MainWindow::populateExamplesMenu(QString directory, QMenu* menu) {
    QDir examplesDir(directory);
    QFileInfoList examplesList = examplesDir.entryInfoList();

    for(int i = 0; i < examplesList.size(); ++i) {
        // If we found a directory, create a submenu and call ourselves again to populate it
        if(examplesList.at(i).isDir()) {
            QMenu* submenu = new QMenu(this);
            submenu->setTitle(examplesList.at(i).fileName());
            menu->addMenu(submenu);
            connect(submenu, SIGNAL(triggered(QAction *)),
                    this, SLOT(on_ExampleSelected(QAction *)), Qt::UniqueConnection);

            populateExamplesMenu(directory + "/" + examplesList.at(i).fileName(), submenu);
        }
        // Otherwise this is a file, so add it to the examples menu.
        else {
            QAction* action = new QAction(examplesList.at(i).baseName(), this);
            action->setObjectName(directory + "/" + examplesList.at(i).fileName());
            menu->addAction(action);
        }
    }
}

MainWindow::~MainWindow(){
#if defined(Q_OS_MAC)
    // start the app nap inhibitor
    if(appNap != NULL) {
        delete appNap;
        appNap = NULL;
    }
#endif

}

int MainWindow::getCurrentPatternIndex() {
    if(!patternCollectionListView->currentIndex().isValid()) {
        return 0;
    }

    return patternCollectionListView->currentIndex().row();
}

int MainWindow::getPatternCount() {
    if(patternCollectionListView->model() == NULL) {
        return 0;
    }

    return patternCollectionListView->model()->rowCount();
}


int MainWindow::getCurrentFrameIndex() {
    if(!timeline->currentIndex().isValid()) {
        return 0;
    }


    return timeline->currentIndex().row();
}

int MainWindow::getFrameCount() {
    if(timeline->model() == NULL) {
        return 0;
    }

    return timeline->model()->rowCount();
}


void MainWindow::drawTimer_timeout() {
    if(getFrameCount() == 0) {
        return;
    }

    setNewFrame((getCurrentFrameIndex()+1)%getFrameCount());
}


void MainWindow::connectionScannerTimer_timeout() {
    // If we are already connected, disregard.
    if((!controller.isNull()) || mode==Uploading) {
        return;
    }

    // First look for Blinky devices
    QList<QSerialPortInfo> blinky = BlinkyTape::probe();

    if(blinky.length() > 0) {
        qDebug() << "BlinkyTapes found:" << blinky.length();

        // TODO: Try another one if this one fails?
        qDebug() << "Attempting to connect to tape on:" << blinky[0].portName();

        controller = new BlinkyTape(this);
        connectController();
        controller->open(blinky[0]);
        return;
    }
}

void MainWindow::patternSpeed_valueChanged(int value)
{
    drawTimer.setInterval(1000/value);
}

void MainWindow::frameIndex_valueChanged(QString value)
{
    setNewFrame(value.toInt() - 1);
}

void MainWindow::on_actionPlay_triggered()
{
    if (drawTimer.isActive()) {
        stopPlayback();
    } else {
        startPlayback();
    }
}

void MainWindow::startPlayback() {
    drawTimer.start();
    actionPlay->setText(tr("Pause"));
    actionPlay->setIcon(QIcon(":/icons/images/icons/Pause-100.png"));
}

void MainWindow::stopPlayback() {
    drawTimer.stop();
    actionPlay->setText(tr("Play"));
    actionPlay->setIcon(QIcon(":/icons/images/icons/Play-100.png"));
}

void MainWindow::on_actionLoad_File_triggered()
{
    QSettings settings;
    QString lastDirectory = settings.value("File/LoadDirectory").toString();

    QDir dir(lastDirectory);
    if(!dir.isReadable()) {
        lastDirectory = QDir::homePath();
    }

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Pattern"), lastDirectory, tr("Pattern Files (*.png *.jpg *.bmp *.gif)"));

    if(fileName.length() == 0) {
        return;
    }

    QFileInfo fileInfo(fileName);
    settings.setValue("File/LoadDirectory", fileInfo.absolutePath());

    // TODO: choose scrolling/frame-based
    if(!loadPattern(Pattern::Scrolling, fileName)) {
        showError(tr("Could not open file %1. Perhaps it has a formatting problem?")
                  .arg(fileName));
        return;
    }
}


bool MainWindow::savePatternAs(Pattern *item) {
    QSettings settings;
    QString lastDirectory = settings.value("File/SaveDirectory").toString();

    QDir dir(lastDirectory);
    if(!dir.isReadable()) {
        lastDirectory = QDir::homePath();
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Pattern"), lastDirectory, tr("Pattern Files (*.png *.jpg *.bmp)"));

    if(fileName.length() == 0)
        return false;

    QFileInfo fileInfo(fileName);
    settings.setValue("File/SaveDirectory", fileInfo.absolutePath());

    if (!item->saveAs(fileInfo.absoluteFilePath()))
        showError(tr("Error saving pattern %1. Try saving it somewhere else?")
                       .arg(fileInfo.absolutePath()));

    return (!item->getModified());
}

bool MainWindow::savePattern(Pattern *item) {
    if(!item->hasValidFilename()) {
        return savePatternAs(item);
    } else {
        if (!item->save()) {
            showError(tr("Error saving pattern %1. Try saving it somewhere else?")
                      .arg(item->getPatternName()));
        }
        return (!item->getModified());
    }
}

void MainWindow::on_actionSave_File_as_triggered() {
    if(!patternCollection.hasPattern()) {
        return;
    }
    savePatternAs(patternCollection.getPattern(getCurrentPatternIndex()));
}

void MainWindow::on_actionSave_File_triggered() {
    if(!patternCollection.hasPattern()) {
        return;
    }
    savePattern(patternCollection.getPattern(getCurrentPatternIndex()));
}

void MainWindow::on_actionExit_triggered() {
    this->close();
}

void MainWindow::on_actionExport_pattern_for_Arduino_triggered() {
    // TODO: Merge this with save

    if(!patternCollection.hasPattern()) {
        return;
    }

    QSettings settings;
    QString lastDirectory = settings.value("File/ExportArduinoDirectory").toString();

    QDir dir(lastDirectory);
    if(!dir.isReadable()) {
        lastDirectory = QDir::homePath();
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Pattern for Arduino"), lastDirectory, tr("Header File (*.h)"));

    if(fileName.length() == 0) {
        return;
    }

    QFileInfo fileInfo(fileName);
    settings.setValue("File/ExportArduinoDirectory", fileInfo.absolutePath());

    // Note: Converting frameRate to frame delay here.
    PatternWriter patternWriter(patternCollection.getPattern(getCurrentPatternIndex()),
                                drawTimer.interval(),
                                PatternWriter::RGB24,
                                colorMode);


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
    actionSave_to_Blinky->setEnabled(connected);

    if(connected) {
        mode = Connected;
        startPlayback();

#if defined(Q_OS_MAC)
        // start the app nap inhibitor
        if(appNap == NULL) {
            appNap = new CAppNapInhibitor("Interaction with hardware");
        }
#endif

    }
    else {
        mode = Disconnected;
        stopPlayback();

        // TODO: Does this delete the serial object reliably?
        controller.clear();

        connectionScannerTimer.start();

#if defined(Q_OS_MAC)
        // start the app nap inhibitor
        if(appNap != NULL) {
            delete appNap;
            appNap = NULL;
        }
#endif
    }
}

void MainWindow::on_actionAbout_triggered()
{
    // TODO: store this somewhere, for later disposal.
    AboutPatternPaint* info = new AboutPatternPaint(this);
    info->show();
}

void MainWindow::on_actionSystem_Information_triggered()
{
    // TODO: store this somewhere, for later disposal.
    SystemInformation* info = new SystemInformation(this);
    info->show();
}

void MainWindow::on_uploaderMaxProgressChanged(int progressValue)
{
    if(progressDialog.isHidden()) {
        qDebug() << "Got a progress event while the progress dialog is hidden, event order problem?";
        return;
    }

    progressDialog.setMaximum(progressValue);
}

void MainWindow::on_uploaderProgressChanged(int progressValue)
{
    if(progressDialog.isHidden()) {
        qDebug() << "Got a progress event while the progress dialog is hidden, event order problem?";
        return;
    }

    // Clip the progress to maximum, until we work out a better way to estimate it.
    if(progressValue >= progressDialog.maximum()) {
        progressValue = progressDialog.maximum() - 1;
    }

    progressDialog.setValue(progressValue);
}

void MainWindow::on_uploaderFinished(bool result)
{
    mode = Disconnected;
    uploader.clear();

    progressDialog.hide();

    qDebug() << "Uploader finished! Result:" << result;
    if(!result) {
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
    QDesktopServices::openUrl(QUrl("http://blinkinlabs.com/blinkytape/docs/troubleshooting/", QUrl::TolerantMode));
}

void MainWindow::on_actionFlip_Horizontal_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    QImage frame = patternCollection.getPattern(getCurrentPatternIndex())->getEditImage(getCurrentFrameIndex());
    frame = frame.mirrored(true, false);
    patternCollection.getPattern(getCurrentPatternIndex())->setEditImage(getCurrentFrameIndex(),frame);
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    QImage frame = patternCollection.getPattern(getCurrentPatternIndex())->getEditImage(getCurrentFrameIndex());
    frame = frame.mirrored(false, true);
    patternCollection.getPattern(getCurrentPatternIndex())->setEditImage(getCurrentFrameIndex(),frame);
}

void MainWindow::on_actionClear_Pattern_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    QImage frame = patternCollection.getPattern(getCurrentPatternIndex())->getEditImage(getCurrentFrameIndex());
    frame.fill(COLOR_CANVAS_DEFAULT);
    patternCollection.getPattern(getCurrentPatternIndex())->setEditImage(getCurrentFrameIndex(),frame);
}

void MainWindow::showError(QString errorMessage) {
    QMessageBox box(this);
    box.setWindowModality(Qt::WindowModal);
    box.setText(errorMessage);
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}

void MainWindow::on_actionLoad_rainbow_sketch_triggered()
{   
    // If the controller doesn't exist, create a new uploader based on the blinkytape
    if(controller.isNull()) {
        uploader = QPointer<PatternUploader>(new BlinkyTapeUploader(this));

        connectUploader();

        if(!uploader->upgradeFirmware(-1)) {
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
        if (!controller->getUploader(uploader)) {
            return;
        }

        if(uploader.isNull()) {
            return;
        }

        connectUploader();

        if(!uploader->upgradeFirmware(*controller)) {
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
    if(controller.isNull()) {
        return;
    }

    if(!patternCollection.hasPattern()) {
        return;
    }

    std::vector<PatternWriter> patterns;

    for(int i = 0; i < patternCollection.count(); i++) {
        PatternWriter patternWriter(patternCollection.getPattern(i),
                        drawTimer.interval(),
                        //PatternWriter::RGB24,
                        PatternWriter::RGB565_RLE,
                        colorMode);

        patterns.push_back(patternWriter);
    }

    if(!controller->getUploader(uploader)) {
        return;
    }

    if(uploader.isNull()) {
        return;
    }

    connectUploader();

    if(!uploader->startUpload(*controller, patterns)) {
        showError(uploader->getErrorString());
        return;
    }
    mode = Uploading;

    progressDialog.setWindowTitle("Blinky exporter");
    progressDialog.setLabelText("Saving pattern to Blinky...");

    progressDialog.setValue(progressDialog.minimum());
    progressDialog.show();
}


void MainWindow::on_actionResize_Pattern_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    FixtureSettings fixtureSettings(this);
    fixtureSettings.setWindowModality(Qt::WindowModal);
    fixtureSettings.setOutputSize(patternCollection.getPattern(getCurrentPatternIndex())->getEditImage(0).size());
    fixtureSettings.setColorMode(colorMode);

    fixtureSettings.exec();

    if(fixtureSettings.result() != QDialog::Accepted) {
        return;
    }

    QSize newDisplaySize = fixtureSettings.getOutputSize();
    colorMode = fixtureSettings.getColorMode();

    // Push this to a function?
    QSettings settings;
    settings.setValue("Options/DisplaySize", newDisplaySize);
    settings.setValue("Options/ColorOrder", colorMode);

    for(int i = 0; i < patternCollection.count(); i++) {
        // Resize the pattern
        patternCollection.getPattern(i)->resize(newDisplaySize,false);
    }
}

void MainWindow::on_actionAddress_programmer_triggered()
{
    AddressProgrammer programmer(this);
    programmer.setWindowModality(Qt::WindowModal);
    programmer.exec();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    std::vector<Pattern*> unsaved;

    for(int i = 0; i < patternCollection.count(); i++) {
        // Convert the current pattern into a Pattern
        Pattern* pattern = patternCollection.getPattern(i);

        if (pattern->getModified() == true) {
            unsaved.push_back(pattern);
        }
    }

    if(unsaved.size() == 1) {
        // If we only have one pattern, show the regular prompt for save dialog
        if(!promptForSave(unsaved.front())) {
            event->ignore();
            return;
        }
    }
    else if(unsaved.size() > 1) {
        // If we only have one pattern, show the regular prompt for save dialog
        if(!promptForSave(unsaved)) {
            event->ignore();
            return;
        }
    }

    QSettings settings;
    settings.setValue("MainWindow/size", size());
    settings.setValue("MainWindow/pos", pos());

    // If we are connected to a blinky, try to reset it so that it will start playing back its pattern
    if(!controller.isNull()) {
        qDebug() << "Attempting to reset blinky";
        controller->reset();
    }

    event->accept();
}


void MainWindow::on_actionConnect_triggered()
{
    // If we were already connected, disconnect here
    if(!controller.isNull()) {
        qDebug() << "Disconnecting from tape";
        controller->close();

        return;
    }

    // Otherwise, search for a controller.
    connectionScannerTimer_timeout();
}

void MainWindow::on_instrumentSelected(bool) {
    QAction* act = static_cast<QAction*>(sender());
    Q_ASSERT(act != NULL);
    foreach(QAction* a, instrumentToolbar->actions()) {
        a->setChecked(false);
    }

    act->setChecked(true);
    frameEditor->setInstrument(qvariant_cast<AbstractInstrument*>(act->data()));
}

void MainWindow::on_colorPicked(QColor color) {
    colorChooser.setColor(color);
    frameEditor->setToolColor(color);
}

bool MainWindow::promptForSave(Pattern * item) {
    if (item->getModified() == false) {
        return true;
    }

    QString messageText = tr("The pattern %1 has been modified.")
            .arg(item->getPatternName());

    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setIconPixmap(QPixmap::fromImage(item->getEditImage(0)));
    msgBox.setText(messageText);
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ans = msgBox.exec();

    if (ans == QMessageBox::Save) {
        return savePattern(item);
    }

    if (ans == QMessageBox::Cancel) {
        return false;
    }

    if (ans == QMessageBox::Discard) {
        return true;
    }

    return false;
}

bool MainWindow::promptForSave(std::vector<Pattern*> patterns) {
    QString messageText = tr("The following patterns have been modified:\n");

    for(std::size_t i = 0; i < patterns.size(); i++) {
        messageText += patterns.at(i)->getPatternName();
        messageText += "\n";
    }

    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setText(messageText);
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ans = msgBox.exec();

    if (ans == QMessageBox::Save) {
        for(std::size_t i = 0; i < patterns.size(); i++) {
            if(!savePattern(patterns.at(i))) {
                return false;
            }
        }
        return true;
    }
    else if (ans == QMessageBox::Discard) {
        return true;
    }
    else { // Cancel
        return false;
    }
}

void MainWindow::connectController()
{
    // Modify our UI when the tape connection status changes
    connect(controller, SIGNAL(connectionStatusChanged(bool)),
            this,SLOT(on_blinkyConnectionStatusChanged(bool)));
}

void MainWindow::connectUploader()
{
    // TODO: Should this be a separate view?
    connect(uploader, SIGNAL(maxProgressChanged(int)),
            this, SLOT(on_uploaderMaxProgressChanged(int)));
    connect(uploader, SIGNAL(progressChanged(int)),
            this, SLOT(on_uploaderProgressChanged(int)));
    connect(uploader, SIGNAL(finished(bool)),
            this, SLOT(on_uploaderFinished(bool)));

}

bool MainWindow::loadPattern(Pattern::PatternType type, const QString fileName)
{
    QSettings settings;
    int frameCount = settings.value("Options/FrameCount", DEFAULT_FRAME_COUNT).toUInt();

    QSize displaySize;
    if(!patternCollection.hasPattern()) {
        displaySize = settings.value("Options/DisplaySize", QSize(DEFAULT_DISPLAY_WIDTH, DEFAULT_DISPLAY_HEIGHT)).toSize();
    }
    else {
        displaySize =  patternCollection.getPattern(getCurrentPatternIndex())->getFrameSize();
    }

    Pattern *pattern = new Pattern(type, displaySize, frameCount);

    // TODO: Fix load procedure fit the pattern type
    if(!fileName.isEmpty())
        if(!pattern->load(fileName))
            return false;

    int newPosition = 0;
    if(getPatternCount() > 0)
        newPosition = getCurrentPatternIndex()+1;

    patternCollection.addPattern(pattern, newPosition);
    patternCollectionListView->setCurrentIndex(patternCollectionListView->model()->index(newPosition,0));
    return true;
}

void MainWindow::setNewFrame(int newFrame)
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    // TODO: Detect if we changed frames and only continue if it's a new frame...

    if(newFrame > getFrameCount()) {
        newFrame = getFrameCount() - 1;
    }
    if(newFrame < 0) {
        newFrame = 0;
    }

    timeline->setCurrentIndex(timeline->model()->index(newFrame,0));

    pFrame.setText(QString::number(getCurrentFrameIndex()+1));

    setPatternData(getCurrentFrameIndex(),
                   patternCollection.getPattern(getCurrentPatternIndex())->getEditImage(newFrame));
}

void MainWindow::updateBlinky()
{
    if(controller.isNull()) {
        return;
    }

    if(!patternCollection.hasPattern()) {
        return;
    }

    QImage frame = patternCollection.getPattern(getCurrentPatternIndex())->getFrameImage(getCurrentFrameIndex());
    MatrixFixture fixture(patternCollection.getPattern(getCurrentPatternIndex())->getFrameSize());

    QList<QColor> pixels = fixture.getColorStreamForFrame(frame);

    QByteArray ledData;
    for(int i = 0; i < pixels.size(); i++) {
        QColor color = ColorModel::correctBrightness(pixels[i]);

        switch(colorMode) {
        case PatternWriter::RGB:
            ledData.append(color.red());
            ledData.append(color.green());
            ledData.append(color.blue());
            break;
        case PatternWriter::GRB:
            ledData.append(color.green());
            ledData.append(color.red());
            ledData.append(color.blue());
            break;
        case PatternWriter::COLOR_MODE_COUNT:
        default:
            break;
        }
    }

    controller->sendUpdate(ledData);
}

void MainWindow::on_actionNew_triggered()
{
    loadPattern(Pattern::Scrolling, QString());
}

void MainWindow::on_actionClose_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    if(!promptForSave(patternCollection.getPattern(getCurrentPatternIndex()))) {
        return;
    }

    // TODO: remove the undo stack from the undo group?
    patternCollection.removePattern(getCurrentPatternIndex());
}

void MainWindow::on_patternCollectionCurrentChanged(const QModelIndex &current, const QModelIndex &) {

    emit(patternStatusChanged(current.isValid()));

    on_patternSizeUpdated();

    // TODO: we're going to have to unload our references, but for now skip that.
    if(!current.isValid()) {
        undoGroup.setActiveStack(NULL);
        timeline->setModel(NULL);

        setPatternName("()");
        setPatternModified(false);
        setPatternData(0, QImage());
        return;
    }

    Pattern* newpattern = patternCollection.getPattern(current.row());

    undoGroup.setActiveStack(newpattern->getUndoStack());
    timeline->setModel(newpattern->getModel());


    setPatternName(newpattern->getPatternName());
    setPatternModified(newpattern->getModified());
    setPatternData(getCurrentFrameIndex(), newpattern->getEditImage(getCurrentPatternIndex()));


    // TODO: Should we unregister these eventually?
    connect(timeline->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(on_timelineSelectedChanged(const QModelIndex &, const QModelIndex &)));
    connect(timeline->model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)),
            this, SLOT(on_timelineDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));
}


void MainWindow::on_timelineSelectedChanged(const QModelIndex &current, const QModelIndex &) {
    setNewFrame(current.row());
}

void MainWindow::on_timelineDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {
    int currentIndex = getCurrentFrameIndex();

    for(int i = 0; i < roles.count(); i++) {
        if(roles[i] == PatternFrameModel::FileName)
            setPatternName(patternCollection.getPattern(getCurrentPatternIndex())->getPatternName());

        else if(roles[i] == PatternFrameModel::Modified)
            setPatternModified(patternCollection.getPattern(getCurrentPatternIndex())->getModified());

        else if(roles[i] == PatternFrameModel::FrameImage) {
            // If the current selection changed, refresh so that the FrameEditor contents will be redrawn
            if(currentIndex >= topLeft.row() && currentIndex <= bottomRight.row()) {
                setPatternData(getCurrentFrameIndex(),
                               patternCollection.getPattern(getCurrentPatternIndex())->getEditImage(getCurrentFrameIndex()));
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
    if(!patternCollection.hasPattern()) {
        setPatternData(0, QImage());
        return;
    }

    setPatternData(getCurrentFrameIndex(),
                   patternCollection.getPattern(getCurrentPatternIndex())->getEditImage(getCurrentFrameIndex()));

    // And kick the scroll area so that it will size itself
    //scrollArea->resize(scrollArea->width()+1, scrollArea->height());
    //this->patternsplitter->refresh();
}

void MainWindow::on_frameDataEdited(int index, QImage update)
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    patternCollection.getPattern(getCurrentPatternIndex())->setEditImage(index, update);
}


void MainWindow::setPatternName(QString name)
{
    this->setWindowTitle(name + " - Pattern Paint");
}

void MainWindow::on_actionStepForward_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    setNewFrame((getCurrentFrameIndex()+1)%getFrameCount());
}

void MainWindow::on_actionStepBackward_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    setNewFrame(getCurrentFrameIndex()<=0?getFrameCount()-1:getCurrentFrameIndex()-1);
}

void MainWindow::on_actionTimeline_triggered()
{
}

void MainWindow::on_actionMatrix_triggered()
{
}

void MainWindow::on_actionAddFrame_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    patternCollection.getPattern(getCurrentPatternIndex())->addFrame(getCurrentFrameIndex());
    setNewFrame(getCurrentFrameIndex()-1);
}

void MainWindow::on_actionDeleteFrame_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    patternCollection.getPattern(getCurrentPatternIndex())->deleteFrame(getCurrentFrameIndex());
    setNewFrame(getCurrentFrameIndex());
}

void MainWindow::setPatternModified(bool modified)
{
    actionSave_File->setEnabled(modified);
}

void MainWindow::on_ExampleSelected(QAction* action) {
    qDebug() << "Example selected:" << action->objectName();

    // TODO: Choose scrolling/frame based
    if(!loadPattern(Pattern::Scrolling, action->objectName())) {
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
