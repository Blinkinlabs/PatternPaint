#ifndef BLINKYPENDANTUPLOADDATATESTS_H
#define BLINKYPENDANTUPLOADDATATESTS_H

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
};

#endif // BLINKYPENDANTUPLOADDATATESTS_H
