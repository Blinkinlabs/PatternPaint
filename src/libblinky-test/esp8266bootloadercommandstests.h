#ifndef ESP8266BOOTLOADERCOMMANDSTESTS_H
#define ESP8266BOOTLOADERCOMMANDSTESTS_H

#include <QObject>

class Esp8266BootloaderCommandsTests : public QObject
{
    Q_OBJECT

private slots:
    void calculateChecksumTest_data();
    void calculateChecksumTest();

    void slipEncodeTest_data();
    void slipEncodeTest();

    void slipDecodeTest_data();
    void slipDecodeTest();

    void slipEncodeDecodeTest();
};

#endif // ESP8266BOOTLOADERCOMMANDSTESTS_H
