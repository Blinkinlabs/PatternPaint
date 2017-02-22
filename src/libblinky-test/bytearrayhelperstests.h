#ifndef BYTEARRAYHELPERSTESTS_H
#define BYTEARRAYHELPERSTESTS_H

#include <QObject>

class ByteArrayHelpersTests : public QObject
{
    Q_OBJECT
private slots:
    void uint16ToByteArrayTest_data();
    void uint16ToByteArrayTest();

    void uint32ToByteArrayTest_data();
    void uint32ToByteArrayTest();

    void chunkDataTest_data();
    void chunkDataTest();
};

#endif // BYTEARRAYHELPERSTESTS_H
