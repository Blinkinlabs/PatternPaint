#ifndef BYTEARRAYHELPERSTESTS_H
#define BYTEARRAYHELPERSTESTS_H

#include <QObject>

class ByteArrayHelpersTests : public QObject
{
    Q_OBJECT
private slots:
    void uint16ToByteArrayLittleTest_data();
    void uint16ToByteArrayLittleTest();

    void uint16ToByteArrayBigTest_data();
    void uint16ToByteArrayBigTest();

    void uint32ToByteArrayTest_data();
    void uint32ToByteArrayTest();

    void chunkDataTest_data();
    void chunkDataTest();

    void padToBoundaryTest_data();
    void padToBoundaryTest();
};

#endif // BYTEARRAYHELPERSTESTS_H
