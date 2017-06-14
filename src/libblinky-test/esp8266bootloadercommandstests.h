#ifndef ESP8266BOOTLOADERCOMMANDSTESTS_H
#define ESP8266BOOTLOADERCOMMANDSTESTS_H

#include "multitests.h"

#include <QObject>

class Esp8266BootloaderCommandsTests : public QObject
{
    Q_OBJECT

private slots:
    void calculateChecksumTest_data();
    void calculateChecksumTest();

    void buildCommandTest_data();
    void buildCommandTest();

    void slipEncodeTest_data();
    void slipEncodeTest();

    void slipDecodeTest_data();
    void slipDecodeTest();

    void slipEncodeDecodeTest();
};

TEST_DECLARE(Esp8266BootloaderCommandsTests)

#endif // ESP8266BOOTLOADERCOMMANDSTESTS_H
