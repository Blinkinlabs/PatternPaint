#ifndef PATTERNSCROLLMODELTESTS_H
#define PATTERNSCROLLMODELTESTS_H

#include <QObject>

class PatternScrollModelTests : public QObject
{
    Q_OBJECT
private slots:
    void emptyAtConstructionTest();

    void flagsInvalidIndexTest();
    void flagsValidIndexTest();

    void supportedDropActionsTest();

    void getUndoStackTest();

    void insertRowsNegativeIndexFails();
    void insertRowsTooHighIndexFails();
    void insertRowsOneRow();
    void insertRowsMultipleRows();
    void insertRowsAtFront();
    void insertRowsAtBack();
    void insertRowsInMiddle();

    void removeRowsNegativeIndexFails();
    void removeRowsTooHighIndexFails();
    void removeRowsTooHighCountFails();
    void removeRowsOneRow();
    void removeRowsMultipleRows();
    void removeRowsAtFront();
    void removeRowsAtBack();
    void removeRowsInMiddle();

    void dataInvalidIndexTest();
    void dataIndexOutOfRangeTest();

    void setDataInvalidIndexTest();
    void setDataIndexOutOfRangeTest();

    void setFrameImageFailsTest();

    void canSetEditImageBadSizeTest();
    void canSetEditImageTest();

    void canSetFrameSizeTest();
    void canSetFrameSpeedTest();
    void canSetFileNameTest();
    void canSetModifiedTest();

    // TODO: Frame resize tests
    // TODO: getEditImage tests when the fixture size is larger than edit image size
    // TODO: frameImage / editImage cross tests

    void readFromStreamTest();
    void writeToStreamTest();
};

#endif // PATTERNSCROLLMODELTESTS_H
