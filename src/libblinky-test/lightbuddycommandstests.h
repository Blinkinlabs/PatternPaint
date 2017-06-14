#ifndef LIGHTBUDDYCOMMANDSTESTS_H
#define LIGHTBUDDYCOMMANDSTESTS_H

#include "multitests.h"

#include <QObject>

class LightBuddyCommandsTests : public QObject
{
    Q_OBJECT

private slots:
    void commandHeaderTest();

    void eraseFlashTest();

    void fileNewTest_data();
    void fileNewTest();

    void writePageTest_data();
    void writePageTest();

    void reloadAnimationsTest();
};

TEST_DECLARE(LightBuddyCommandsTests)

#endif // LIGHTBUDDYCOMMANDSTESTS_H
