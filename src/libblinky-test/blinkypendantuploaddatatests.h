#ifndef BLINKYPENDANTUPLOADDATATESTS_H
#define BLINKYPENDANTUPLOADDATATESTS_H

#include "multitests.h"

#include <QObject>

class BlinkyPendantUploadDataTests: public QObject
{
    Q_OBJECT
private slots:
    void makePatternHeaderTableTest();
    void makePatternTableEntryTest();

    void noPatternsFailsTest();
    void maxPatternsSucceedsTest();
    void tooManyPatternsFailsTest();

    void wrongEncodingFails();
//    void tooLongFrameCountFails();
//    void tooLongFrameDelayFails();
    void wrongLEDCountFails();

    void uploadDataTest();
};

TEST_DECLARE(BlinkyPendantUploadDataTests)

#endif // BLINKYPENDANTUPLOADDATATESTS_H
