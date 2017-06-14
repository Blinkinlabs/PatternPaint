#ifndef BLINKYTAPEUPLOADDATATESTS_H
#define BLINKYTAPEUPLOADDATATESTS_H

#include "multitests.h"

#include <QObject>

class BlinkyTapeUploadDataTests : public QObject
{
    Q_OBJECT
private slots:
    void makePatternHeaderTableTest();
    void makePatternTableEntryTest();

    void makeBrightnessTest_data();
    void makeBrightnessTest();

    void badFirmwareNameTest();
    void noPatternsFailsTest();
    void maxPatternsSucceedsTest();
    void tooManyPatternsFailsTest();

    void padsFirmwareSectionTest();

    // TODO: Finish tests!
};

TEST_DECLARE(BlinkyTapeUploadDataTests)

#endif // BLINKYTAPEUPLOADDATATESTS_H
