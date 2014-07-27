#include "pattern.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "colormodel.h"
#include "systeminformation.h"
#include "aboutpatternpaint.h"
#include "resizepattern.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QtWidgets>

// TODO: Move this to pattern uploader or something?
#include "ColorSwirl_Sketch.h"

#define DEFAULT_PATTERN_HEIGHT 60
#define DEFAULT_PATTERN_LENGTH 100

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO: should this go somewhere else?
    qSetMessagePattern("%{type} %{function}: %{message}");

    // Windows only?
    // TODO: Run on windows to see if this works
//    setWindowIcon(QIcon(":/resources/images/blinkytape.ico"));

    ui->patternEditor->init(DEFAULT_PATTERN_LENGTH, DEFAULT_PATTERN_HEIGHT);
    ui->colorPicker->init();

    // Our pattern editor wants to get some notifications
    connect(ui->colorPicker, SIGNAL(colorChanged(QColor)),
            ui->patternEditor, SLOT(setToolColor(QColor)));
    connect(ui->penSize, SIGNAL(valueChanged(int)),
            ui->patternEditor, SLOT(setToolSize(int)));

//     Now that our window is drawn, fix the vertical height so it can't be changed by the user
//    // TODO: some sort of scaling instead of fixing this? A non-hack way of doing it?
//    setFixedHeight(height());

    // The draw timer tells the pattern to advance
    drawTimer = new QTimer(this);
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawTimer_timeout()));
    drawTimer->start(33);

    tape = new BlinkyTape(this, DEFAULT_PATTERN_HEIGHT);

    // Modify our UI when the tape connection status changes
    connect(tape, SIGNAL(connectionStatusChanged(bool)),
            this,SLOT(on_tapeConnectionStatusChanged(bool)));


    uploader = new PatternUploader(this);

    // TODO: Should this be a separate view? it seems weird to have it chillin
    // all static like.
    connect(uploader, SIGNAL(progressChanged(int)),
            this, SLOT(on_uploaderProgressChanged(int)));
    connect(uploader, SIGNAL(finished(bool)),
            this, SLOT(on_uploaderFinished(bool)));

    // Set some default values for the painting interface
    ui->penSize->setSliderPosition(2);
    ui->patternSpeed->setSliderPosition(30);


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
}

MainWindow::~MainWindow()
{
    delete ui;
}


#define MIN_INTERVAL 10  // minimum interval to wait before firing a drawtimer update

void MainWindow::drawTimer_timeout() {
    // TODO: move this state to somewhere; the patternEditor class maybe?
    static int n = 0;

    static qint64 lastTime = 0;

    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_INTERVAL) {
        qDebug() << "Too short time, last interval:" << lastTime << "Now:" << newTime;
        return;
    }

    lastTime = newTime;

    // TODO: drop timeouts that come too quickly after a previous one (maybe 10ms dead time)

    // TODO: Get the width from elsewhere, so we don't need to load the image every frame
    QImage image = ui->patternEditor->getPatternAsImage();

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
        ui->patternEditor->setPlaybackRow(n);
    }
}


void MainWindow::on_tapeConnectDisconnect_clicked()
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

void MainWindow::on_patternSpeed_valueChanged(int value)
{
    drawTimer->setInterval(1000/value);
}

void MainWindow::on_patternPlayPause_clicked()
{
    if(drawTimer->isActive()) {
        drawTimer->stop();
        ui->patternPlayPause->setText("Play");
    }
    else {
        drawTimer->start();
        ui->patternPlayPause->setText("Pause");
    }
}

void MainWindow::on_actionLoad_File_triggered()
{
    // TODO: Add a simple image gallery thing instead of this, and push
    // this to 'import' and 'export'
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Pattern"), "", tr("Pattern Files (*.png *.jpg *.bmp)"));

    if(fileName.length() == 0) {
        return;
    }

    QImage pattern;

    // TODO: How to handle stuff that's not the right size?
    // Right now we always resize, could offer to crop, center, etc instead.
    if(!pattern.load(fileName)) {
        qDebug() << "Error loading pattern file " << fileName;
        return;
    }

    ui->patternEditor->init(pattern);
}

void MainWindow::on_actionSave_File_triggered()
{
    //TODO: Track if we already had an open file to enable this, add save as?

    // TODO: Add a simple image gallery thing instead of this, and push
    // this to 'import' and 'export'
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Pattern"), "", tr("Pattern Files (*.png *.jpg *.bmp)"));

    if(fileName.length() == 0) {
        return;
    }

    // TODO: Alert the user if this failed.
    if(!ui->patternEditor->getPatternAsImage().save(fileName)) {
        QMessageBox::warning(this, tr("Error"), tr("Error, cannot write file %1.")
                       .arg(fileName));
    }
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_saveToTape_clicked()
{
    on_actionSave_to_Tape_triggered();
}

void MainWindow::on_actionExport_pattern_for_Arduino_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Pattern for Arduino"), "pattern.h", tr("Header File (*.h)"));

    if(fileName.length() == 0) {
        return;
    }

    // Convert the current pattern into a Pattern
    QImage image =  ui->patternEditor->getPatternAsImage();

    // Note: Converting frameRate to frame delay here.
    Pattern pattern(image,
                        1000/ui->patternSpeed->value(),
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
    if(connected) {
        ui->tapeConnectDisconnect->setText("Disconnect");
        ui->saveToTape->setEnabled(true);
    }
    else {
        ui->tapeConnectDisconnect->setText("Connect");
        ui->saveToTape->setEnabled(false);
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
    qDebug() << "Uploader finished! Result:" << result;

    progressDialog->hide();

    // Reconnect to the BlinkyTape
    if(!tape->isConnected()) {
        // TODO: Make connect() function that does this automagically?
        QList<QSerialPortInfo> tapes = BlinkyTape::findBlinkyTapes();
        qDebug() << "Tapes found:" << tapes.length();

        if(tapes.length() > 0) {
            // TODO: Try another one if this one fails?
            qDebug() << "Attempting to connect to tape on:" << tapes[0].portName();
            tape->open(tapes[0]);
        }
    }
}


void MainWindow::on_saveFile_clicked()
{
    on_actionSave_File_triggered();
}

void MainWindow::on_loadFile_clicked()
{
    on_actionLoad_File_triggered();
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
    QImage image =  ui->patternEditor->getPatternAsImage();
    ui->patternEditor->init(image.mirrored(true, false));
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    // TODO: This in a less hacky way?
    // TODO: Undo/redo
    QImage image =  ui->patternEditor->getPatternAsImage();
    ui->patternEditor->init(image.mirrored(false, true));
}

void MainWindow::on_actionClear_Pattern_triggered()
{
    // TODO: This in a less hacky way?
    // TODO: Undo/redo
    QImage image =  ui->patternEditor->getPatternAsImage();
    image.fill(0);
    ui->patternEditor->init(image);
}

void MainWindow::on_actionLoad_rainbow_sketch_triggered()
{
    if(!(tape->isConnected())) {
        return;
    }

    QByteArray sketch = QByteArray(ColorSwirlSketch,COLORSWIRL_LENGTH);
    uploader->startUpload(*tape, sketch);

    progressDialog->setValue(progressDialog->minimum());
    progressDialog->show();
}

void MainWindow::on_actionSave_to_Tape_triggered()
{
    if(!(tape->isConnected())) {
        return;
    }

    // Convert the current pattern into a Pattern
    QImage image =  ui->patternEditor->getPatternAsImage();

    // Note: Converting frameRate to frame delay here.
    Pattern pattern(image,
                        1000/ui->patternSpeed->value(),
                        Pattern::INDEXED_RLE);

    // TODO: Attempt different compressions till one works.

    qDebug() << "Color count: " << pattern.colorCount();

    if(!uploader->startUpload(*tape, pattern)) {
        errorMessageDialog->setText(uploader->getErrorString());
        errorMessageDialog->show();
        return;
    }

    progressDialog->setValue(progressDialog->minimum());
    progressDialog->show();
}


void MainWindow::on_actionResize_Pattern_triggered()
{
    int patternLength = ui->patternEditor->getPatternAsImage().width();
    int ledCount = ui->patternEditor->getPatternAsImage().height();

    // TODO: Dispose of this?
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
        QImage originalImage = ui->patternEditor->getPatternAsImage();
        painter.drawImage(0,0,originalImage);

        ui->patternEditor->init(newImage, false);
    }
}
