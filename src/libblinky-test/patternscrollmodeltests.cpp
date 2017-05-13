#include <QBuffer>
#include <QSignalSpy>
#include <QTest>

#include "patternscrollmodeltests.h"

#include "patternscrollmodel.h"

void PatternScrollModelTests::emptyAtConstructionTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    QVERIFY(model.rowCount() == 0);
}

void PatternScrollModelTests::flagsInvalidIndexTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    QVERIFY(model.flags(QModelIndex()) == (Qt::ItemIsEnabled));
}

void PatternScrollModelTests::flagsValidIndexTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    model.insertRows(0,1);

    QVERIFY(model.flags(model.index(0)) == (Qt::ItemIsEnabled
                                            | Qt::ItemIsSelectable
                                            | Qt::ItemIsEditable));
}

void PatternScrollModelTests::supportedDropActionsTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    model.insertRows(0,1);

    QVERIFY(model.supportedDropActions() == 0);
}

void PatternScrollModelTests::getUndoStackTest()
{
    // Just a cursory test
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    QVERIFY(model.getUndoStack()->canUndo() == false);

    model.insertRows(0,1);
    QVERIFY(model.getUndoStack()->canUndo() == true);
}

void PatternScrollModelTests::insertRowsNegativeIndexFails()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    QVERIFY(model.rowCount() == 0);
    QVERIFY(model.insertRows(-1,1) == false);
}

void PatternScrollModelTests::insertRowsTooHighIndexFails()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    QVERIFY(model.rowCount() == 0);
    QVERIFY(model.insertRows(1,1) == false);
}

void PatternScrollModelTests::insertRowsOneRow()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QVERIFY(model.rowCount() == 0);
    QVERIFY(model.insertRows(0,1) == true);
    QVERIFY(model.rowCount() == 1);

    QVERIFY(spy.count() == 2); // 1 for 'modified', 1 for 'insert rows'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::insertRowsMultipleRows()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QVERIFY(model.rowCount() == 0);
    QVERIFY(model.insertRows(0,3) == true);
    QVERIFY(model.rowCount() == 3);

    QVERIFY(spy.count() == 2); // 1 for 'modified', 1 for 'insertRows'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::insertRowsAtFront()
{
    QSize startSize(1,5);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage defaultImage(startSize, QImage::Format_ARGB32_Premultiplied);
    defaultImage.fill(FRAME_COLOR_DEFAULT);

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QVERIFY(defaultImage != redImage);

    // Insert one row, then color its image red
    model.insertRows(0,1);
    model.setData(model.index(0), redImage, PatternScrollModel::EditImage);

    // Insert a row at the front, then verify the images are in the correct order.
    QVERIFY(model.insertRows(0,1) == true);
    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == defaultImage);
    QVERIFY(model.data(model.index(1), PatternScrollModel::FrameImage).value<QImage>() == redImage);

    QVERIFY(spy.count() == 4); // 1 for 'modified', 2 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::insertRowsAtBack()
{
    QSize startSize(1,5);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage defaultImage(startSize, QImage::Format_ARGB32_Premultiplied);
    defaultImage.fill(FRAME_COLOR_DEFAULT);

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QVERIFY(defaultImage != redImage);

    // Insert one row, then color its image red
    model.insertRows(0,1);
    model.setData(model.index(0), redImage, PatternScrollModel::EditImage);

    // Insert a row at the front, then verify the images are in the correct order.
    QVERIFY(model.insertRows(1,1) == true);

    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == redImage);
    QVERIFY(model.data(model.index(1), PatternScrollModel::FrameImage).value<QImage>() == defaultImage);

    QVERIFY(spy.count() == 4); // 1 for 'modified', 2 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::insertRowsInMiddle()
{
    QSize startSize(1,5);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage defaultImage(startSize, QImage::Format_ARGB32_Premultiplied);
    defaultImage.fill(FRAME_COLOR_DEFAULT);

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QVERIFY(defaultImage != redImage);

    // Insert two rows, then color the image red
    QImage redEditImage(QSize(2,5), QImage::Format_ARGB32_Premultiplied);
    redEditImage.fill(Qt::GlobalColor::red);

    model.insertRows(0,2);
    model.setData(model.index(0), redEditImage, PatternScrollModel::EditImage);

    // Insert a row in the middle, then verify the images are in the correct order.
    QVERIFY(model.insertRows(1,1) == true);

    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == redImage);
    QVERIFY(model.data(model.index(1), PatternScrollModel::FrameImage).value<QImage>() == defaultImage);
    QVERIFY(model.data(model.index(2), PatternScrollModel::FrameImage).value<QImage>() == redImage);

    QVERIFY(spy.count() == 4); // 1 for 'modified', 2 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::removeRowsNegativeIndexFails()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    model.insertRows(0,1);

    QVERIFY(model.rowCount() == 1);
    QVERIFY(model.removeRows(-1,1) == false);

    // TODO: Test that dataChanged() not signalled
    // TODO: Test that the undo stack did not grow
}

void PatternScrollModelTests::removeRowsTooHighIndexFails()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    model.insertRows(0,1);

    QVERIFY(model.rowCount() == 1);
    QVERIFY(model.removeRows(2,1) == false);

    // TODO: Test that dataChanged() not signalled
    // TODO: Test that the undo stack did not grow
}

void PatternScrollModelTests::removeRowsTooHighCountFails()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    model.insertRows(0,1);

    QVERIFY(model.rowCount() == 1);
    QVERIFY(model.removeRows(0,2) == false);

    // TODO: Test that dataChanged() not signalled
    // TODO: Test that the undo stack did not grow
}

void PatternScrollModelTests::removeRowsOneRow()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QVERIFY(model.rowCount() == 1);
    QVERIFY(model.removeRows(0,1) == true);
    QVERIFY(model.rowCount() == 0);

    QVERIFY(spy.count() == 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::removeRowsMultipleRows()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,2);

    QVERIFY(model.rowCount() == 2);
    QVERIFY(model.removeRows(0,2) == true);
    QVERIFY(model.rowCount() == 0);

    QVERIFY(spy.count() == 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::removeRowsAtFront()
{
    QSize startSize(1,5);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QImage defaultImage(startSize, QImage::Format_ARGB32_Premultiplied);
    defaultImage.fill(FRAME_COLOR_DEFAULT);

    QVERIFY(redImage != defaultImage);

    // Insert a row, color it red, then insert a second row which will be the default color.
    model.insertRows(0,1);
    model.setData(model.index(0), redImage, PatternScrollModel::EditImage);
    model.insertRows(1,1);

    // Remove the first one, then verify that the remaining row is the default color.
    QVERIFY(model.removeRows(0,1) == true);

    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == defaultImage);

    QVERIFY(spy.count() == 5); // 1 for 'modified', 2 for 'insertRows', 1 for 'setData', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::removeRowsAtBack()
{
    QSize startSize(1,5);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QImage defaultImage(startSize, QImage::Format_ARGB32_Premultiplied);
    defaultImage.fill(FRAME_COLOR_DEFAULT);

    QVERIFY(redImage != defaultImage);

    // Insert a row, color it red, then insert a second row which will be the default color.
    model.insertRows(0,1);
    model.setData(model.index(0), redImage, PatternScrollModel::EditImage);
    model.insertRows(1,1);

    // Remove the last one, then verify that the remaining row is red.
    QVERIFY(model.removeRows(1,1) == true);

    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == redImage);

    QVERIFY(spy.count() == 5); // 1 for 'modified', 2 for 'insertRows', 1 for 'setData', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::removeRowsInMiddle()
{
    QSize startSize(1,5);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QImage defaultImage(startSize, QImage::Format_ARGB32_Premultiplied);
    defaultImage.fill(FRAME_COLOR_DEFAULT);

    QVERIFY(redImage != defaultImage);

    // Insert a row, color it red, then insert rows before and after it which will be the default color.
    model.insertRows(0,1);
    model.setData(model.index(0), redImage, PatternScrollModel::EditImage);
    model.insertRows(0,1);
    model.insertRows(2,1);

    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == defaultImage);
    QVERIFY(model.data(model.index(1), PatternScrollModel::FrameImage).value<QImage>() == redImage);
    QVERIFY(model.data(model.index(2), PatternScrollModel::FrameImage).value<QImage>() == defaultImage);

    // Remove the middle one, then verify that the remaining rows are default color
    QVERIFY(model.removeRows(1,1) == true);

    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == defaultImage);
    QVERIFY(model.data(model.index(1), PatternScrollModel::FrameImage).value<QImage>() == defaultImage);

    QVERIFY(spy.count() == 6); // 1 for 'modified', 3 for 'insertRows', 1 for 'setData', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::dataInvalidIndexTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    QVERIFY(model.data(QModelIndex(), PatternScrollModel::FrameImage) == QVariant());
}

void PatternScrollModelTests::dataIndexOutOfRangeTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    model.insertRows(0,1);
    QModelIndex modelIndex = model.index(0);
    model.removeRows(0,1);
    QVERIFY(modelIndex.row() == 0);

    QVERIFY(model.data(modelIndex, PatternScrollModel::FrameImage) == QVariant());
}

void PatternScrollModelTests::setDataInvalidIndexTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    QVERIFY(model.setData(QModelIndex(), QImage(), PatternScrollModel::FrameImage) == false);
}

void PatternScrollModelTests::setDataIndexOutOfRangeTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);

    model.insertRows(0,1);
    QModelIndex modelIndex = model.index(0);
    model.removeRows(0,1);
    QVERIFY(modelIndex.row() == 0);

    QVERIFY(model.setData(modelIndex, QImage(), PatternScrollModel::FrameImage) == false);
}

void PatternScrollModelTests::setFrameImageFailsTest()
{
    QSize startSize(10,10);
    PatternScrollModel model(startSize);
    model.insertRows(0,1);

    QImage image = model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>();
    QVERIFY(model.setData(model.index(0), image, PatternScrollModel::FrameImage) == false);
}

void PatternScrollModelTests::canSetEditImageBadSizeTest()
{
    QSize startSize(10,10);
    PatternScrollModel model(startSize);
    model.insertRows(0,1);

    QImage image(QSize(2,10), QImage::Format_ARGB32_Premultiplied);

    QVERIFY(model.setData(model.index(0), image, PatternScrollModel::EditImage) == false);
}

void PatternScrollModelTests::canSetEditImageTest()
{
    QSize startSize(1,10);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QImage image(QSize(1,10), QImage::Format_ARGB32_Premultiplied);
    image.fill(FRAME_COLOR_DEFAULT);

    QVERIFY(model.data(modelIndex, PatternScrollModel::EditImage).value<QImage>() == image);

    image.fill(Qt::GlobalColor::red);

    QVERIFY(model.setData(modelIndex, image, PatternScrollModel::EditImage) == true);
    QVERIFY(model.data(modelIndex, PatternScrollModel::EditImage).value<QImage>() == image);

    QVERIFY(spy.count() == 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}


void PatternScrollModelTests::canSetFrameSizeTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QVERIFY(model.data(modelIndex, PatternScrollModel::FrameSize).toSize() == startSize);

    QSize newSize(3,4);

    QVERIFY(model.setData(modelIndex, newSize, PatternScrollModel::FrameSize) == true);
    QVERIFY(model.data(modelIndex, PatternScrollModel::FrameSize).toSize() == newSize);

    QVERIFY(model.data(modelIndex, PatternScrollModel::FrameImage).value<QImage>().size() == newSize);

    QVERIFY(spy.count() == 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::canSetFrameSpeedTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QVERIFY(model.data(modelIndex, PatternScrollModel::FrameSpeed).toFloat() == PATTERN_FRAME_SPEED_DEFAULT_VALUE);

    float frameSpeed = PATTERN_FRAME_SPEED_DEFAULT_VALUE+1;

    QVERIFY(model.setData(modelIndex, frameSpeed, PatternScrollModel::FrameSpeed) == true);
    QVERIFY(model.data(modelIndex, PatternScrollModel::FrameSpeed).toFloat() == frameSpeed);

    QVERIFY(spy.count() == 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::canSetFileNameTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QVERIFY(model.data(modelIndex, PatternScrollModel::FileName).toString() == "");

    QString fileName = "TestTest";

    QVERIFY(model.setData(modelIndex, fileName, PatternScrollModel::FileName) == true);
    QVERIFY(model.data(modelIndex, PatternScrollModel::FileName).toString() == fileName);

    QVERIFY(spy.count() == 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternScrollModelTests::canSetModifiedTest()
{
    QSize startSize(1,2);
    PatternScrollModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QVERIFY(model.data(modelIndex, PatternScrollModel::Modified).toBool() == true);

    QVERIFY(model.setData(modelIndex, false, PatternScrollModel::Modified) == true);
    QVERIFY(model.data(modelIndex, PatternScrollModel::Modified).toBool() == false);

    QVERIFY(spy.count() == 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

//void PatternScrollModelTests::readFromStreamTest()
//{
//    // Build some data for the read test
//    QSize frameSize(10,11);
//    QString fileName("filename");
//    float frameSpeed = 1.234;
//    QList<QImage> frames;

//    QImage redImage(frameSize, QImage::Format_ARGB32_Premultiplied);
//    redImage.fill(Qt::GlobalColor::red);

//    QImage greenImage(frameSize, QImage::Format_ARGB32_Premultiplied);
//    greenImage.fill(Qt::GlobalColor::green);

//    frames.append(redImage);
//    frames.append(greenImage);

//    // Pack the data into a stream
//    QBuffer buffer;
//    buffer.open(QBuffer::ReadWrite);

//    QDataStream stream;
//    stream.setDevice(&buffer);

//    stream << frameSize;
//    stream << fileName;
//    stream << frameSpeed;
//    stream << frames;
//    stream << QString("endOfStream");
//    buffer.reset();

//    // Read the data back to verify functionality.
//    PatternScrollModel model(frameSize*10);
//    stream >> model;

//    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameSize) == frameSize);
//    QVERIFY(model.data(model.index(0), PatternScrollModel::FileName).toString() == fileName);
//    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameSpeed).toFloat() == frameSpeed);
//    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == redImage);
//    QVERIFY(model.data(model.index(1), PatternScrollModel::FrameImage).value<QImage>() == greenImage);

//    QString endOfStreamMarker;
//    stream >> endOfStreamMarker;
//    QVERIFY(endOfStreamMarker == "endOfStream");
//}

//void PatternScrollModelTests::writeToStreamTest()
//{
//    // Build some data for the write test
//    QSize frameSize(10,11);
//    QString fileName("filename");
//    float frameSpeed = 1.234;

//    QImage redImage(frameSize, QImage::Format_ARGB32_Premultiplied);
//    redImage.fill(Qt::GlobalColor::red);

//    QImage greenImage(frameSize, QImage::Format_ARGB32_Premultiplied);
//    greenImage.fill(Qt::GlobalColor::green);

//    // Load it into the model
//    PatternScrollModel model(frameSize*10);
//    model.insertRows(0,2);
//    model.setData(model.index(0), frameSize, PatternScrollModel::FrameSize);
//    model.setData(model.index(0), fileName, PatternScrollModel::FileName);
//    model.setData(model.index(0), frameSpeed, PatternScrollModel::FrameSpeed);
//    model.setData(model.index(0), redImage, PatternScrollModel::FrameImage);
//    model.setData(model.index(1), redImage, PatternScrollModel::FrameImage);

//    // Pack the model into a stream
//    QBuffer buffer;
//    buffer.open(QBuffer::ReadWrite);

//    QDataStream stream;
//    stream.setDevice(&buffer);

//    stream << model;
//    stream << QString("endOfStream");
//    buffer.reset();

//    // Then read the data out from the stream
//    QSize readFrameSize;
//    QString readFileName;
//    float readFrameSpeed;
//    QList<QImage> readFrames;

//    stream >> readFrameSize;
//    stream >> readFileName;
//    stream >> readFrameSpeed;
//    stream >> readFrames;

//    for(QImage &frame : readFrames)
//        frame = frame.convertToFormat(QImage::Format_ARGB32_Premultiplied);

//    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameSize) == readFrameSize);
//    QVERIFY(model.data(model.index(0), PatternScrollModel::FileName).toString() == readFileName);
//    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameSpeed).toFloat() == readFrameSpeed);
//    QVERIFY(model.data(model.index(0), PatternScrollModel::FrameImage).value<QImage>() == readFrames.at(0));
//    QVERIFY(model.data(model.index(1), PatternScrollModel::FrameImage).value<QImage>() == readFrames.at(1));

//    QString endOfStreamMarker;
//    stream >> endOfStreamMarker;
//    QVERIFY(endOfStreamMarker == "endOfStream");
//}
