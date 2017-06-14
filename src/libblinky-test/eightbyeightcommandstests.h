#ifndef EIGHTBYEIGHTCOMMANDSTESTS_H
#define EIGHTBYEIGHTCOMMANDSTESTS_H

#include "multitests.h"

#include <QObject>

class EightByEightCommandsTests : public QObject
{
    Q_OBJECT

private slots:
    void commandHeaderTest();
    void formatFilesystemTest();

    void openFileTest();
    void openFileNameTooLongCropsTest();

    void writeChunkTest_data();
    void writeChunkTest();

    void writeTest_data();
    void writeTest();

    void verifyChunkTest_data();
    void verifyChunkTest();

    void verifyTest_data();
    void verifyTest();

    void closeFileTest();

    void lockFileAccessTest();
    void unlockFileAccessTest();

    void getFirmwareVersionTest();
};

TEST_DECLARE(EightByEightCommandsTests)

#endif // EIGHTBYEIGHTCOMMANDSTESTS_H
