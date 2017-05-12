#ifndef PATTERNFRAMEMODELTESTS_H
#define PATTERNFRAMEMODELTESTS_H

#include <QObject>

class PatternFrameModelTests : public QObject
{
    Q_OBJECT
private slots:
    void emptyAtConstructionTest();

    void flagsInvalidIndexTest();
    void flagsValidIndexTest();

    void supportedDropActionsTest();

    void getUndoStackTest();

    // insertRows
    // removeRows

    void dataInvalidIndexTest();
    void dataIndexOutOfRangeTest();

    void setDataInvalidIndexTest();
    void setDataIndexOutOfRangeTest();

    void canSetFrameImageBadSizeTest();
    void canSetFrameImageTest();

    void canSetFrameSizeTest();
    void canSetFrameSpeedTest();
    void canSetFileNameTest();
    void canSetModifiedTest();
};

#endif // PATTERNFRAMEMODELTESTS_H
