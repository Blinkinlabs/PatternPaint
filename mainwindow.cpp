#include "animation.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <systeminformation.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>

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
    progress = new QProgressDialog(this);
    progress->setWindowTitle("Uploader");
    progress->setLabelText("Uploading animation to BlinkyTape...");
    progress->setMinimum(0);
    progress->setMaximum(150);
    progress->setWindowModality(Qt::WindowModal);
    progress->setAutoClose(false);
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
            QRgb color = BlinkyTape::correctBrightness(img.pixel(n, i));
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

void MainWindow::on_actionLoad_Animation_triggered()
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

void MainWindow::on_actionSave_Animation_triggered()
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

void MainWindow::on_uploadAnimation_clicked()
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
    uploader->startUpload(*tape, animation);

    progress->setValue(progress->minimum());
    progress->show();

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
        ui->uploadAnimation->setEnabled(true);
    }
    else {
        ui->tapeConnectDisconnect->setText("Connect");
        ui->uploadAnimation->setEnabled(false);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Pattern Paint",
                       "<b>PatternPaint</b> is companion software for the BlinkyTape. "
                       "It allows you to create animations for your BlinkyTape in "
                       "real-time, and save your designs to the BlinkyTape for "
                       "playback on-the-go.");
}

void MainWindow::on_actionSystem_Information_triggered()
{
    // TODO: store this somewhere, for later disposal.
    SystemInformation* info = new SystemInformation(this);
    info->show();
}

void MainWindow::on_uploaderProgressChanged(int progressValue)
{
    // Clip the progress to maximum, until we work out a better way to estimate it.
    if(progressValue >= progress->maximum()) {
        progressValue = progress->maximum() - 1;
    }

    progress->setValue(progressValue);
}

void MainWindow::on_uploaderFinished(bool result)
{
    qDebug() << "Uploader finished! Result:" << result;

    progress->hide();

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


void MainWindow::on_saveAnimation_clicked()
{
    on_actionSave_Animation_triggered();
}

void MainWindow::on_loadAnimation_clicked()
{
    on_actionLoad_Animation_triggered();
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
    QImage pattern =  ui->animationEditor->getPattern();
    ui->animationEditor->init(pattern.mirrored(true, false));
}

void MainWindow::on_actionFlip_Vertical_triggered()
{
    // TODO: This in a less hacky way?
    QImage pattern =  ui->animationEditor->getPattern();
    ui->animationEditor->init(pattern.mirrored(false, true));
}

void MainWindow::on_actionLoad_rainbow_sketch_triggered()
{
    if(!(tape->isConnected())) {
        return;
    }

    // TODO: So close!
    QByteArray sketch = QByteArray(ColorSwirlSketch,COLORSWIRL_LENGTH);
    uploader->startUpload(*tape, sketch);

    progress->setValue(progress->minimum());
    progress->show();
}
