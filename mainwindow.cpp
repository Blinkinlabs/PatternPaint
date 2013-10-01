#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <cmath>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if(info.description().startsWith("BlinkyTape")) {
            std::cout << "starten" << std::endl;
            QSerialPort serial(info);
            if (serial.open(QIODevice::ReadWrite)) {

                std::cout << "writing" << std::endl;
                float phase = 0;
                while(true) {
                    int LED_COUNT = 60;

                    QByteArray LedData(LED_COUNT * 3, 0);

                    // Generate some color
                    for(int i = 0; i < LED_COUNT; i++) {
                        LedData[i*3    ] = (int)((std::sin(phase        + i/12.0) + 1)*127);
                        LedData[i*3 + 1] = (int)((std::sin(phase + 2.09 + i/12.0) + 1)*127);
                        LedData[i*3 + 2] = (int)((std::sin(phase + 4.18 + i/12.0) + 1)*127);
                    }


                    phase += .02;

                    // Chunk it out
                    int CHUNK_SIZE = 600;
                    for(int p = 0; p < LED_COUNT * 3; p += CHUNK_SIZE) {
                        int length = 0;
                        if(p + CHUNK_SIZE < LedData.length()) {
                            // send a whole chunk
                            length = CHUNK_SIZE;
                        }
                        else {
                            // send a partial chunk
                            length = LedData.length() - p;
                        }

                        QByteArray chunk(length, 0);
                        for(int i = 0; i < length; i++) {
                            chunk[i] = LedData[p + i];
                        }

                        int written = serial.write(chunk);
                        // if we didn't write everything, save it for later.
                        if(written == -1) {
                            exit(1);
                            p -= CHUNK_SIZE;
                        }
                        else if(written != length) {
                            exit(2);
                            p-= length - written;
                        }
                        serial.flush();

//                        std::cout << "p:" << p << " length:" << length << std::endl;
                    }

                    // Then send the flip command
                    QByteArray data(3, 0);
                    data[0] = 0xff;
                    data[1] = 0xff;
                    data[2] = 0xff;

                    serial.write(data);
                    serial.flush();
                }

                serial.close();
                std::cout << "done" << std::endl;
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
