#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <cmath>
#include <iostream>

#include "blinkytape.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    float phase = 0;
    while(true) {
        QList<QSerialPortInfo> tapes = BlinkyTape::findBlinkyTapes();
        if(tapes.length() > 0) {

            std::cout << "starten" << std::endl;
            BlinkyTape tape;

            std::cout << "connecting" << std::endl;
            if(tape.connect(tapes[0])) {
                std::cout << "writing" << std::endl;
                    int LED_COUNT = 60;

                    QByteArray LedData(LED_COUNT * 3, 0);

                    // Generate some color
                    for(int i = 0; i < LED_COUNT; i++) {
                        LedData[i*3    ] = (int)((std::sin(phase        + i/12.0) + 1)*127);
                        LedData[i*3 + 1] = (int)((std::sin(phase + 2.09 + i/12.0) + 1)*127);
                        LedData[i*3 + 2] = (int)((std::sin(phase + 4.18 + i/12.0) + 1)*127);
                    }


                    phase += .02;

                    tape.sendUpdate(LedData);
                std::cout << "done" << std::endl;
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
