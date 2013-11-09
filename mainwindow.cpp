#include "ledwriter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <QSerialPortInfo>
#include <QSysInfo>
#include <QLibraryInfo>


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

    // TODO: Run on windows to see if this works
    setWindowIcon(QIcon(":/resources/images/blinkytape.jpg"));

    // TODO: Standard init in QWidget we can override instead?
    ui->patternEditor->init(DEFAULT_ANIMATION_LENGTH,BLINKYTAPE_STRIP_HEIGHT);
    ui->colorPicker->init();

    // Our pattern editor wants to get some notifications
    connect(ui->colorPicker, SIGNAL(colorChanged(QColor)), ui->patternEditor, SLOT(setToolColor(QColor)));
    connect(ui->penSize, SIGNAL(valueChanged(int)), ui->patternEditor, SLOT(setToolSize(int)));

    // The draw timer tells the animation to advance
    drawTimer = new QTimer(this);
    connect(drawTimer, SIGNAL(timeout()), this, SLOT(drawTimer_timeout()));
    drawTimer->start(33);

    // Modify our UI when the tape connection status changes
    connect(&tape, SIGNAL(connectionStatusChanged(bool)),this,SLOT(on_tapeConnectionStatusChanged(bool)));

    // Respond to the uploader
    // TODO: Should this be a separate view? it seems weird to have it chillin all static like.
    connect(&uploader, SIGNAL(progressChanged(float)),this,SLOT(on_uploadProgressChanged(float)));

    // Set some default values for the painting interface
    ui->penSize->setSliderPosition(2);
    ui->animationSpeed->setSliderPosition(30);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::drawTimer_timeout() {
    if(tape.isConnected()) {
        static int n = 0;

        int LED_COUNT = 60;
        QByteArray ledData(LED_COUNT * 3, 0);

        QImage img = ui->patternEditor->getPattern();
        for(int i = 0; i < LED_COUNT; i++) {
            int color = img.pixel(n,i);
            ledData[i*3  ] = (color >> 16) & 0xff;
            ledData[i*3+1] = (color >> 8) & 0xff;
            ledData[i*3+2] = (color) & 0xff;
        }
        tape.sendUpdate(ledData);

        n = (n+1)%img.width();
        ui->patternEditor->setPlaybackRow(n);
    }
}


void MainWindow::on_tapeConnectDisconnect_clicked()
{
    if(tape.isConnected()) {
        qDebug() << "Disconnecting from tape";
        tape.close();
    }
    else {
        QList<QSerialPortInfo> tapes = BlinkyTape::findBlinkyTapes();
        qDebug() << "Tapes found:" << tapes.length();

        if(tapes.length() > 0) {
            // TODO: Try another one if this one fails?
            qDebug() << "Attempting to connect to tape on:" << tapes[0].portName();
            tape.open(tapes[0]);
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

void MainWindow::on_actionOpen_Animation_triggered()
{
    // TODO: Add a simple image gallery thing instead of this, and push
    // this to 'import' and 'export'
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Animation"), "", tr("Animation Files (*.png *.jpg *.bmp)"));

    if(fileName.length() == 0) {
        return;
    }

    // TODO: Test if the file exists?
    ui->patternEditor->init(fileName);
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
    if(!ui->patternEditor->getPattern().save(fileName)) {
        QMessageBox::warning(this, tr("Error"), tr("Error, cannot write file %1.")
                       .arg(fileName));
    }
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_uploadButton_clicked()
{
    if(!tape.isConnected()) {
        return;
    }

    // Convert the animation into a QByteArray
    // The RGB encoder just stores the data as R,G,B over and over again.
    QImage animation =  ui->patternEditor->getPattern();

    QByteArray ledData;
    QImage img = ui->patternEditor->getPattern();

    for(int frame = 0; frame < animation.width(); frame++) {
        for(int pixel = 0; pixel < animation.height(); pixel++) {
            int color = img.pixel(frame, pixel);
            ledData.append(qRed(color));
            ledData.append(qGreen(color));
            ledData.append(qBlue(color));
        }
    }

//    tape.uploadAnimation(ledData,ui->animationSpeed->value());
    uploader.startUpload(tape, ledData,ui->animationSpeed->value());
}

void MainWindow::on_tapeConnectionStatusChanged(bool status)
{
    qDebug() << "status changed!";
    if(status) {
        ui->tapeConnectDisconnect->setText("Disconnect");
        ui->uploadButton->setEnabled(true);
    }
    else {
        ui->tapeConnectDisconnect->setText("Connect");
        ui->uploadButton->setEnabled(false);
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

void MainWindow::on_actionSystem_Report_triggered()
{
    // TODO: move to separate class

    QString report;

    QString osName;

    report.append("Pattern Paint (Unknown version)\r");
    report.append("  Build Date: ");
    report.append(__DATE__);
    report.append(" ");
    report.append(__TIME__);
    report.append("\r");

#if defined(Q_OS_WIN)
    switch(QSysInfo::windowsVersion()) {
    case QSysInfo::WV_2000:
        osName = "Windows 2000";
        break;
    case QSysInfo::WV_2003:
        osName = "Windows 2003";
        break;
    case QSysInfo::WV_VISTA:
        osName = "Windows Vista";
        break;
    case QSysInfo::WV_WINDOWS7:
        osName = "Windows 7";
        break;
    case QSysInfo::WV_WINDOWS8:
        osName = "Windows 8";
        break;
    default
        osName = "Windows (Unknown Version)";
        break;
    }
#elif defined(Q_OS_MAC)
    switch(QSysInfo::macVersion()) {
    case QSysInfo::MV_SNOWLEOPARD:
        osName = "OS X 10.6 (Snow Leopard)";
        break;
    case QSysInfo::MV_LION:
        osName = "OS X 10.7 (Lion)";
        break;
    case QSysInfo::MV_MOUNTAINLION:
        osName = "OS X 10.8 (Mountain Lion)";
        break;
    case QSysInfo::MV_MAVERICKS:
        osName = "OS X 10.9 (Mavericks)";
        break;
    default:
        osName = "OS X (Unknown version)";
        break;
    }
#else
// TODO: Linux
    osName = "Unknown";
#endif
    report.append("Operating system: " + osName + "\r");

    report.append("QT information:\r");
    report.append("  buildDate: " + QLibraryInfo::buildDate().toString() + "\r");
    report.append("  licensee: " + QLibraryInfo::licensee() + "\r");
    report.append("  path: " + QLibraryInfo::location(QLibraryInfo::LibrariesPath) + "\r");

    report.append("Detected Serial Ports: \r");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        report.append("  " + info.portName() + "\r");
        report.append("    Manufacturer: " + info.manufacturer() + "\r");
        report.append("    Description: " + info.description() + "\r");
        report.append("    VID: " + QString::number(info.vendorIdentifier()) + "\r");
        report.append("    PID: " + QString::number(info.productIdentifier()) + "\r");
    }

    report.append("Detected BlinkyTapes: \r");
    foreach (const QSerialPortInfo &info, BlinkyTape::findBlinkyTapes()) {
        report.append("  " + info.portName() + "\r");
        report.append("    Manufacturer: " + info.manufacturer() + "\r");
        report.append("    Description: " + info.description() + "\r");
        report.append("    VID: " + QString::number(info.vendorIdentifier()) + "\r");
        report.append("    PID: " + QString::number(info.productIdentifier()) + "\r");
    }


    QMessageBox::about(this, "System Information",
                       report);
}

void MainWindow::on_uploadProgressChanged(float progress)
{
    qDebug() << "Upload progess: ";
}
