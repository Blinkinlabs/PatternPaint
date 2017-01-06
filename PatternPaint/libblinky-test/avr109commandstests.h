#ifndef AVR109COMMANDSTESTS_H
#define AVR109COMMANDSTESTS_H

#include <QObject>

class Avr109CommandsTests: public QObject
{
    Q_OBJECT
private slots:
    void int16ToByteArrayTest_data();
    void int16ToByteArrayTest();

    void chunkDataTest_data();
    void chunkDataTest();

    void checkDeviceSignatureTest();
    void resetTest();
    void chipEraseTest();
    void setAddressTest();

    void writeFlashPageTest_data();
    void writeFlashPageTest();

    void verifyFlashPageTest_data();
    void verifyFlashPageTest();

    void writeEepromBlockTest();

    void writeFlashBadAddressTest();
    void writeFlashTest_data();
    void writeFlashTest();

    void verifyFlashBadAddressTest();
    void verifyFlashTest_data();
    void verifyFlashTest();

    void writeEepromTest_data();
    void writeEepromTest();
};

#endif // AVR109COMMANDSTESTS_H
