#ifndef BYTEARRAYHELPERSTESTS_H
#define BYTEARRAYHELPERSTESTS_H

#include "multitests.h"

#include <QObject>

class ByteArrayHelpersTests : public QObject
{
    Q_OBJECT
private slots:
    void uint16ToByteArrayLittleTest_data();
    void uint16ToByteArrayLittleTest();

    void uint16ToByteArrayBigTest_data();
    void uint16ToByteArrayBigTest();

    void uint32ToByteArrayLittleTest_data();
    void uint32ToByteArrayLittleTest();

    void uint32ToByteArrayBigTest_data();
    void uint32ToByteArrayBigTest();

    void byteArrayToUint32LittleTest_data();
    void byteArrayToUint32LittleTest();

    void byteArrayToUint32BigTest_data();
    void byteArrayToUint32BigTest();

    void chunkDataTest_data();
    void chunkDataTest();

    void padToBoundaryTest_data();
    void padToBoundaryTest();
};

TEST_DECLARE(ByteArrayHelpersTests)

#endif // BYTEARRAYHELPERSTESTS_H
