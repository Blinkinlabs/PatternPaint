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
#include "patternitemdelegate.h"
#include "patternitem.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QtWidgets>
#include <QUndoGroup>
#include <QToolButton>

#define DEFAULT_LED_COUNT 60
#define DEFAULT_PATTERN_LENGTH 100

#define MIN_TIMER_INTERVAL 10  // minimum interval to wait before firing a drawtimer update

#define CONNECTION_SCANNER_INTERVAL 1000

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    // prepare undo/redo
    menuEdit->addSeparator();
    undoStackGroup = new QUndoGroup(this);
    undoAction = undoStackGroup->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Z")));
    undoAction->setEnabled(false);
    menuEdit->addAction(undoAction);

    redoAction = undoStackGroup->createRedoAction(this, tr("&Redo"));
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
    actionPipette->setData(QVariant::fromValue(cpi));
    actionSpray->setData(QVariant::fromValue(new SprayInstrument(this)));
    actionFill->setData(QVariant::fromValue(new FillInstrument(this)));

    colorChooser = new ColorChooser(255, 255, 255, this);
    colorChooser->setGeometry(0,0,32,32);

    colorChooser->setStatusTip(tr("Drawing color"));
    colorChooser->setToolTip(tr("Drawing color"));
    instruments->addSeparator();
    instruments->addWidget(colorChooser);

    QSpinBox *penSizeSpin = new QSpinBox();
    penSizeSpin->setRange(1, 20);
    penSizeSpin->setValue(1);
    penSizeSpin->setStatusTip(tr("Pen size"));
    penSizeSpin->setToolTip(tr("Pen size"));
    instruments->addWidget(penSizeSpin);

    // tools
    pSpeed = new QSpinBox(this);
    pSpeed->setRange(1, 100);
    pSpeed->setValue(20);
    pSpeed->setToolTip(tr("Pattern speed"));
    tools->addWidget(pSpeed);
    connect(pSpeed, SIGNAL(valueChanged(int)), this, SLOT(patternSpeed_valueChanged(int)));

    drawTimer = new QTimer(this);
    connectionScannerTimer = new QTimer(this);

    mode = Disconnected;

    // Our pattern editor wants to get some notifications
    connect(colorChooser, SIGNAL(sendColor(QColor)),
            patternEditor, SLOT(setToolColor(QColor)));
    connect(penSizeSpin, SIGNAL(valueChanged(int)),
            patternEditor, SLOT(setToolSize(int)));
    connect(patternCollection, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(setPatternItem(QListWidgetItem*, QListWidgetItem*)));

    connect(patternEditor, SIGNAL(forcePatternEditorRedraw()), SLOT(on_forcePatternEditorRedraw()));


    // Pre-set the upload progress dialog
    progressDialog = new QProgressDialog(this);
    progressDialog->setWindowTitle("Blinky exporter");
    progressDialog->setLabelText("Saving pattern to Blinky...");
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(150);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setAutoClose(false);

    errorMessageDialog = new QMessageBox(this);
    errorMessageDialog->setWindowModality(Qt::WindowModal);


    // The draw timer tells the pattern to advance
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawTimer_timeout()));
    drawTimer->setInterval(33);

    // Start a scanner to connect to a Blinky automatically
    connect(connectionScannerTimer, SIGNAL(timeout()), this, SLOT(connectionScannerTimer_timeout()));
    connectionScannerTimer->setInterval(CONNECTION_SCANNER_INTERVAL);
    connectionScannerTimer->start();

    // Initial values for interface
    colorChooser->setColor(QColor(255,255,255));      // TODO: Why aren't signals propegated from this?
    patternEditor->setToolColor(QColor(255,255,255));

    penSizeSpin->setValue(1);      // TODO: Why aren't signals propegated from this?
    patternEditor->setToolSize(1);

    actionPen->setChecked(true);
    patternEditor->setInstrument(qvariant_cast<AbstractInstrument*>(actionPen->data()));
    readSettings();

    this->setWindowTitle("Untitled - Pattern Paint");

    QSettings settings;
    setColorMode(static_cast<Pattern::ColorMode>(settings.value("Options/ColorOrder", Pattern::RGB).toUInt()));

    this->patternCollection->setItemDelegate(new PatternItemDelegate());

    // Create a pattern.
    on_actionNew_triggered();
}

MainWindow::~MainWindow(){}

void MainWindow::drawTimer_timeout() {

    // TODO: move this state to somewhere; the patternEditor class maybe?
    static int n = 0;

    // Ignore the timeout if it came to quickly, so that we don't overload the blinky
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_TIMER_INTERVAL) {
        qDebug() << "Dropping timer update due to rate limiting. Last update " << newTime - lastTime << "ms ago";
        return;
    }

    lastTime = newTime;

    if(controller.isNull()) {
        return;
    }

    QImage image = patternEditor->getPatternAsImage();

    // TODO: Put this in a converter class.
    QByteArray ledData;
    for(int i = 0; i < image.height(); i++) {
        QRgb color = ColorModel::correctBrightness(image.pixel(n, i));

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

    n = (n+1)%image.width();
    patternEditor->setPlaybackRow(n);
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
    actionPlay->setIcon(QIcon(":/resources/images/pause.png"));
}

void MainWindow::stopPlayback() {
    drawTimer->stop();
    actionPlay->setText(tr("Play"));
    actionPlay->setIcon(QIcon(":/resources/images/play.png"));
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
        tr("Open Pattern"), lastDirectory, tr("Pattern Files (*.png *.jpg *.bmp)"));

    if(fileName.length() == 0) {
        return;
    }

    QFileInfo fileInfo(fileName);
    settings.setValue("File/LoadDirectory", fileInfo.absolutePath());

    int ledCount = settings.value("Options/ledCount", DEFAULT_LED_COUNT).toUInt();


    // Create a patternItem, and attempt to load the file
    PatternItem* patternItem = new PatternItem(1, ledCount);

    if(!patternItem->load(fileInfo)) {
        errorMessageDialog->setText("Could not open file " + fileName + ". Perhaps it has a formatting problem?");
        errorMessageDialog->show();
        return;
    }

    undoStackGroup->addStack(patternItem->getUndoStack());
    this->patternCollection->addItem(patternItem);
    this->patternCollection->setCurrentItem(patternItem);
}

void MainWindow::on_actionSave_File_as_triggered() {
    QSettings settings;
    QString lastDirectory = settings.value("File/SaveDirectory").toString();

    QDir dir(lastDirectory);
    if(!dir.isReadable()) {
        lastDirectory = QDir::homePath();
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Pattern"), lastDirectory, tr("Pattern Files (*.png *.jpg *.bmp)"));

    if(fileName.length() == 0) {
        return;
    }

    QFileInfo fileInfo(fileName);
    settings.setValue("File/SaveDirectory", fileInfo.absolutePath());

    PatternItem* item = dynamic_cast<PatternItem*>(patternCollection->currentItem());
    if (!item->saveAs(fileInfo)) {
        QMessageBox::warning(this, tr("Error"), tr("Error saving pattern %1. Try saving it somewhere else?")
                       .arg(fileInfo.absolutePath()));
    }
}

void MainWindow::on_actionSave_File_triggered() {
    PatternItem* item = dynamic_cast<PatternItem*>(patternCollection->currentItem());

    if(!item->hasValidFilename()) {
        on_actionSave_File_as_triggered();
    } else {
        if (!item->save()) {
            QMessageBox::warning(this, tr("Error"), tr("Error saving pattern %1. Try saving it somewhere else?")
                           .arg(item->getPatternName()));
        }
    }
}

void MainWindow::on_actionExit_triggered() {
    this->close();
}

void MainWindow::on_actionExport_pattern_for_Arduino_triggered() {
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

    // Convert the current pattern into a Pattern
    QImage image =  patternEditor->getPatternAsImage();

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
    }
    else {
        mode = Disconnected;
        stopPlayback();

        // TODO: Does this delete the serial object reliably?
        controller.clear();

        connectionScannerTimer->start();
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

    qDebug() << "Uploader finished! Result:" << result;
    progressDialog->hide();
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
    dynamic_cast<PatternItem*>(this->patternCollection->currentItem())->flipHorizontal();
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    dynamic_cast<PatternItem*>(this->patternCollection->currentItem())->flipVertical();
}

void MainWindow::on_actionClear_Pattern_triggered()
{
    dynamic_cast<PatternItem*>(this->patternCollection->currentItem())->clear();
}

void MainWindow::on_actionLoad_rainbow_sketch_triggered()
{
    if(controller.isNull()) {
        return;
    }

    if(!controller->getUploader(uploader)) {
        return;
    }

    if(uploader.isNull()) {
        return;
    }

    connectUploader();

    if(!uploader->upgradeFirmware(*controller)) {
        errorMessageDialog->setText(uploader->getErrorString());
        errorMessageDialog->show();
        return;
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

    std::vector<Pattern> patterns;

    for(int i = 0; i < this->patternCollection->count(); i++) {
        // Convert the current pattern into a Pattern
        PatternItem* p = dynamic_cast<PatternItem*>(this->patternCollection->item(i));

        // Note: Converting frameRate to frame delay here.
        // TODO: Attempt different compressions till one works.
        Pattern pattern(p->getImage(),
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
        errorMessageDialog->setText(uploader->getErrorString());
        errorMessageDialog->show();
        return;
    }
    mode = Uploading;

    progressDialog->setValue(progressDialog->minimum());
    progressDialog->show();
}


void MainWindow::on_actionResize_Pattern_triggered()
{
    QSettings settings;

    int patternLength = settings.value("Options/patternLength", DEFAULT_PATTERN_LENGTH).toUInt();
    int ledCount = settings.value("Options/ledCount", DEFAULT_LED_COUNT).toUInt();

    ResizePattern resizeDialog(this);
    resizeDialog.setWindowModality(Qt::WindowModal);
    resizeDialog.setLength(patternLength);
    resizeDialog.setLedCount(ledCount);
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
    PatternItem* patternItem = dynamic_cast<PatternItem*>(this->patternCollection->currentItem());
    patternItem->resize(patternLength, ledCount, true);

    // Re-load the patterneditor to force a screen update
    patternEditor->setPatternItem(patternItem);
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

    for(int i = 0; i < this->patternCollection->count(); i++) {
        // Convert the current pattern into a Pattern
        PatternItem* patternItem = dynamic_cast<PatternItem*>(this->patternCollection->item(i));

        if(!promptForSave(patternItem)) {
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
    foreach(QAction* a, instruments->actions()) {
        a->setChecked(false);
    }

    act->setChecked(true);
    patternEditor->setInstrument(qvariant_cast<AbstractInstrument*>(act->data()));
}

void MainWindow::on_colorPicked(QColor color) {
    colorChooser->setColor(color);
    patternEditor->setToolColor(color);
}

void MainWindow::on_forcePatternEditorRedraw() {
    // Note: This is a hack to get the patterneditor area to redraw.
    scrollArea->resize(scrollArea->width()+1, scrollArea->height());
}

//void MainWindow::on_imageChanged(bool changed)
//{
//    actionSave_File->setEnabled(changed);
//}

void MainWindow::on_patternNameChanged(QString name)
{
    this->setWindowTitle(name + " - Pattern Paint");
}

int MainWindow::promptForSave(PatternItem* patternItem) {
    if (patternItem->getModified() == false) {
        return true;
    }

    QString messageText = QString("The pattern %1 has been modified.")
            .arg(patternItem->getPatternName());

    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setIconPixmap(QPixmap::fromImage(patternItem->getImage()));
    msgBox.setText(messageText);
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ans = msgBox.exec();

    if (ans == QMessageBox::Save) {
        on_actionSave_File_triggered();

        return (!patternItem->getModified());
    }

    if (ans == QMessageBox::Cancel) {
        return false;
    }

    if (ans == QMessageBox::Discard) {
        return true;
    }

    return false;
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
    colorMode = newColorOrder;

    QSettings settings;
    settings.setValue("Options/ColorOrder", static_cast<uint>(colorMode));

    switch(colorMode) {
    case Pattern::RGB:
        actionRGB->setChecked(true);
        actionGRB->setChecked(false);
        break;
    case Pattern::GRB:
        actionRGB->setChecked(false);
        actionGRB->setChecked(true);
        break;
    }
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
    int ledCount = settings.value("Options/ledCount", DEFAULT_LED_COUNT).toUInt();

    PatternItem* patternItem = new PatternItem(patternLength, ledCount);

    undoStackGroup->addStack(patternItem->getUndoStack());

    this->patternCollection->addItem(patternItem);
    this->patternCollection->setCurrentItem(patternItem);
}

void MainWindow::on_actionClose_triggered()
{
    // TODO: Fix framework to allow no active image
    //if(this->patternCollection->currentItem() == NULL) {
    if(this->patternCollection->count() < 2) {
        qDebug() << "on_actionClose: No items left to remove!";
        return;
    }

    PatternItem* patternItem = dynamic_cast<PatternItem*>(patternCollection->currentItem());
    if(!promptForSave(patternItem)) {
        return;
    }

    // TODO: remove the undo stack from the undo group?
    this->patternCollection->takeItem(this->patternCollection->currentRow());
}

void MainWindow::setPatternItem(QListWidgetItem* current, QListWidgetItem* previous) {
    Q_UNUSED(previous);

    PatternItem* newPatternItem = dynamic_cast<PatternItem*>(current);

    // Set a callback so that the pattern item can inform the UI when it is modified
//    newPatternItem->setUpdateCallback(&this->notifyPatternModified);
//    newPatternItem->setUpdateCallback(NULL);

    patternEditor->setPatternItem(newPatternItem);
    undoStackGroup->setActiveStack(newPatternItem->getUndoStack());

    on_patternNameChanged(newPatternItem->getPatternName());
}

void MainWindow::notifyPatternModified() {
    on_forcePatternEditorRedraw();
}
