#include "pattern.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "colormodel.h"
#include "systeminformation.h"
#include "aboutpatternpaint.h"
#include "resizepattern.h"
#include "undocommand.h"
#include "colorchooser.h"


#include "pencilinstrument.h"
#include "lineinstrument.h"
#include "colorpickerinstrument.h"
#include "sprayinstrument.h"
#include "fillinstrument.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QtWidgets>
#include <QUndoGroup>
#include <QToolButton>

// TODO: Move this to pattern uploader or something?
#include "ColorSwirl_Sketch.h"

#define DEFAULT_PATTERN_HEIGHT 60
#define DEFAULT_PATTERN_LENGTH 100

#define MIN_TIMER_INTERVAL 10  // minimum interval to wait before firing a drawtimer update

#define CONNECTION_SCANNER_INTERVAL 100

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);


    // prepare undo/redo
    menuEdit->addSeparator();
    m_undoStackGroup = new QUndoGroup(this);
    m_undoAction = m_undoStackGroup->createUndoAction(this, tr("&Undo"));
    m_undoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Z")));
    m_undoAction->setEnabled(false);
    menuEdit->addAction(m_undoAction);

    m_redoAction = m_undoStackGroup->createRedoAction(this, tr("&Redo"));
    m_redoAction->setEnabled(false);
    m_redoAction->setShortcut(QKeySequence(QString::fromUtf8("Ctrl+Y")));
    menuEdit->addAction(m_redoAction);

    m_undoStackGroup->addStack(patternEditor->getUndoStack());
    m_undoStackGroup->setActiveStack(patternEditor->getUndoStack());

    // instruments
    ColorpickerInstrument* cpi = new ColorpickerInstrument(this);
    connect(cpi, SIGNAL(pickedColor(QColor)), SLOT(on_colorPicked(QColor)));

    connect(actionPen, SIGNAL(triggered(bool)), SLOT(on_instrumentAction(bool)));
    connect(actionLine, SIGNAL(triggered(bool)), SLOT(on_instrumentAction(bool)));
    connect(actionSpray, SIGNAL(triggered(bool)), SLOT(on_instrumentAction(bool)));
    connect(actionPipette, SIGNAL(triggered(bool)), SLOT(on_instrumentAction(bool)));
    connect(actionFill, SIGNAL(triggered(bool)), SLOT(on_instrumentAction(bool)));

    actionPen->setData(QVariant::fromValue(new PencilInstrument(this)));
    actionLine->setData(QVariant::fromValue(new LineInstrument(this)));
    actionPipette->setData(QVariant::fromValue(cpi));
    actionSpray->setData(QVariant::fromValue(new SprayInstrument(this)));
    actionFill->setData(QVariant::fromValue(new FillInstrument(this)));

    m_colorChooser = new ColorChooser(255, 255, 255, this);
    m_colorChooser->setStatusTip(tr("Pen color"));
    m_colorChooser->setToolTip(tr("Pen color"));
    instruments->addSeparator();
    instruments->addWidget(m_colorChooser);

    QSpinBox *penSizeSpin = new QSpinBox();
    penSizeSpin->setRange(1, 20);
    penSizeSpin->setValue(1);
    penSizeSpin->setStatusTip(tr("Pen size"));
    penSizeSpin->setToolTip(tr("Pen size"));
    instruments->addWidget(penSizeSpin);

    // tools
    pSpeed = new QSpinBox(this);
    pSpeed->setEnabled(false);
    pSpeed->setRange(1, 100);
    pSpeed->setValue(20);
    pSpeed->setToolTip(tr("Pattern speed"));
    tools->addWidget(pSpeed);
    connect(pSpeed, SIGNAL(valueChanged(int)), this, SLOT(on_patternSpeed_valueChanged(int)));

    drawTimer = new QTimer(this);
    connectionScannerTimer = new QTimer(this);

    mode = Disconnected;

    patternEditor->init(DEFAULT_PATTERN_LENGTH, DEFAULT_PATTERN_HEIGHT);

    // Our pattern editor wants to get some notifications
    connect(m_colorChooser, SIGNAL(sendColor(QColor)),
            patternEditor, SLOT(setToolColor(QColor)));
    connect(patternEditor, SIGNAL(changed(bool)), SLOT(on_patternChanged(bool)));
    connect(patternEditor, SIGNAL(resized()), SLOT(on_patternResized()));

    connect(patternEditor, SIGNAL(changed(bool)), this, SLOT(on_imageChanged(bool)));

    tape = new BlinkyTape(this);
    // Modify our UI when the tape connection status changes
    connect(tape, SIGNAL(connectionStatusChanged(bool)),
            this,SLOT(on_tapeConnectionStatusChanged(bool)));

    // TODO: Make this on demand by calling the blinkytape object?
    uploader = new AvrPatternUploader(this);

    // TODO: Should this be a separate view? it seems weird to have it chillin
    // all static like.
    connect(uploader, SIGNAL(maxProgressChanged(int)),
            this, SLOT(on_uploaderMaxProgressChanged(int)));
    connect(uploader, SIGNAL(progressChanged(int)),
            this, SLOT(on_uploaderProgressChanged(int)));
    connect(uploader, SIGNAL(finished(bool)),
            this, SLOT(on_uploaderFinished(bool)));

    connect(penSizeSpin, SIGNAL(valueChanged(int)), patternEditor, SLOT(setToolSize(int)));

    // Pre-set the upload progress dialog
    progressDialog = new QProgressDialog(this);
    progressDialog->setWindowTitle("BlinkyTape exporter");
    progressDialog->setLabelText("Saving pattern to BlinkyTape...");
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(150);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setAutoClose(false);

    errorMessageDialog = new QMessageBox(this);
    errorMessageDialog->setWindowModality(Qt::WindowModal);


    // The draw timer tells the pattern to advance
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawTimer_timeout()));
    drawTimer->setInterval(33);


    // Start a scanner to connect to a BlinkyTape automatically
    connect(connectionScannerTimer, SIGNAL(timeout()), this, SLOT(connectionScannerTimer_timeout()));
    connectionScannerTimer->setInterval(CONNECTION_SCANNER_INTERVAL);
    connectionScannerTimer->start();

    // Initial values for interface
    penSizeSpin->setValue(1);
    patternEditor->setToolSize(1);
    patternEditor->setToolColor(QColor(255,255,255));
    actionPen->setChecked(true);
    patternEditor->setInstrument(qvariant_cast<AbstractInstrument*>(actionPen->data()));
    readSettings();

    this->setWindowTitle("Untitled - Pattern Paint");
}

MainWindow::~MainWindow(){}

void MainWindow::drawTimer_timeout() {

    // TODO: move this state to somewhere; the patternEditor class maybe?
    static int n = 0;

    // Ignore the timeout if it came to quickly, so that we don't overload the tape
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_TIMER_INTERVAL) {
        qDebug() << "Dropping timer update due to rate limiting. Last update " << newTime - lastTime << "ms ago";
        return;
    }

    lastTime = newTime;


    // TODO: Get the width from elsewhere, so we don't need to load the image every frame
    QImage image = patternEditor->getPatternAsImage();

    if(tape->isConnected()) {
        QByteArray ledData;

        for(int i = 0; i < image.height(); i++) {
            QRgb color = ColorModel::correctBrightness(image.pixel(n, i));
            ledData.append(qRed(color));
            ledData.append(qGreen(color));
            ledData.append(qBlue(color));
        }
        tape->sendUpdate(ledData);

        n = (n+1)%image.width();
        patternEditor->setPlaybackRow(n);
    }
}


void MainWindow::connectionScannerTimer_timeout() {
    // If we are already connected, disregard.
    if(tape->isConnected() || mode==Uploading) {
        return;
    }

    // Check if our serial port is on the list
    QList<QSerialPortInfo> tapes = BlinkyTape::findBlinkyTapes();

    if(tapes.length() > 0) {
        on_actionConnect_triggered();
        return;
    }
}

void MainWindow::on_patternSpeed_valueChanged(int value)
{
    drawTimer->setInterval(1000/value);
}

void MainWindow::on_actionPlay_triggered()
{
    if (drawTimer->isActive()) {
        drawTimer->stop();
        actionPlay->setText(tr("Play"));
        actionPlay->setIcon(QIcon(":/resources/images/play.png"));
    } else {
        drawTimer->start();
        actionPlay->setText(tr("Pause"));
        actionPlay->setIcon(QIcon(":/resources/images/pause.png"));
    }
}

void MainWindow::on_actionLoad_File_triggered()
{
    if(!promptForSave()) {
        return;
    }

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

    QImage pattern;

    if(!pattern.load(fileName)) {
        errorMessageDialog->setText("Could not open file " + fileName + ". Perhaps it has a formatting problem?");
        errorMessageDialog->show();
        return;
    }

    on_patternFilenameChanged(fileInfo);

    patternEditor->init(pattern);
    patternEditor->setEdited(false);
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

    patternEditor->setEdited(!saveFile(fileInfo));
}

void MainWindow::on_actionSave_File_triggered() {
    if (m_lastFileInfo.fileName() == "") {
        on_actionSave_File_as_triggered();
    } else {
        patternEditor->setEdited(!saveFile(m_lastFileInfo));
    }
}

void MainWindow::on_actionExit_triggered()
{
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
                        Pattern::INDEXED_RLE);


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


void MainWindow::on_tapeConnectionStatusChanged(bool connected)
{
    qDebug() << "status changed, connected=" << connected;
    actionSave_to_Tape->setEnabled(connected);
    actionPlay->setEnabled(connected);
    pSpeed->setEnabled(connected);

    if(connected) {
        mode = Connected;
        actionConnect->setText(tr("Disconnect"));
        actionConnect->setIcon(QIcon(":/images/resources/disconnect.png"));
    }
    else {
        mode = Disconnected;
        actionConnect->setText(tr("Connect"));
        actionConnect->setIcon(QIcon(":/images/resources/connect.png"));

        // TODO: Don't do this if we disconnected intentionally.
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

    qDebug() << "Uploader finished! Result:" << result;
    progressDialog->hide();
}

void MainWindow::on_actionVisit_the_BlinkyTape_forum_triggered()
{
    QDesktopServices::openUrl(QUrl("http://forums.blinkinlabs.com/", QUrl::TolerantMode));
}

void MainWindow::on_actionTroubleshooting_tips_triggered()
{
    QDesktopServices::openUrl(QUrl("http://blinkinlabs.com/blinkytape/docs/troubleshooting/", QUrl::TolerantMode));
}

void MainWindow::on_actionFlip_Horizontal_triggered()
{
    // TODO: This in a less hacky way?
    // TODO: Undo/redo
    QImage image =  patternEditor->getPatternAsImage();
    patternEditor->init(image.mirrored(true, false));
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    // TODO: This in a less hacky way?
    // TODO: Undo/redo
    QImage image =  patternEditor->getPatternAsImage();
    patternEditor->init(image.mirrored(false, true));
}

void MainWindow::on_actionClear_Pattern_triggered()
{
    // TODO: This in a less hacky way?
    // TODO: Undo/redo
    QImage image =  patternEditor->getPatternAsImage();
    image.fill(0);
    patternEditor->init(image);
}

void MainWindow::on_actionLoad_rainbow_sketch_triggered()
{
    if(!(tape->isConnected())) {
        return;
    }

    QByteArray sketch = QByteArray(reinterpret_cast<const char*>(COLORSWIRL_DATA),COLORSWIRL_LENGTH);

    if(!uploader->startUpload(*tape, sketch)) {
        errorMessageDialog->setText(uploader->getErrorString());
        errorMessageDialog->show();
        return;
    }
    mode = Uploading;

    progressDialog->setValue(progressDialog->minimum());
    progressDialog->show();
}

void MainWindow::on_actionSave_to_Tape_triggered()
{
    if(!(tape->isConnected())) {
        return;
    }

    // Convert the current pattern into a Pattern
    QImage image =  patternEditor->getPatternAsImage();

    // Note: Converting frameRate to frame delay here.
    Pattern pattern(image,
                        drawTimer->interval(),
                        Pattern::RGB24);

    // TODO: Attempt different compressions till one works.

    qDebug() << "Color count: " << pattern.colorCount();

    std::vector<Pattern> patterns;
    patterns.push_back(pattern);

    if(!uploader->startUpload(*tape, patterns)) {
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
    int patternLength = patternEditor->getPatternAsImage().width();
    int ledCount = patternEditor->getPatternAsImage().height();

    ResizePattern* resizer = new ResizePattern(this);
    resizer->setWindowModality(Qt::WindowModal);
    resizer->setLength(patternLength);
    resizer->setLedCount(ledCount);
    resizer->exec();

    if(resizer->result() != QDialog::Accepted) {
        return;
    }

    // TODO: Data validation
    if(resizer->length() > 0) {

        qDebug() << "Resizing pattern, length:"
                 << resizer->length()
                 << "height:"
                 << resizer->ledCount();

        // Create a new pattern, filled with a black color
        QImage newImage(resizer->length(),
                            resizer->ledCount(),
                            QImage::Format_RGB32);
        newImage.fill(QColor(0,0,0,0));

        // Copy over whatever portion of the original pattern will fit
        QPainter painter(&newImage);
        QImage originalImage = patternEditor->getPatternAsImage();
        patternEditor->pushUndoCommand(new UndoCommand(originalImage, *(patternEditor)));
        painter.drawImage(0,0,originalImage);
        patternEditor->init(newImage, false);
    }
}

void MainWindow::on_actionAddress_programmer_triggered()
{
//    int patternLength = patternEditor->getPatternAsImage().width();
//    int ledCount = patternEditor->getPatternAsImage().height();

    // TODO: Dispose of this?
    AddressProgrammer* programmer = new AddressProgrammer(this);
    programmer->setWindowModality(Qt::WindowModal);
    programmer->exec();
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
    if(!promptForSave()) {
        event->ignore();
        return;
    }

    writeSettings();
    event->accept();
}

void MainWindow::on_actionConnect_triggered()
{
    if(tape->isConnected()) {
        qDebug() << "Disconnecting from tape";
        tape->close();
    }
    else {
        QList<QSerialPortInfo> tapes = BlinkyTape::findBlinkyTapes();
        qDebug() << "Tapes found:" << tapes.length();

        if(tapes.length() > 0) {
            // TODO: Try another one if this one fails?
            qDebug() << "Attempting to connect to tape on:" << tapes[0].portName();
            tape->open(tapes[0]);
        }
    }
}

void MainWindow::on_instrumentAction(bool) {
    QAction* act = dynamic_cast<QAction*>(sender());
    Q_ASSERT(act != NULL);
    foreach(QAction* a, instruments->actions()) {
        a->setChecked(false);
    }

    act->setChecked(true);
    patternEditor->setInstrument(qvariant_cast<AbstractInstrument*>(act->data()));
}

void MainWindow::on_colorPicked(QColor color) {
    m_colorChooser->setColor(color);
    patternEditor->setToolColor(color);
}

void MainWindow::on_patternChanged(bool changed) {
    Q_UNUSED(changed);
}

void MainWindow::on_patternResized() {
    scrollArea->resize(scrollArea->width()+1, scrollArea->height());
}

void MainWindow::on_imageChanged(bool changed)
{
    actionSave_File->setEnabled(changed);
}

void MainWindow::on_patternFilenameChanged(QFileInfo fileinfo)
{
    m_lastFileInfo = fileinfo;
    this->setWindowTitle(m_lastFileInfo.baseName() + " - Pattern Paint");
}

bool MainWindow::saveFile(const QFileInfo fileinfo) {
    if(fileinfo.fileName() == "") {
        return false;
    }

    if(!patternEditor->getPatternAsImage().save(fileinfo.filePath())) {
        QMessageBox::warning(this, tr("Error"), tr("Error saving pattern %1. Try saving it somewhere else?")
                       .arg(fileinfo.filePath()));
        return false;
    }

    on_patternFilenameChanged(fileinfo);
    return true;
}

int MainWindow::promptForSave() {
    if (patternEditor->isEdited() == false) {
        return true;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setText("The pattern has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ans = msgBox.exec();

    if (ans == QMessageBox::Save) {
        on_actionSave_File_triggered();

        return (patternEditor->isEdited() == false);
    }

    if (ans == QMessageBox::Cancel) {
        return false;
    }

    if (ans == QMessageBox::Discard) {
        return true;
    }

    return false;
}
