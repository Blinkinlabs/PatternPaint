#include "ledwriter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <cmath>
#include <iostream>

// TODO: Change this when we connect to a tape, etc?
#define BLINKYTAPE_STRIP_HEIGHT 60
#define DEFAULT_ANIMATION_LENGTH 60

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO: Run on windows to see if this works
    setWindowIcon(QIcon(":/resources/images/blinkytape.jpg"));

    // TODO: Standard init in QWidget we can override instead?
    ui->patternEditor->init(DEFAULT_ANIMATION_LENGTH,BLINKYTAPE_STRIP_HEIGHT);
    ui->colorPicker->init();

    // Our pattern editor wants to get some notifications
    connect(ui->colorPicker, SIGNAL(colorChanged(QColor)), ui->patternEditor, SLOT(setToolColor(QColor)));
    connect(ui->penSize, SIGNAL(valueChanged(int)), ui->patternEditor, SLOT(setToolSize(int)));

    // The draw timer tells the animation to advance
    m_drawTimer = new QTimer(this);
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(drawTimerTimeout()));
    m_drawTimer->start(33);

    // Modify our UI when the tape connection status changes
    connect(&tape, SIGNAL(connectionStatusChanged(bool)),this,SLOT(on_tapeConnectionStatusChanged(bool)));

    // Set some default values for the painting interface
    ui->penSize->setSliderPosition(2);
    ui->animationSpeed->setSliderPosition(30);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::drawTimerTimeout() {
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
        tape.close();
    }
    else {
        QList<QSerialPortInfo> tapes = BlinkyTape::findBlinkyTapes();
        if(tapes.length() > 0) {
            // TODO: Try another one if this one fails?
            tape.open(tapes[0]);
        }
    }
}

void MainWindow::on_animationSpeed_valueChanged(int value)
{
    std::cout << "Changed animation speed: " << value << std::endl;
    m_drawTimer->setInterval(1000/value);
}

void MainWindow::on_animationPlayPause_clicked()
{
    if(m_drawTimer->isActive()) {
        m_drawTimer->stop();
        ui->animationPlayPause->setText("Play");
    }
    else {
        m_drawTimer->start();
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

    std::cout << "MainWindow::on_uploadButton_clicked: " << thread() << std::endl;
    tape.uploadAnimation(ledData,ui->animationSpeed->value());
}

void MainWindow::on_tapeConnectionStatusChanged(bool status)
{
    std::cout << "status changed!" << std::endl;
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
    QMessageBox::about(this, tr("About Pattern Paint"),
                       tr("<b>PatternPaint</b> is companion software for the BlinkyTape. "
                          "It allows you to create animations for your BlinkyTape in "
                          "real-time, and save your designs to the BlinkyTape for "
                          "playback on-the-go."));
}
