#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "colormodel.h"
#include "systeminformation.h"
#include "aboutpatternpaint.h"
#include "resizepattern.h"
#include "undocommand.h"
#include "colorchooser.h"
#include "blinkytape.h"
#include "patternoutput.h"

#include "pencilinstrument.h"
#include "lineinstrument.h"
#include "colorpickerinstrument.h"
#include "sprayinstrument.h"
#include "fillinstrument.h"
#include "pattern.h"
#include "patterncollectiondelegate.h"

#include "patternframemodel.h"
#include "patternframedelegate.h"
#include "patternCollection.h"

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
    patternEditor->setToolColor(COLOR_TOOL_DEFAULT);

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
            patternEditor, SLOT(setToolColor(QColor)));
    connect(penSizeSpin, SIGNAL(valueChanged(int)),
            patternEditor, SLOT(setToolSize(int)));

    connect(patternEditor, SIGNAL(frameDataUpdated(int, const QImage)),
            this, SLOT(on_frameDataUpdated(int, const QImage)));

    patternEditor->setToolSize(DRAWING_SIZE_MINIMUM_VALUE);

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
    patternEditor->setInstrument(qvariant_cast<AbstractInstrument*>(actionPen->data()));

    QSettings settings;

    resize(settings.value("MainWindow/size", QSize(880, 450)).toSize());
    move(settings.value("MainWindow/pos", QPoint(100, 100)).toPoint());

    setColorMode(static_cast<PatternOutput::ColorMode>(settings.value("Options/ColorOrder", PatternOutput::RGB).toUInt()));

    // Fill the examples menu using the examples resource
    populateExamplesMenu(":/examples", menuExamples);
    connect(menuExamples, SIGNAL(triggered(QAction *)),
            this, SLOT(on_ExampleSelected(QAction *)), Qt::UniqueConnection);


    patternCollectionListView->setItemDelegate(new PatternCollectionDelegate(this));
    patternCollectionListView->setModel(patternCollection.getModel());

    connect(patternCollectionListView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(on_patternSelected(const QModelIndex &, const QModelIndex &)));

    timeline->setItemDelegate(new PatternFrameDelegate(this));

    // Create a pattern.
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

    QSize displaySize;
    displaySize.setWidth(settings.value("Options/displayWidth", DEFAULT_DISPLAY_WIDTH).toUInt());
    displaySize.setHeight(settings.value("Options/displayHeight", DEFAULT_DISPLAY_HEIGHT).toUInt());

    // Create a pattern, and attempt to load the file
    // TODO: Can't there be a constructor that accepts a file directly? This seems odd
    Pattern * pattern = new Pattern(displaySize,1);

    if(!pattern->load(fileInfo)) {
        showError("Could not open file "
                   + fileName
                   + ". Perhaps it has a formatting problem?");
        return;
    }

    patternCollection.addPattern(pattern);
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

    if(fileName.length() == 0) {
        return false;
    }

    QFileInfo fileInfo(fileName);
    settings.setValue("File/SaveDirectory", fileInfo.absolutePath());

    if (!item->saveAs(fileInfo)) {
        QMessageBox::warning(this, tr("Error"), tr("Error saving pattern %1. Try saving it somewhere else?")
                       .arg(fileInfo.absolutePath()));
    }

    return (!item->getModified());
}

bool MainWindow::savePattern(Pattern *item) {
    if(!item->hasValidFilename()) {
        return savePatternAs(item);
    } else {
        if (!item->save()) {
            QMessageBox::warning(this, tr("Error"), tr("Error saving pattern %1. Try saving it somewhere else?")
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

    // TODO: This only saves the first frame!
    QImage image =  patternCollection.getPattern(getCurrentPatternIndex())->getFrame(0);

    // Note: Converting frameRate to frame delay here.
    PatternOutput patternOutput(image, drawTimer.interval(),
                        PatternOutput::RGB24,
                        colorMode);


    // Attempt to open the specified file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("Error"), tr("Error, cannot write file %1.")
                       .arg(fileName));
        return;
    }

    QTextStream ts(&file);
    ts << patternOutput.header;
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

    QImage frame = patternCollection.getPattern(getCurrentPatternIndex())->getFrame(getCurrentFrameIndex());
    frame = frame.mirrored(true, false);
    patternCollection.getPattern(getCurrentPatternIndex())->replaceFrame(getCurrentFrameIndex(),frame);
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    QImage frame = patternCollection.getPattern(getCurrentPatternIndex())->getFrame(getCurrentFrameIndex());
    frame = frame.mirrored(false, true);
    patternCollection.getPattern(getCurrentPatternIndex())->replaceFrame(getCurrentFrameIndex(),frame);
}

void MainWindow::on_actionClear_Pattern_triggered()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    QImage frame = patternCollection.getPattern(getCurrentPatternIndex())->getFrame(getCurrentFrameIndex());
    frame.fill(COLOR_CANVAS_DEFAULT);
    patternCollection.getPattern(getCurrentPatternIndex())->replaceFrame(getCurrentFrameIndex(),frame);
}

void MainWindow::showError(QString errorMessage) {
    QMessageBox box(this);
    box.setWindowModality(Qt::WindowModal);
    box.setText(errorMessage);
    box.show();
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

    std::vector<PatternOutput> patterns;

    for(int i = 0; i < patternCollection.count(); i++) {
        // TODO: This needs to be run over all the frames!
        // Rewrite me.
        PatternOutput patternOutput(patternCollection.getPattern(i)->getFrame(0),
                        drawTimer.interval(),
                        //Pattern::RGB24,
                        PatternOutput::RGB565_RLE,
                        colorMode);

        patterns.push_back(patternOutput);
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

    ResizePattern resizeDialog(this);
    resizeDialog.setWindowModality(Qt::WindowModal);
    resizeDialog.setOutputSize(patternCollection.getPattern(getCurrentPatternIndex())->getFrame(0).size());

    resizeDialog.exec();

    if(resizeDialog.result() != QDialog::Accepted) {
        return;
    }

    QSize newDisplaySize = resizeDialog.getOutputSize();

    QSettings settings;
    settings.setValue("Options/displayHeight", static_cast<uint>(newDisplaySize.height()));
    settings.setValue("Options/displayWidth", static_cast<uint>(newDisplaySize.width()));

    qDebug() << "Resizing patterns, height:"
             << newDisplaySize.height()
             << "width:"
             << newDisplaySize.width();

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
    patternEditor->setInstrument(qvariant_cast<AbstractInstrument*>(act->data()));
}

void MainWindow::on_colorPicked(QColor color) {
    colorChooser.setColor(color);
    patternEditor->setToolColor(color);
}

bool MainWindow::promptForSave(Pattern * item) {
    if (item->getModified() == false) {
        return true;
    }

    QString messageText = QString("The pattern %1 has been modified.")
            .arg(item->getPatternName());

    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setIconPixmap(QPixmap::fromImage(item->getFrame(0)));
    msgBox.setText(messageText);
    msgBox.setInformativeText("Do you want to save your changes?");
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
    QString messageText = QString("The following patterns have been modified:\n");

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

void MainWindow::setColorMode(PatternOutput::ColorMode newColorOrder)
{
    switch(newColorOrder) {
    case PatternOutput::RGB:
        actionRGB->setChecked(true);
        actionGRB->setChecked(false);
        break;
    case PatternOutput::GRB:
        actionRGB->setChecked(false);
        actionGRB->setChecked(true);
        break;
    default:
        return;
        break;
    }

    colorMode = newColorOrder;

    QSettings settings;
    settings.setValue("Options/ColorOrder", static_cast<uint>(colorMode));
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

    patternEditor->setFrameData(getCurrentFrameIndex(),
                                patternCollection.getPattern(getCurrentPatternIndex())->getFrame(newFrame));

    pFrame.setText(QString::number(newFrame+1));

    updateBlinky();
}

void MainWindow::updateBlinky()
{
    if(controller.isNull()) {
        return;
    }

    // Ignore the timeout if it came too quickly, so that we don't overload the blinky
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_TIMER_INTERVAL) {
        qDebug() << "Skipping update due to rate limiting. Last update " << newTime - lastTime << "ms ago";
        return;
    }

    lastTime = newTime;

    if(!patternCollection.hasPattern()) {
        return;
    }

    QImage frame = patternCollection.getPattern(getCurrentPatternIndex())->getFrame(getCurrentFrameIndex());
    QVector<QColor> pixels;

    for(int x = 0; x <frame.width(); x++) {
        for(int y = 0; y < frame.height(); y++) {
            pixels.append(frame.pixel(x,x%2?frame.height()-1-y:y));
        }
    }

    QByteArray ledData;
    for(int i = 0; i < pixels.size(); i++) {
        QColor color = ColorModel::correctBrightness(pixels[i]);

        switch(colorMode) {
        case PatternOutput::GRB:
            ledData.append(color.green());
            ledData.append(color.red());
            ledData.append(color.blue());
            break;
        case PatternOutput::RGB:
        default:
            ledData.append(color.red());
            ledData.append(color.green());
            ledData.append(color.blue());
            break;
        }
    }

    controller->sendUpdate(ledData);
}

void MainWindow::on_actionGRB_triggered()
{
    setColorMode(PatternOutput::GRB);
}

void MainWindow::on_actionRGB_triggered()
{
    setColorMode(PatternOutput::RGB);
}

void MainWindow::on_actionNew_triggered()
{
    QSettings settings;
    int patternLength = settings.value("Options/frameCount", DEFAULT_FRAME_COUNT).toUInt();

    QSize displaySize;
    if(!patternCollection.hasPattern()) {
        displaySize.setWidth(settings.value("Options/displayWidth", DEFAULT_DISPLAY_WIDTH).toUInt());
        displaySize.setHeight(settings.value("Options/displayHeight", DEFAULT_DISPLAY_HEIGHT).toUInt());

    }
    else {
        displaySize =  patternCollection.getPattern(getCurrentPatternIndex())->getFrame(0).size();
    }

    Pattern *pattern = new Pattern(displaySize,patternLength);

    patternCollection.addPattern(pattern);
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

void MainWindow::on_patternSelected(const QModelIndex &current, const QModelIndex &) {

    emit(patternStatusChanged(current.isValid()));

    on_patternNameUpdated();
    on_patternModifiedChanged();
    on_patternSizeUpdated();

    // TODO: we're going to have to unload our references, but for now skip that.
    if(!current.isValid()) {
        undoGroup.setActiveStack(NULL);
        timeline->setModel(NULL);
        return;
    }

    Pattern* newpattern = patternCollection.getPattern(current.row());

    undoGroup.setActiveStack(newpattern->getUndoStack());

    timeline->setModel(newpattern->getFrameModel());

    // TODO: Should we unregister these eventually?
    connect(timeline->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(on_patternFrameSelected(const QModelIndex &, const QModelIndex &)));
    connect(timeline->model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)),
            this, SLOT(handleUpdatedData(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    on_patternDataUpdated();
}


void MainWindow::on_patternFrameSelected(const QModelIndex &current, const QModelIndex &) {
    setNewFrame(current.row());
}

void MainWindow::handleUpdatedData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &) {
    int currentIndex = getCurrentFrameIndex();

    // If the first index changed, refresh so that the patternCollection icon will be drawn correctly
    // If the current selection changed, refresh so that the patternEditor contents will be redrawn
    if(topLeft.row() == 0
       || (currentIndex >= topLeft.row() && currentIndex <= bottomRight.row())) {
        on_patternDataUpdated();
    }
}


void MainWindow::on_patternDataUpdated()
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    patternEditor->setFrameData(getCurrentFrameIndex(),
                                patternCollection.getPattern(getCurrentPatternIndex())->getFrame(getCurrentFrameIndex()));

    // Update the LED output
    updateBlinky();
}

void MainWindow::on_patternSizeUpdated()
{
    if(!patternCollection.hasPattern()) {
        patternEditor->setFrameData(0, QImage());
        return;
    }

    patternEditor->setFrameData(getCurrentFrameIndex(),
                                patternCollection.getPattern(getCurrentPatternIndex())->getFrame(getCurrentFrameIndex()));

    // And kick the scroll area so that it will size itself
    scrollArea->resize(scrollArea->width()+1, scrollArea->height());
}

void MainWindow::on_frameDataUpdated(int index, QImage update)
{
    if(!patternCollection.hasPattern()) {
        return;
    }

    patternCollection.getPattern(getCurrentPatternIndex())->replaceFrame(index, update);
}


void MainWindow::on_patternNameUpdated()
{
    QString name;

    if(!patternCollection.hasPattern()) {
        name = "()";
    }
    else {
        name = patternCollection.getPattern(getCurrentPatternIndex())->getPatternName();
    }

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

void MainWindow::on_patternModifiedChanged()
{
    if(!patternCollection.hasPattern()) {
        actionSave_File->setEnabled(false);
        return;
    }

    if(patternCollection.getPattern(getCurrentPatternIndex())->getModified() == true) {
        actionSave_File->setEnabled(true);
    }
    else {
        actionSave_File->setEnabled(false);
    }
}

void MainWindow::on_ExampleSelected(QAction* action) {
    qDebug() << "Example selected:" << action->objectName();

    QSettings settings;

    // TODO: This is duplicated in on_actionLoad_file_triggered(), on_actionNew_triggered()
    QSize displaySize;
    displaySize.setWidth(settings.value("Options/displayWidth", DEFAULT_DISPLAY_WIDTH).toUInt());
    displaySize.setHeight(settings.value("Options/displayHeight", DEFAULT_DISPLAY_HEIGHT).toUInt());

    // Create a pattern, and attempt to load the file
    Pattern *pattern = new Pattern(displaySize,1);

    if(!pattern->load(action->objectName())) {
        showError("Could not open file "
                   + action->objectName()
                   + ". Perhaps it has a formatting problem?");
        return;
    }

    patternCollection.addPattern(pattern);
}
