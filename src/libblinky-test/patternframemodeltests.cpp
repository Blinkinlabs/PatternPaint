#include <QTest>

#include "patternframemodeltests.h"

#include "patternframemodel.h"

void PatternFrameModelTests::emptyAtConstructionTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QVERIFY(model.rowCount() == 0);
}

void PatternFrameModelTests::flagsInvalidIndexTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QVERIFY(model.flags(QModelIndex()) == (Qt::ItemIsEnabled | Qt::ItemIsDropEnabled));
}

void PatternFrameModelTests::flagsValidIndexTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QVERIFY(model.flags(model.index(0)) == (Qt::ItemIsEnabled
                                            | Qt::ItemIsSelectable
                                            | Qt::ItemIsEditable
                                            | Qt::ItemIsDragEnabled));
}

void PatternFrameModelTests::supportedDropActionsTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QVERIFY(model.supportedDropActions() == (Qt::CopyAction
                                             | Qt::MoveAction));
}

void PatternFrameModelTests::getUndoStackTest()
{
    // Just a cursory test
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QVERIFY(model.getUndoStack()->canUndo() == false);

    model.insertRows(0,1);
    QVERIFY(model.getUndoStack()->canUndo() == true);
}

void PatternFrameModelTests::dataInvalidIndexTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QVERIFY(model.data(QModelIndex(), PatternFrameModel::FrameImage) == QVariant());
}

void PatternFrameModelTests::dataIndexOutOfRangeTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    model.insertRows(0,1);
    QModelIndex modelIndex = model.index(0,0);
    model.removeRows(0,1);
    QVERIFY(modelIndex.row() == 0);

    QVERIFY(model.data(modelIndex, PatternFrameModel::FrameImage) == QVariant());
}

void PatternFrameModelTests::setDataInvalidIndexTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QVERIFY(model.setData(QModelIndex(), QImage(), PatternFrameModel::FrameImage) == false);
}

void PatternFrameModelTests::setDataIndexOutOfRangeTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    model.insertRows(0,1);
    QModelIndex modelIndex = model.index(0,0);
    model.removeRows(0,1);
    QVERIFY(modelIndex.row() == 0);

    QVERIFY(model.setData(modelIndex, QImage(), PatternFrameModel::FrameImage) == false);
}

void PatternFrameModelTests::canSetFrameImageBadSizeTest()
{
    QSize startSize(10,10);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0,0);

    QImage image(startSize*2, QImage::Format_ARGB32_Premultiplied);

    QVERIFY(model.setData(modelIndex, image, PatternFrameModel::FrameImage) == false);
}

void PatternFrameModelTests::canSetFrameImageTest()
{
    QSize startSize(10,10);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0,0);

    QImage image(startSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(FRAME_COLOR_DEFAULT);

    QVERIFY(model.data(modelIndex, PatternFrameModel::FrameImage).value<QImage>() == image);

    image.fill(Qt::GlobalColor::red);

    QVERIFY(model.setData(modelIndex, image, PatternFrameModel::FrameImage) == true);
    QVERIFY(model.data(modelIndex, PatternFrameModel::FrameImage).value<QImage>() == image);

    // TODO: Test that dataChanged() is signalled
}


void PatternFrameModelTests::canSetFrameSizeTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0,0);

    QVERIFY(model.data(modelIndex, PatternFrameModel::FrameSize).toSize() == startSize);

    QSize newSize(3,4);

    QVERIFY(model.setData(modelIndex, newSize, PatternFrameModel::FrameSize) == true);
    QVERIFY(model.data(modelIndex, PatternFrameModel::FrameSize).toSize() == newSize);

    QVERIFY(model.data(modelIndex, PatternFrameModel::FrameImage).value<QImage>().size() == newSize);

    // TODO: Test that dataChanged() is signalled
}

void PatternFrameModelTests::canSetFrameSpeedTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0,0);

    QVERIFY(model.data(modelIndex, PatternFrameModel::FrameSpeed).toFloat() == PATTERN_FRAME_SPEED_DEFAULT_VALUE);

    float frameSpeed = PATTERN_FRAME_SPEED_DEFAULT_VALUE+1;

    QVERIFY(model.setData(modelIndex, frameSpeed, PatternFrameModel::FrameSpeed) == true);
    QVERIFY(model.data(modelIndex, PatternFrameModel::FrameSpeed).toFloat() == frameSpeed);

    // TODO: Test that dataChanged() is signalled
}

void PatternFrameModelTests::canSetFileNameTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0,0);

    QVERIFY(model.data(modelIndex, PatternFrameModel::FileName).toString() == "");

    QString fileName = "TestTest";

    QVERIFY(model.setData(modelIndex, fileName, PatternFrameModel::FileName) == true);
    QVERIFY(model.data(modelIndex, PatternFrameModel::FileName).toString() == fileName);

    // TODO: Test that dataChanged() is signalled
}

void PatternFrameModelTests::canSetModifiedTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0,0);

    QVERIFY(model.data(modelIndex, PatternFrameModel::Modified).toBool() == true);

    QVERIFY(model.setData(modelIndex, false, PatternFrameModel::Modified) == true);
    QVERIFY(model.data(modelIndex, PatternFrameModel::Modified).toBool() == false);

    // TODO: Test that dataChanged() is signalled
}
