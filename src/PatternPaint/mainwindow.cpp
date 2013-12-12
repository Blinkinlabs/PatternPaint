#include "animation.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "colormodel.h"
#include "systeminformation.h"
#include "aboutpatternpaint.h"
#include "resizeanimation.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QtWidgets>

// TODO: Move this to animation uploader or something?
#include "ColorSwirl_Sketch.h"

// TODO: Change this when we connect to a tape, etc?
#define BLINKYTAPE_STRIP_HEIGHT 60
#define DEFAULT_ANIMATION_LENGTH 60

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

    // TODO: Standard init in QWidget we can override instead?
    ui->animationEditor->init(DEFAULT_ANIMATION_LENGTH,BLINKYTAPE_STRIP_HEIGHT);
    ui->colorPicker->init();

    // Our pattern editor wants to get some notifications
    connect(ui->colorPicker, SIGNAL(colorChanged(QColor)),
            ui->animationEditor, SLOT(setToolColor(QColor)));
    connect(ui->penSize, SIGNAL(valueChanged(int)),
            ui->animationEditor, SLOT(setToolSize(int)));

    // Now that our window is drawn, fix the vertical height so it can't be changed by the user
    // TODO: some sort of scaling instead of fixing this? A non-hack way of doing it?
    setFixedHeight(height());

    // The draw timer tells the animation to advance
    drawTimer = new QTimer(this);
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawTimer_timeout()));
    drawTimer->start(33);

    tape = new BlinkyTape(this);

    // Modify our UI when the tape connection status changes
    connect(tape, SIGNAL(connectionStatusChanged(bool)),
            this,SLOT(on_tapeConnectionStatusChanged(bool)));


    uploader = new AnimationUploader(this);

    // TODO: Should this be a separate view? it seems weird to have it chillin
    // all static like.
    connect(uploader, SIGNAL(progressChanged(int)),
            this, SLOT(on_uploaderProgressChanged(int)));
    connect(uploader, SIGNAL(finished(bool)),
            this, SLOT(on_uploaderFinished(bool)));

    // Set some default values for the painting interface
    ui->penSize->setSliderPosition(2);
    ui->animationSpeed->setSliderPosition(30);


    // Pre-set the upload progress dialog
    progressDialog = new QProgressDialog(this);
    progressDialog->setWindowTitle("BlinkyTape exporter");
    progressDialog->setLabelText("Saving animation to BlinkyTape...");
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


void MainWindow::drawTimer_timeout() {
    // TODO: move this state to somewhere; the animationEditor class maybe?
    static int n = 0;

    if(tape->isConnected()) {
        QByteArray ledData;

        QImage img = ui->animationEditor->getPattern();
        for(int i = 0; i < BLINKYTAPE_STRIP_HEIGHT; i++) {
            QRgb color = ColorModel::correctBrightness(img.pixel(n, i));
            ledData.append(qRed(color));
            ledData.append(qGreen(color));
            ledData.append(qBlue(color));
        }
        tape->sendUpdate(ledData);

        n = (n+1)%img.width();
        ui->animationEditor->setPlaybackRow(n);
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

void MainWindow::on_animationSpeed_valueChanged(int value)
{
    drawTimer->setInterval(1000/value);
}

void MainWindow::on_animationPlayPause_clicked()
{
    if(drawTimer->isActive()) {
        drawTimer->stop();
        ui->animationPlayPause->setText("Play");
    }
    else {
        drawTimer->start();
        ui->animationPlayPause->setText("Pause");
    }
}

void MainWindow::on_actionLoad_File_triggered()
{
    // TODO: Add a simple image gallery thing instead of this, and push
    // this to 'import' and 'export'
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Animation"), "", tr("Animation Files (*.png *.jpg *.bmp)"));

    if(fileName.length() == 0) {
        return;
    }

    QImage animation;

    // TODO: How to handle stuff that's not the right size?
    // Right now we always resize, could offer to crop, center, etc instead.
    if(!animation.load(fileName)) {
        qDebug() << "Error loading animation file " << fileName;
        return;
    }

    ui->animationEditor->init(animation);
}

void MainWindow::on_actionSave_File_triggered()
{
    //TODO: Track if we already had an open file to enable this, add save as?

    // TODO: Add a simple image gallery thing instead of this, and push
    // this to 'import' and 'export'
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Animation"), "", tr("Animation Files (*.png *.jpg *.bmp)"));

    if(fileName.length() == 0) {
        return;
    }

    // TODO: Alert the user if this failed.
    if(!ui->animationEditor->getPattern().save(fileName)) {
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

void MainWindow::on_actionExport_animation_for_Arduino_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Animation for Arduino"), "animation.h", tr("Header File (*.h)"));

    if(fileName.length() == 0) {
        return;
    }

    // Convert the current pattern into an Animation
    QImage pattern =  ui->animationEditor->getPattern();

    // Note: Converting frameRate to frame delay here.
    Animation animation(pattern,
                        1000/ui->animationSpeed->value(),
                        Animation::Encoding_RGB565_RLE);


    // Attempt to open the specified file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("Error"), tr("Error, cannot write file %1.")
                       .arg(fileName));
        return;
    }

    QTextStream ts(&file);
    ts << animation.header;
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
    QImage pattern =  ui->animationEditor->getPattern();
    ui->animationEditor->init(pattern.mirrored(true, false));
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    // TODO: This in a less hacky way?
    // TODO: Undo/redo
    QImage pattern =  ui->animationEditor->getPattern();
    ui->animationEditor->init(pattern.mirrored(false, true));
}

void MainWindow::on_actionClear_Animation_triggered()
{
    // TODO: This in a less hacky way?
    // TODO: Undo/redo
    QImage pattern =  ui->animationEditor->getPattern();
    pattern.fill(0);
    ui->animationEditor->init(pattern);
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

    // Convert the current pattern into an Animation
    QImage pattern =  ui->animationEditor->getPattern();

    // Note: Converting frameRate to frame delay here.
    Animation animation(pattern,
                        1000/ui->animationSpeed->value(),
                        Animation::Encoding_RGB565_RLE);

    if(!uploader->startUpload(*tape, animation)) {
        errorMessageDialog->setText(uploader->getErrorString());
        errorMessageDialog->show();
        return;
    }

    progressDialog->setValue(progressDialog->minimum());
    progressDialog->show();
}


void MainWindow::on_actionResize_Animation_triggered()
{
    int animationLength = ui->animationEditor->getPattern().width();

    // TODO: Dispose of this?
    ResizeAnimation* resizer = new ResizeAnimation(this);
    resizer->setWindowModality(Qt::WindowModal);
    resizer->setLength(animationLength);
    resizer->exec();

    if(resizer->result() != QDialog::Accepted) {
        return;
    }

    int newLength = resizer->length();
    if(newLength > 0) {
        qDebug() << "Resizing Animation to length: " << resizer->length();
        // TODO: This in a non-hacky way
        QImage originalAnimation = ui->animationEditor->getPattern();
        QImage newAnimation(newLength, originalAnimation.height(),QImage::Format_RGB32);

        newAnimation.fill(QColor(0,0,0,0));

        QPainter painter(&newAnimation);
        painter.drawImage(0,0,originalAnimation);

        ui->animationEditor->init(newAnimation);
    }
}
