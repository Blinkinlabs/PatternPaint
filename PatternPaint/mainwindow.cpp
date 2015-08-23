#include "pattern.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "colormodel.h"
#include "systeminformation.h"
#include "aboutpatternpaint.h"
#include "resizepattern.h"
#include "undocommand.h"
#include "colorchooser.h"
#include "blinkytape.h"

#include "pencilinstrument.h"
#include "lineinstrument.h"
#include "colorpickerinstrument.h"
#include "sprayinstrument.h"
#include "fillinstrument.h"
#include "patternitem.h"
#include "patternitemdelegate.h"

#include "matrixdisplay.h"
#include "timelinedisplay.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QtWidgets>
#include <QUndoGroup>
#include <QToolButton>
#include <blinkytapeuploader.h>


#define PATTERN_SPEED_MINIMUM_VALUE 1
#define PATTERN_SPEED_MAXIMUM_VALUE 100
#define PATTERN_SPEED_DEFAULT_VALUE 20

#define DRAWING_SIZE_MINIMUM_VALUE 1
#define DRAWING_SIZE_MAXIMUM_VALUE 20
#define DRAWING_SIZE_DEFAULT_VALUE 1

#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,255)
#define COLOR_TOOL_DEFAULT    QColor(255,255,255,255)

#define DEFAULT_LED_COUNT 60
#define DEFAULT_PATTERN_LENGTH 100

#define MIN_TIMER_INTERVAL 5  // minimum interval to wait between blinkytape updates

#define CONNECTION_SCANNER_INTERVAL 1000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ledDisplay(NULL),
    frame(0)
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

    drawTimer = new QTimer(this);
    connectionScannerTimer = new QTimer(this);

    // prepare undo/redo
    menuEdit->addSeparator();
    undoGroup = new QUndoGroup(this);
    undoAction = undoGroup->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Z")));
    undoAction->setEnabled(false);
    menuEdit->addAction(undoAction);

    redoAction = undoGroup->createRedoAction(this, tr("&Redo"));
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

    colorChooser = new ColorChooser(COLOR_TOOL_DEFAULT, this);
    colorChooser->setToolTip(tr("Drawing color"));
    instrumentToolbar->addWidget(colorChooser);
    patternEditor->setToolColor(COLOR_TOOL_DEFAULT);

    QSpinBox *penSizeSpin = new QSpinBox(this);
    penSizeSpin->setRange(DRAWING_SIZE_MINIMUM_VALUE, DRAWING_SIZE_MAXIMUM_VALUE);
    penSizeSpin->setValue(DRAWING_SIZE_DEFAULT_VALUE);
    penSizeSpin->setToolTip(tr("Pen size"));
    instrumentToolbar->addWidget(penSizeSpin);


    // tools
    pSpeed = new QSpinBox(this);
    pSpeed->setRange(PATTERN_SPEED_MINIMUM_VALUE, PATTERN_SPEED_MAXIMUM_VALUE);
    pSpeed->setValue(PATTERN_SPEED_DEFAULT_VALUE);
    pSpeed->setToolTip(tr("Pattern speed"));
    playbackToolbar->addWidget(pSpeed);
    connect(pSpeed, SIGNAL(valueChanged(int)), this, SLOT(patternSpeed_valueChanged(int)));
    patternSpeed_valueChanged(PATTERN_SPEED_DEFAULT_VALUE);

    pFrame = new QLineEdit(this);
    pFrame->setMaximumWidth(30);
    pFrame->setMinimumWidth(30);
    pFrame->setValidator(new QIntValidator(1,std::numeric_limits<int>::max(),this));
    pFrame->setToolTip(tr("Current frame"));
    playbackToolbar->insertWidget(actionStepForward, pFrame);
    connect(pFrame, SIGNAL(textEdited(QString)), this, SLOT(frameIndex_valueChanged(QString)));
    frameIndex_valueChanged("1");

    mode = Disconnected;

    // Our pattern editor wants to get some notifications
    connect(colorChooser, SIGNAL(sendColor(QColor)),
            patternEditor, SLOT(setToolColor(QColor)));
    connect(penSizeSpin, SIGNAL(valueChanged(int)),
            patternEditor, SLOT(setToolSize(int)));
    connect(patternCollection, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(setPatternItem(QListWidgetItem*, QListWidgetItem*)));


    // Pre-set the upload progress dialog
    progressDialog = new QProgressDialog(this);
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(150);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setAutoClose(false);

    // The draw timer tells the pattern to advance
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawTimer_timeout()));

    // Start a scanner to connect to a Blinky automatically
    connect(connectionScannerTimer, SIGNAL(timeout()), this, SLOT(connectionScannerTimer_timeout()));
    connectionScannerTimer->setInterval(CONNECTION_SCANNER_INTERVAL);
    connectionScannerTimer->start();


    penSizeSpin->setValue(1);
    patternEditor->setToolSize(1);

    actionPen->setChecked(true);
    patternEditor->setInstrument(qvariant_cast<AbstractInstrument*>(actionPen->data()));
    readSettings();

    QSettings settings;
    setColorMode(static_cast<Pattern::ColorMode>(settings.value("Options/ColorOrder", Pattern::RGB).toUInt()));

    setDisplayMode(static_cast<LedDisplay::Mode>(settings.value("Options/DisplayMode", LedDisplay::TIMELINE).toUInt()));

    patternCollection->setItemDelegate(new PatternItemDelegate(this));

    connect(&patternUpdateNotifier, SIGNAL(patternSizeUpdated()),
            this, SLOT(on_patternSizeUpdated()));
    connect(&patternUpdateNotifier, SIGNAL(patternDataUpdated()),
            this, SLOT(on_patternDataUpdated()));
    connect(&patternUpdateNotifier, SIGNAL(patternNameUpdated()),
            this, SLOT(on_patternNameUpdated()));
    connect(&patternUpdateNotifier, SIGNAL(patternModifiedChanged()),
            this, SLOT(on_patternModifiedChanged()));  // TODO

    patternCollection->setNotifier(&patternUpdateNotifier);
    patternCollection->setUndoGroup(undoGroup);

    // Create a pattern.
    on_actionNew_triggered();
}

MainWindow::~MainWindow(){
#if defined(Q_OS_MAC)
    // start the app nap inhibitor
    if(appNap != NULL) {
        delete appNap;
        appNap = NULL;
    }
#endif

    if(ledDisplay != NULL) {
        delete ledDisplay;
    }
}

void MainWindow::drawTimer_timeout() {
    if(!ledDisplay->hasPatternItem()) {
        return;
    }

    setNewFrame((frame+1)%ledDisplay->getFrameCount());
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
    drawTimer->setInterval(1000/value);
}

void MainWindow::frameIndex_valueChanged(QString value)
{
    setNewFrame(value.toInt() - 1);
}

void MainWindow::on_actionPlay_triggered()
{
    if (drawTimer->isActive()) {
        stopPlayback();
    } else {
        startPlayback();
    }
}

void MainWindow::startPlayback() {
    drawTimer->start();
    actionPlay->setText(tr("Pause"));
    actionPlay->setIcon(QIcon(":/icons/images/icons/Pause-100.png"));
}

void MainWindow::stopPlayback() {
    drawTimer->stop();
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

    int ledCount = 0;
    if((ledDisplay != NULL) && (ledDisplay->hasFixedLedCount())) {
        ledCount = ledDisplay->getFixedLedCount();
    }
    else {
        ledCount = settings.value("Options/ledCount", DEFAULT_LED_COUNT).toUInt();
    }

    // Create a patternItem, and attempt to load the file
    PatternItem* patternItem = new PatternItem(1, ledCount);

    if(!patternItem->load(fileInfo)) {
        showError("Could not open file "
                   + fileName
                   + ". Perhaps it has a formatting problem?");
        return;
    }

    patternCollection->addItem(patternItem);
    patternCollection->setCurrentItem(patternItem);
}


bool MainWindow::savePatternAs(PatternItem* item) {
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

bool MainWindow::savePattern(PatternItem* item) {
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
    if(patternCollection->currentItem() == NULL) {
        return;
    }

    PatternItem* item = dynamic_cast<PatternItem*>(patternCollection->currentItem());
    savePatternAs(item);
}

void MainWindow::on_actionSave_File_triggered() {
    if(patternCollection->currentItem() == NULL) {
        return;
    }

    PatternItem* item = dynamic_cast<PatternItem*>(patternCollection->currentItem());
    savePattern(item);
}

void MainWindow::on_actionExit_triggered() {
    this->close();
}

void MainWindow::on_actionExport_pattern_for_Arduino_triggered() {
    if(patternCollection->currentItem() == NULL) {
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

    PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->currentItem());
    QImage image =  patternItem->getImage();

    // Note: Converting frameRate to frame delay here.
    Pattern pattern(image, drawTimer->interval(),
                        Pattern::RGB24,
                        colorMode);


    // Attempt to open the specified file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("Error"), tr("Error, cannot write file %1.")
                       .arg(fileName));
        return;
    }

    QTextStream ts(&file);
    ts << pattern.header;
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

        connectionScannerTimer->start();

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
    if(progressDialog->isHidden()) {
        qDebug() << "Got a progress event while the progress dialog is hidden, event order problem?";
        return;
    }

    progressDialog->setMaximum(progressValue);
}

void MainWindow::on_uploaderProgressChanged(int progressValue)
{
    if(progressDialog->isHidden()) {
        qDebug() << "Got a progress event while the progress dialog is hidden, event order problem?";
        return;
    }

    // Clip the progress to maximum, until we work out a better way to estimate it.
    if(progressValue >= progressDialog->maximum()) {
        progressValue = progressDialog->maximum() - 1;
    }

    progressDialog->setValue(progressValue);
}

void MainWindow::on_uploaderFinished(bool result)
{
    mode = Disconnected;
    uploader.clear();

    progressDialog->hide();

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
    if (!ledDisplay->hasPatternItem()) {
        return;
    }

    QImage frame = ledDisplay->getFrame();
    frame = frame.mirrored(true, false);
    ledDisplay->applyInstrument(frame);
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    if (!ledDisplay->hasPatternItem()) {
        return;
    }

    QImage frame = ledDisplay->getFrame();
    frame = frame.mirrored(false, true);
    ledDisplay->applyInstrument(frame);
}

void MainWindow::on_actionClear_Pattern_triggered()
{
    if (!ledDisplay->hasPatternItem()) {
        return;
    }

    QImage frame = ledDisplay->getFrame();
    frame.fill(COLOR_CANVAS_DEFAULT);
    ledDisplay->applyInstrument(frame);
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

        progressDialog->setWindowTitle("Firmware reset");
        progressDialog->setLabelText(
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

        progressDialog->setWindowTitle("Firmware reset");
        progressDialog->setLabelText("Loading new firmware onto Blinky");
    }

    mode = Uploading;

    progressDialog->setValue(progressDialog->minimum());
    progressDialog->show();
}

void MainWindow::on_actionSave_to_Blinky_triggered()
{
    if(controller.isNull()) {
        return;
    }

    if(patternCollection->currentItem() == NULL) {
        return;
    }


    // Create a temporary displaymodel to interpret the patterns
    LedDisplay* display;

    QSettings settings;
    LedDisplay::Mode displayMode = static_cast<LedDisplay::Mode>(
                settings.value("Options/DisplayMode", LedDisplay::TIMELINE).toUInt()
                );

    // Create a new displaymodel to handle the conversions
    switch(displayMode) {
    case LedDisplay::TIMELINE:
        display = new TimelineDisplay();
        break;
    case LedDisplay::MATRIX8x8:
        display = new MatrixDisplay(8,8);
        break;
    default:
        return;
        break;
    }

    std::vector<Pattern> patterns;

    for(int i = 0; i < patternCollection->count(); i++) {
        // Convert the current pattern into a Pattern
        PatternItem* p = dynamic_cast<PatternItem*>(patternCollection->item(i));
        display->setSource(p);

        // TODO: Attempt different compressions till one works.
        Pattern pattern(display->getStreamImage(),
                        drawTimer->interval(),
                        //Pattern::RGB24,
                        Pattern::RGB565_RLE,
                        colorMode);

        patterns.push_back(pattern);
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

    progressDialog->setWindowTitle("Blinky exporter");
    progressDialog->setLabelText("Saving pattern to Blinky...");

    progressDialog->setValue(progressDialog->minimum());
    progressDialog->show();
}


void MainWindow::on_actionResize_Pattern_triggered()
{
    QSettings settings;
    PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->currentItem());

    if((ledDisplay == NULL) || (patternCollection->currentItem() == NULL)) {
        return;
    }

    int patternLength = patternItem->getImage().width();
    int ledCount = patternItem->getImage().height();

    bool fixedLedCount = ledDisplay->hasFixedLedCount();

    if(fixedLedCount) {
        ledCount = ledDisplay->getFixedLedCount();
    }

    ResizePattern resizeDialog(this);
    resizeDialog.setWindowModality(Qt::WindowModal);
    resizeDialog.setLength(patternLength);
    resizeDialog.setLedCount(ledCount);
    resizeDialog.setFixedLedCount(fixedLedCount);

    resizeDialog.exec();

    if(resizeDialog.result() != QDialog::Accepted) {
        return;
    }

    // Do a quick sanity check on the inputs, they should be validated by the resizer class.
    if(resizeDialog.length() < 1 || resizeDialog.ledCount() < 1) {
        qDebug() << "Resize pattern: data out of range, discarding";
        return;
    }
    patternLength = resizeDialog.length();
    ledCount = resizeDialog.ledCount();

    settings.setValue("Options/patternLength", static_cast<uint>(patternLength));
    settings.setValue("Options/ledCount", static_cast<uint>(ledCount));

    qDebug() << "Resizing patterns, length:"
             << patternLength
             << "height:"
             << ledCount;

    // Resize the selected pattern
    patternItem->resize(patternLength, ledCount, true);
}

void MainWindow::on_actionAddress_programmer_triggered()
{
    AddressProgrammer programmer(this);
    programmer.setWindowModality(Qt::WindowModal);
    programmer.exec();
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(880, 450)).toSize());
    move(settings.value("pos", QPoint(100, 100)).toPoint());
    settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    // TODO: Combine all of these into one big 'save' message
    std::vector<PatternItem*> unsaved;

    for(int i = 0; i < patternCollection->count(); i++) {
        // Convert the current pattern into a Pattern
        PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->item(i));

        if (patternItem->getModified() == true) {
            unsaved.push_back(patternItem);
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

    writeSettings();
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
    QAction* act = dynamic_cast<QAction*>(sender());
    Q_ASSERT(act != NULL);
    foreach(QAction* a, instrumentToolbar->actions()) {
        a->setChecked(false);
    }

    act->setChecked(true);
    patternEditor->setInstrument(qvariant_cast<AbstractInstrument*>(act->data()));
}

void MainWindow::on_colorPicked(QColor color) {
    colorChooser->setColor(color);
    patternEditor->setToolColor(color);
}

bool MainWindow::promptForSave(PatternItem* item) {
    if (item->getModified() == false) {
        return true;
    }

    QString messageText = QString("The pattern %1 has been modified.")
            .arg(item->getPatternName());

    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setIconPixmap(QPixmap::fromImage(item->getImage()));
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

bool MainWindow::promptForSave(std::vector<PatternItem*> patternItems) {
    QString messageText = QString("The following patterns have been modified:\n");

    for(std::size_t i = 0; i < patternItems.size(); i++) {
        messageText += patternItems.at(i)->getPatternName();
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
        for(std::size_t i = 0; i < patternItems.size(); i++) {
            if(!savePattern(patternItems.at(i))) {
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

void MainWindow::setColorMode(Pattern::ColorMode newColorOrder)
{
    switch(newColorOrder) {
    case Pattern::RGB:
        actionRGB->setChecked(true);
        actionGRB->setChecked(false);
        break;
    case Pattern::GRB:
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

void MainWindow::setDisplayMode(LedDisplay::Mode newDisplayMode)
{
    LedDisplay* newDisplayModel;



    // Create a new displaymodel if possible
    switch(newDisplayMode) {
    case LedDisplay::TIMELINE:
        newDisplayModel = new TimelineDisplay();
        break;
    case LedDisplay::MATRIX8x8:
        newDisplayModel = new MatrixDisplay(8,8);
        break;
    default:
        return;
        break;
    }

    // Check if any of the patterns need to be resized
    // TODO: This doesn't make any sense.
    if(newDisplayModel->hasFixedLedCount()) {
        bool needToResize = false;

        for(int i = 0; i < patternCollection->count(); i++) {
            // Convert the current pattern into a Pattern
            PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->item(i));
            if(patternItem->getImage().height() != newDisplayModel->getFixedLedCount()) {
                needToResize = true;
                break;
            }
        }

        // ask about it
        if(needToResize) {
            QMessageBox msgBox(this);
            msgBox.setWindowModality(Qt::WindowModal);
            //msgBox.setText(messageText);
            msgBox.setText("Some patterns need to be resized to use this mode. Ok to resize?");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            int ans = msgBox.exec();

            if (ans == QMessageBox::Cancel) {
                delete newDisplayModel;
                return;
            }
        }

        // then resize them
        for(int i = 0; i < patternCollection->count(); i++) {
            // Convert the current pattern into a Pattern
            // TODO: Run this resizing through the display model
            PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->item(i));
            if(patternItem->getImage().height() != newDisplayModel->getFixedLedCount()) {
                patternItem->resize(patternItem->getImage().width(),newDisplayModel->getFixedLedCount(),true);
            }
        }
    }

    // Finally, apply the new mode.

    switch(newDisplayMode) {
    case LedDisplay::TIMELINE:
        actionTimeline->setChecked(true);
        actionMatrix->setChecked(false);
        break;
    case LedDisplay::MATRIX8x8:
        actionTimeline->setChecked(false);
        actionMatrix->setChecked(true);
        break;
    }

    QSettings settings;
    settings.setValue("Options/DisplayMode", static_cast<uint>(newDisplayMode));


    // TODO: Smart pointer for this.
    if(ledDisplay != NULL) {
        delete ledDisplay;
    }
    ledDisplay = newDisplayModel;

    ledDisplay->setSource(dynamic_cast<PatternItem*>(patternCollection->currentItem()));
    patternEditor->setDisplayModel(ledDisplay);

    on_patternSizeUpdated();
    on_patternDataUpdated();
}

void MainWindow::setNewFrame(int newFrame)
{
    int frameCount = 0;

    if(ledDisplay != NULL && ledDisplay->hasPatternItem()) {
        frameCount = ledDisplay->getFrameCount();
    }

    if(newFrame >= frameCount) {
        newFrame = frameCount - 1;
    }
    if(newFrame < 0) {
        newFrame = 0;
    }

    frame = newFrame;

    pFrame->setText(QString("%1").arg(frame+1));
    patternEditor->setPlaybackRow(frame);
    updateBlinky();
}

void MainWindow::updateBlinky()
{
    // Ignore the timeout if it came too quickly, so that we don't overload the blinky
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_TIMER_INTERVAL) {
        qDebug() << "Skipping update due to rate limiting. Last update " << newTime - lastTime << "ms ago";
        return;
    }

    lastTime = newTime;

    if((ledDisplay == NULL) || (patternCollection->currentItem() == NULL)) {
        return;
    }

    if(controller.isNull()) {
        return;
    }

    QImage image = ledDisplay->getStreamImage();

    // TODO: Put this in a converter class.
    QByteArray ledData;
    for(int i = 0; i < image.height(); i++) {
        QRgb color = ColorModel::correctBrightness(image.pixel(frame, i));

        switch(colorMode) {
        case Pattern::GRB:
            ledData.append(qGreen(color));
            ledData.append(qRed(color));
            ledData.append(qBlue(color));
            break;
        case Pattern::RGB:
        default:
            ledData.append(qRed(color));
            ledData.append(qGreen(color));
            ledData.append(qBlue(color));
            break;
        }
    }
    controller->sendUpdate(ledData);
}

void MainWindow::on_actionGRB_triggered()
{
    setColorMode(Pattern::GRB);
}

void MainWindow::on_actionRGB_triggered()
{
    setColorMode(Pattern::RGB);
}

void MainWindow::on_actionNew_triggered()
{
    QSettings settings;
    int patternLength = settings.value("Options/patternLength", DEFAULT_PATTERN_LENGTH).toUInt();

    int ledCount = 0;
    if((ledDisplay != NULL) && (ledDisplay->hasFixedLedCount())) {
        ledCount = ledDisplay->getFixedLedCount();
    }
    else {
        ledCount = settings.value("Options/ledCount", DEFAULT_LED_COUNT).toUInt();
    }

    PatternItem* patternItem = new PatternItem(patternLength, ledCount);

    patternCollection->addItem(patternItem);
    patternCollection->setCurrentItem(patternItem);
}

void MainWindow::on_actionClose_triggered()
{
    if(patternCollection->currentItem() == NULL) {
        return;
    }

    PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->currentItem());
    if(!promptForSave(patternItem)) {
        return;
    }

    // TODO: remove the undo stack from the undo group?
    patternCollection->takeItem(patternCollection->currentRow());
}

void MainWindow::setPatternItem(QListWidgetItem* current, QListWidgetItem* previous) {
    Q_UNUSED(previous);

    on_patternNameUpdated();
    on_patternModifiedChanged();
    on_patternSizeUpdated();

    // TODO: we're going to have to unload our references, but for now skip that.
    if(current == NULL) {
        actionClose->setEnabled(false);  // TODO: move me?
        actionFlip_Horizontal->setEnabled(false);
        actionFlip_Vertical->setEnabled(false);
        actionClear_Pattern->setEnabled(false);
        actionAddFrame->setEnabled(false);
        actionDeleteFrame->setEnabled(false);
        actionPlay->setEnabled(false);
        actionStepForward->setEnabled(false);
        actionStepBackward->setEnabled(false);

        patternEditor->setPatternItem(NULL);
        undoGroup->setActiveStack(NULL);
        return;
    }
    actionClose->setEnabled(true);      // TODO: Move me?
    actionFlip_Horizontal->setEnabled(true);
    actionFlip_Vertical->setEnabled(true);
    actionClear_Pattern->setEnabled(true);
    actionAddFrame->setEnabled(true);
    actionDeleteFrame->setEnabled(true);
    actionPlay->setEnabled(true);
    actionStepForward->setEnabled(true);
    actionStepBackward->setEnabled(true);

    PatternItem* newPatternItem = dynamic_cast<PatternItem*>(current);
    undoGroup->setActiveStack(newPatternItem->getUndoStack());

    on_patternDataUpdated();
}

void MainWindow::on_patternDataUpdated()
{
    if(patternCollection->currentItem() == NULL) {
        return;
    }

    patternEditor->update();

    // Redraw the data-dependent views
    patternCollection->doItemsLayout();

    // Update the LED output
    updateBlinky();
}

void MainWindow::on_patternSizeUpdated()
{
    if(patternCollection->currentItem() == NULL) {
        return;
    }

    // Reset the current frame, in case the new size is smaller than the old one
    setNewFrame(frame);

    PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->currentItem());

    // Re-load the patterneditor so that it can redraw its view
    patternEditor->setPatternItem(patternItem);

    // And kick the scroll area so that it will size itself
    scrollArea->resize(scrollArea->width()+1, scrollArea->height());
}


void MainWindow::on_patternNameUpdated()
{
    QString name;

    if(patternCollection->currentItem() == NULL) {
        name = "()";
    }
    else {
        PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->currentItem());
        name = patternItem->getPatternName();
    }

    this->setWindowTitle(name + " - Pattern Paint");
}

void MainWindow::on_actionStepForward_triggered()
{
    if(patternCollection->currentItem() == NULL) {
        return;
    }


    setNewFrame((frame+1)%ledDisplay->getFrameCount());
}

void MainWindow::on_actionStepBackward_triggered()
{
    if(patternCollection->currentItem() == NULL) {
        return;
    }

    setNewFrame(frame<=0?ledDisplay->getFrameCount()-1:frame-1);
}

void MainWindow::on_actionTimeline_triggered()
{
    setDisplayMode(LedDisplay::TIMELINE);
}

void MainWindow::on_actionMatrix_triggered()
{
    setDisplayMode(LedDisplay::MATRIX8x8);
}

void MainWindow::on_actionAddFrame_triggered()
{
    if(!ledDisplay->hasPatternItem()) {
        return;
    }

    ledDisplay->addFrame(ledDisplay->getFrameIndex());

    setNewFrame(ledDisplay->getFrameIndex()+1);
}

void MainWindow::on_actionDeleteFrame_triggered()
{
    if(!ledDisplay->hasPatternItem()) {
        return;
    }

    ledDisplay->deleteFrame(ledDisplay->getFrameIndex());

    setNewFrame(ledDisplay->getFrameIndex()-1);
}

void MainWindow::on_patternModifiedChanged()
{
    if(patternCollection->currentItem() == NULL) {
        actionSave_File->setEnabled(false);
        return;
    }

    PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->currentItem());
    if(patternItem->getModified() == true) {
        actionSave_File->setEnabled(true);
    }
    else {
        actionSave_File->setEnabled(false);
    }
}
