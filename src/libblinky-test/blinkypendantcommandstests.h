#ifndef BLINKYPENDANTCOMMANDSTESTS_H
#define BLINKYPENDANTCOMMANDSTESTS_H

#include <QObject>

class BlinkyPendantCommandsTests: public QObject
{
    Q_OBJECT
private slots:
    void commandHeaderTest();
    void startWriteTest();
    void stopWriteTest();

    void writeFlashChunkTest_data();
    void writeFlashChunkTest();

    void writeFlashTest_data();
    void writeFlashTest();
};

#endif // BLINKYPENDANTCOMMANDSTESTS_H
