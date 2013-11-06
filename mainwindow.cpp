#include "ledwriter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <cmath>
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO: Run on windows to see if this works
    setWindowIcon(QIcon(":/resources/images/blinkytape.jpg"));

    // TODO: Standard init in QWidget we can override instead?
    ui->patternEditor->init(60,60);
    ui->colorPicker->init();

    // Our pattern editor wants to get some notifications
    connect(ui->colorPicker, SIGNAL(colorChanged(QColor)), ui->patternEditor, SLOT(setToolColor(QColor)));
    connect(ui->penSize, SIGNAL(valueChanged(int)), ui->patternEditor, SLOT(setToolSize(int)));

    m_drawTimer = new QTimer(this);
    connect(m_drawTimer, SIGNAL(timeout()), this, SLOT(drawTimerTimeout()));
    m_drawTimer->start(33);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    if(tape.isConnected()) {
        std::cout << "already connected to a tape!" << std::endl;
        return;
    }
    std::cout << "ok, lets find a tape!" << std::endl;


    QList<QSerialPortInfo> tapes = BlinkyTape::findBlinkyTapes();
    if(tapes.length() > 0) {
        std::cout << "connecting" << std::endl;
        if(tape.connect(tapes[0])) {
            // TODO: Stop after the first tape?
        }
    }
    else {
        std::cout << "No tapes to connect to!" << std::endl;
    }
}

void MainWindow::on_disconnectButton_clicked()
{
    if(tape.isConnected()) {
        tape.disconnect();
    }
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

        n = (n+1)%60;
        ui->patternEditor->setPlaybackRow(n);
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

void MainWindow::on_actionLoad_Image_triggered()
{
    // TODO: Add a simple image gallery thing instead of this, and push
    // this to 'import' and 'export'
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
    ui->patternEditor->init(fileName);
}

void MainWindow::on_uploadButton_clicked()
{
//    LedWriter a;
//    a.loadAnimation(ui->patternEditor->getPattern());
//    a.makeCHeader();

    // Convert the animation into a QByteArray
    // The RGB encoder just stores the data as R,G,B over and over again.
    QImage animation =  ui->patternEditor->getPattern();
//    QByteArray animationData;

//    for(int frame = 0; frame < animation.width(); frame++) {
//        for(int pixel = 0; pixel < animation.height(); pixel++) {
//            int color = animation.pixel(frame, pixel);
//            animationData.append(qRed(color));
//            animationData.append(qGreen(color));
//            animationData.append(qBlue(color));
//        }
//    }

    int LED_COUNT = 60;
    QByteArray ledData;
    QImage img = ui->patternEditor->getPattern();

    for(int frame = 0; frame < animation.width(); frame++) {
        for(int pixel = 0; pixel < animation.height(); pixel++) {
            int color = img.pixel(frame, pixel);
            ledData.append((color >> 16) & 0xff);
            ledData.append((color >>  8) & 0xff);
            ledData.append((color)       & 0xff);
        }
    }

    // Only if we are already conected, try to reset the strip.
    if(tape.isConnected()) {
        tape.uploadAnimation(ledData);
    }

}
