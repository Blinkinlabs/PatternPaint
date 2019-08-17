#include <QBuffer>
#include <QSignalSpy>
#include <QTest>

#include "patternframemodeltests.h"

#include "patternframemodel.h"

void PatternFrameModelTests::emptyAtConstructionTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QCOMPARE(model.rowCount(), 0);
}

void PatternFrameModelTests::flagsInvalidIndexTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QCOMPARE(model.flags(QModelIndex()), (Qt::ItemIsEnabled | Qt::ItemIsDropEnabled));
}

void PatternFrameModelTests::flagsValidIndexTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QCOMPARE(model.flags(model.index(0)), (Qt::ItemIsEnabled
                                           | Qt::ItemIsSelectable
                                           | Qt::ItemIsEditable
                                           | Qt::ItemIsDragEnabled));
}

void PatternFrameModelTests::supportedDropActionsTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QCOMPARE(model.supportedDropActions(), (Qt::CopyAction
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

void PatternFrameModelTests::insertRowsNegativeIndexFails()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QCOMPARE(model.rowCount(), 0);
    QVERIFY(model.insertRows(-1,1) == false);
}

void PatternFrameModelTests::insertRowsTooHighIndexFails()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QCOMPARE(model.rowCount(), 0);
    QVERIFY(model.insertRows(1,1) == false);
}

void PatternFrameModelTests::insertRowsOneRow()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QCOMPARE(model.rowCount(), 0);
    QVERIFY(model.insertRows(0,1) == true);
    QCOMPARE(model.rowCount(), 1);

    QCOMPARE(spy.count(), 2); // 1 for 'modified', 1 for 'insert rows'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::insertRowsMultipleRows()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QCOMPARE(model.rowCount(), 0);
    QVERIFY(model.insertRows(0,3) == true);
    QCOMPARE(model.rowCount(), 3);

    QCOMPARE(spy.count(), 2); // 1 for 'modified', 1 for 'insertRows'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::insertRowsAtFront()
{
    QSize startSize(5,5);
    PatternFrameModel model(startSize);
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
    model.setData(model.index(0), redImage, PatternFrameModel::EditImage);

    // Insert a row at the front, then verify the images are in the correct order.
    QVERIFY(model.insertRows(0,1) == true);

    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>(), defaultImage);
    QCOMPARE(model.data(model.index(1), PatternFrameModel::FrameImage).value<QImage>(), redImage);

    QCOMPARE(spy.count(), 4); // 1 for 'modified', 2 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::insertRowsAtBack()
{
    QSize startSize(5,5);
    PatternFrameModel model(startSize);
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
    model.setData(model.index(0), redImage, PatternFrameModel::EditImage);

    // Insert a row at the front, then verify the images are in the correct order.
    QVERIFY(model.insertRows(1,1) == true);

    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>(), redImage);
    QCOMPARE(model.data(model.index(1), PatternFrameModel::FrameImage).value<QImage>(), defaultImage);

    QCOMPARE(spy.count(), 4); // 1 for 'modified', 2 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::insertRowsInMiddle()
{
    QSize startSize(5,5);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage defaultImage(startSize, QImage::Format_ARGB32_Premultiplied);
    defaultImage.fill(FRAME_COLOR_DEFAULT);

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QVERIFY(defaultImage != redImage);

    // Insert two rows, then color them red
    model.insertRows(0,2);
    model.setData(model.index(0), redImage, PatternFrameModel::EditImage);
    model.setData(model.index(1), redImage, PatternFrameModel::EditImage);

    // Insert a row in the middle, then verify the images are in the correct order.
    QVERIFY(model.insertRows(1,1) == true);

    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>(), redImage);
    QCOMPARE(model.data(model.index(1), PatternFrameModel::FrameImage).value<QImage>(), defaultImage);
    QCOMPARE(model.data(model.index(2), PatternFrameModel::FrameImage).value<QImage>(), redImage);

    QCOMPARE(spy.count(), 5); // 1 for 'modified', 2 for 'insertRows', 2 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::removeRowsNegativeIndexFails()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QCOMPARE(model.rowCount(), 1);
    QVERIFY(model.removeRows(-1,1) == false);

    // TODO: Test that dataChanged() not signalled
    // TODO: Test that the undo stack did not grow
}

void PatternFrameModelTests::removeRowsTooHighIndexFails()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QCOMPARE(model.rowCount(), 1);
    QVERIFY(model.removeRows(2,1) == false);

    // TODO: Test that dataChanged() not signalled
    // TODO: Test that the undo stack did not grow
}

void PatternFrameModelTests::removeRowsTooHighCountFails()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QCOMPARE(model.rowCount(), 1);
    QVERIFY(model.removeRows(0,2) == false);

    // TODO: Test that dataChanged() not signalled
    // TODO: Test that the undo stack did not grow
}

void PatternFrameModelTests::removeRowsOneRow()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QCOMPARE(model.rowCount(), 1);
    QVERIFY(model.removeRows(0,1) == true);
    QCOMPARE(model.rowCount(), 0);

    QCOMPARE(spy.count(), 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::removeRowsMultipleRows()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,2);

    QCOMPARE(model.rowCount(), 2);
    QVERIFY(model.removeRows(0,2) == true);
    QCOMPARE(model.rowCount(), 0);

    QCOMPARE(spy.count(), 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::removeRowsAtFront()
{
    QSize startSize(5,5);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QImage greenImage(startSize, QImage::Format_ARGB32_Premultiplied);
    greenImage.fill(Qt::GlobalColor::green);

    QVERIFY(redImage != greenImage);

    // Insert two rows, color the first one red and the second one green
    model.insertRows(0,2);
    model.setData(model.index(0), redImage, PatternFrameModel::EditImage);
    model.setData(model.index(1), greenImage, PatternFrameModel::EditImage);

    // Remove the first one, then verify that the remaining row is green.
    QVERIFY(model.removeRows(0,1) == true);

    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>(), greenImage);

    QCOMPARE(spy.count(), 5); // 1 for 'modified', 1 for 'insertRows', 2 for 'setData', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::removeRowsAtBack()
{
    QSize startSize(5,5);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QImage greenImage(startSize, QImage::Format_ARGB32_Premultiplied);
    greenImage.fill(Qt::GlobalColor::green);

    QVERIFY(redImage != greenImage);

    // Insert two rows, color the first one red and the second one green
    model.insertRows(0,2);
    model.setData(model.index(0), redImage, PatternFrameModel::EditImage);
    model.setData(model.index(1), greenImage, PatternFrameModel::EditImage);

    // Remove the second one, then verify that the remaining row is red.
    QVERIFY(model.removeRows(1,1) == true);

    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>(), redImage);

    QCOMPARE(spy.count(), 5); // 1 for 'modified', 1 for 'insertRows', 2 for 'setData', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::removeRowsInMiddle()
{
    QSize startSize(5,5);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    QImage redImage(startSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QImage greenImage(startSize, QImage::Format_ARGB32_Premultiplied);
    greenImage.fill(Qt::GlobalColor::green);

    QVERIFY(redImage != greenImage);

    // Insert three rows, color them red, green,red
    model.insertRows(0,3);
    model.setData(model.index(0), redImage, PatternFrameModel::EditImage);
    model.setData(model.index(1), greenImage, PatternFrameModel::EditImage);
    model.setData(model.index(2), redImage, PatternFrameModel::EditImage);

    // Remove the second one, then verify that the remaining rows are red.
    QVERIFY(model.removeRows(1,1) == true);

    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>(), redImage);
    QCOMPARE(model.data(model.index(1), PatternFrameModel::FrameImage).value<QImage>(), redImage);

    QCOMPARE(spy.count(), 6); // 1 for 'modified', 1 for 'insertRows', 3 for 'setData', 1 for 'removeRows'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::dataInvalidIndexTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    QCOMPARE(model.data(QModelIndex(), PatternFrameModel::FrameImage), QVariant());
}

void PatternFrameModelTests::dataIndexOutOfRangeTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);

    model.insertRows(0,1);
    QModelIndex modelIndex = model.index(0);
    model.removeRows(0,1);
    QCOMPARE(modelIndex.row(), 0);

    QCOMPARE(model.data(modelIndex, PatternFrameModel::FrameImage), QVariant());
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
    QModelIndex modelIndex = model.index(0);
    model.removeRows(0,1);
    QCOMPARE(modelIndex.row(), 0);

    QVERIFY(model.setData(modelIndex, QImage(), PatternFrameModel::FrameImage) == false);
}

void PatternFrameModelTests::setFrameImageFailsTest()
{
    QSize startSize(10,10);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QImage image = model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>();
    QVERIFY(model.setData(model.index(0), image, PatternFrameModel::FrameImage) == false);
}

void PatternFrameModelTests::canSetEditImageBadSizeTest()
{
    QSize startSize(10,10);
    PatternFrameModel model(startSize);
    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QImage image(startSize*2, QImage::Format_ARGB32_Premultiplied);

    QVERIFY(model.setData(modelIndex, image, PatternFrameModel::EditImage) == false);
}

void PatternFrameModelTests::canSetEditImageTest()
{
    QSize startSize(10,10);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QImage image(startSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(FRAME_COLOR_DEFAULT);

    QCOMPARE(model.data(modelIndex, PatternFrameModel::EditImage).value<QImage>(), image);

    image.fill(Qt::GlobalColor::red);

    QVERIFY(model.setData(modelIndex, image, PatternFrameModel::EditImage) == true);
    QCOMPARE(model.data(modelIndex, PatternFrameModel::EditImage).value<QImage>(), image);

    QCOMPARE(spy.count(), 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}


void PatternFrameModelTests::canSetFrameSizeTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QCOMPARE(model.data(modelIndex, PatternFrameModel::FrameSize).toSize(), startSize);

    QSize newSize(3,4);

    QVERIFY(model.setData(modelIndex, newSize, PatternFrameModel::FrameSize) == true);
    QCOMPARE(model.data(modelIndex, PatternFrameModel::FrameSize).toSize(), newSize);

    QCOMPARE(model.data(modelIndex, PatternFrameModel::FrameImage).value<QImage>().size(), newSize);

    QCOMPARE(spy.count(), 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::canSetFrameSpeedTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QCOMPARE(model.data(modelIndex, PatternFrameModel::FrameSpeed).toFloat(), PATTERN_FRAME_SPEED_DEFAULT_VALUE);

    float frameSpeed = PATTERN_FRAME_SPEED_DEFAULT_VALUE+1;

    QVERIFY(model.setData(modelIndex, frameSpeed, PatternFrameModel::FrameSpeed) == true);
    QCOMPARE(model.data(modelIndex, PatternFrameModel::FrameSpeed).toFloat(), frameSpeed);

    QCOMPARE(spy.count(), 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::canSetFileNameTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QCOMPARE(model.data(modelIndex, PatternFrameModel::FileName).toString(), QString(""));

    QString fileName = "TestTest";

    QVERIFY(model.setData(modelIndex, fileName, PatternFrameModel::FileName) == true);
    QCOMPARE(model.data(modelIndex, PatternFrameModel::FileName).toString(), fileName);

    QCOMPARE(spy.count(), 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::canSetModifiedTest()
{
    QSize startSize(1,2);
    PatternFrameModel model(startSize);
    QSignalSpy spy(&model, SIGNAL(dataChanged(const QModelIndex &,
                                              const QModelIndex &,
                                              const QVector<int> &)));

    model.insertRows(0,1);

    QModelIndex modelIndex = model.index(0);

    QVERIFY(model.data(modelIndex, PatternFrameModel::Modified).toBool() == true);

    QVERIFY(model.setData(modelIndex, false, PatternFrameModel::Modified) == true);
    QVERIFY(model.data(modelIndex, PatternFrameModel::Modified).toBool() == false);

    QCOMPARE(spy.count(), 3); // 1 for 'modified', 1 for 'insertRows', 1 for 'setData'
    // TODO: Verify the spy messages are correct?
}

void PatternFrameModelTests::readFromStreamTest()
{
    // Build some data for the read test
    QSize frameSize(10,11);
    QString fileName("filename");
    float frameSpeed = 1.234f;
    QList<QImage> frames;

    QImage redImage(frameSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QImage greenImage(frameSize, QImage::Format_ARGB32_Premultiplied);
    greenImage.fill(Qt::GlobalColor::green);

    frames.append(redImage);
    frames.append(greenImage);

    // Pack the data into a stream
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    QDataStream stream;
    stream.setDevice(&buffer);

    stream << static_cast<qint32>(1);
    stream << frameSize;
    stream << fileName;
    stream << frameSpeed;
    stream << frames;
    stream << QString("endOfStream");
    buffer.reset();

    // Read the data back to verify functionality.
    PatternFrameModel model(frameSize*10);
    stream >> model;

    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameSize).toSize(), frameSize);
    QCOMPARE(model.data(model.index(0), PatternFrameModel::FileName).toString(), fileName);
    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameSpeed).toFloat(), frameSpeed);
    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>(), redImage);
    QCOMPARE(model.data(model.index(1), PatternFrameModel::FrameImage).value<QImage>(), greenImage);

    QString endOfStreamMarker;
    stream >> endOfStreamMarker;
    QCOMPARE(endOfStreamMarker, QString("endOfStream"));
}

void PatternFrameModelTests::writeToStreamTest()
{
    // Build some data for the write test
    QSize frameSize(10,11);
    QString fileName("filename");
    float frameSpeed = 1.234f;

    QImage redImage(frameSize, QImage::Format_ARGB32_Premultiplied);
    redImage.fill(Qt::GlobalColor::red);

    QImage greenImage(frameSize, QImage::Format_ARGB32_Premultiplied);
    greenImage.fill(Qt::GlobalColor::green);

    // Load it into the model
    PatternFrameModel model(frameSize*10);
    model.insertRows(0,2);
    model.setData(model.index(0), frameSize, PatternFrameModel::FrameSize);
    model.setData(model.index(0), fileName, PatternFrameModel::FileName);
    model.setData(model.index(0), frameSpeed, PatternFrameModel::FrameSpeed);
    model.setData(model.index(0), redImage, PatternFrameModel::FrameImage);
    model.setData(model.index(1), redImage, PatternFrameModel::FrameImage);

    // Pack the model into a stream
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    QDataStream stream;
    stream.setDevice(&buffer);

    stream << model;
    stream << QString("endOfStream");
    buffer.reset();

    // Then read the data out from the stream
    qint32 readVersion;
    QSize readFrameSize;
    QString readFileName;
    float readFrameSpeed;
    QList<QImage> readFrames;

    stream >> readVersion;
    stream >> readFrameSize;
    stream >> readFileName;
    stream >> readFrameSpeed;
    stream >> readFrames;

    for(QImage &frame : readFrames)
        frame = frame.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    QCOMPARE(readVersion, 1);
    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameSize).toSize(), readFrameSize);
    QCOMPARE(model.data(model.index(0), PatternFrameModel::FileName).toString(), readFileName);
    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameSpeed).toFloat(), readFrameSpeed);
    QCOMPARE(model.data(model.index(0), PatternFrameModel::FrameImage).value<QImage>(), readFrames.at(0));
    QCOMPARE(model.data(model.index(1), PatternFrameModel::FrameImage).value<QImage>(), readFrames.at(1));

    QString endOfStreamMarker;
    stream >> endOfStreamMarker;
    QCOMPARE(endOfStreamMarker, QString("endOfStream"));
}
