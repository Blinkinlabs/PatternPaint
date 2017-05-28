#include "blinkypendantuploaddatatests.h"
#include "blinkypendantuploaddata.h"
#include "linearfixture.h"
#include "exponentialbrightness.h"

#include <QTest>
#include <QList>

void BlinkyPendantUploadDataTests::makePatternHeaderTableTest()
{
    uint8_t patternCount = 12;
    BlinkyPendantUploadData::DisplayMode displayMode = BlinkyPendantUploadData::DisplayMode::POV;

    QByteArray expectedResponse;
    expectedResponse.append('\x31');
    expectedResponse.append('\x23');
    expectedResponse.append((char)patternCount);
    expectedResponse.append((char)displayMode);

    QCOMPARE(BlinkyPendantUploadData::makePatternTableHeader(patternCount,displayMode), expectedResponse);
}

void BlinkyPendantUploadDataTests::makePatternTableEntryTest()
{
    QByteArray expectedResponse;
    expectedResponse.append('\x00');
    expectedResponse.append('\x12');
    expectedResponse.append('\x34');
    expectedResponse.append('\x56');
    expectedResponse.append('\x78');
    expectedResponse.append('\x9A');
    expectedResponse.append('\xBC');
    expectedResponse.append('\x12');
    expectedResponse.append('\x34');

    QCOMPARE(BlinkyPendantUploadData::makePatternTableEntry(305419896, 39612, 4660), expectedResponse);
}

void BlinkyPendantUploadDataTests::noPatternsFailsTest()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    BlinkyPendantUploadData::DisplayMode displayMode = BlinkyPendantUploadData::DisplayMode::POV;
    QList<PatternWriter> patternWriters;

    QCOMPARE(blinkyPendantUploadData.init(displayMode, patternWriters), false);
    QCOMPARE(blinkyPendantUploadData.errorString, QString("No Patterns detected!"));
}

void BlinkyPendantUploadDataTests::maxPatternsSucceedsTest()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    BlinkyPendantUploadData::DisplayMode displayMode = BlinkyPendantUploadData::DisplayMode::POV;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(10);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    for(int i = 0; i < 255; i++)
        patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QCOMPARE(blinkyPendantUploadData.init(displayMode, patternWriters), true);
}

void BlinkyPendantUploadDataTests::tooManyPatternsFailsTest()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    BlinkyPendantUploadData::DisplayMode displayMode = BlinkyPendantUploadData::DisplayMode::POV;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(10);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    for(int i = 0; i < 256; i++)
        patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QCOMPARE(blinkyPendantUploadData.init(displayMode, patternWriters), false);
    QCOMPARE(blinkyPendantUploadData.errorString, QString("Too many patterns, cannot fit in pattern table."));
}


void BlinkyPendantUploadDataTests::wrongEncodingFails()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    BlinkyPendantUploadData::DisplayMode displayMode = BlinkyPendantUploadData::DisplayMode::POV;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(10);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB565_RLE;

    patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QCOMPARE(blinkyPendantUploadData.init(displayMode, patternWriters), false);
    QCOMPARE(blinkyPendantUploadData.errorString, QString("Wrong encoding type- must be RGB24!"));
}

void BlinkyPendantUploadDataTests::wrongLEDCountFails()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    BlinkyPendantUploadData::DisplayMode displayMode = BlinkyPendantUploadData::DisplayMode::POV;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(9);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QCOMPARE(blinkyPendantUploadData.init(displayMode, patternWriters), false);
    QCOMPARE(blinkyPendantUploadData.errorString, QString("Wrong pattern size- must be 10 pixels high!"));
}

void BlinkyPendantUploadDataTests::uploadDataTest() {
    LinearFixture fixture(10);
    fixture.setBrightnessModel(new ExponentialBrightness(1,1,1));
    fixture.setColorMode(ColorMode::RGB);

    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    Pattern patternA(Pattern::PatternType::Scrolling, QSize(1,10), 1);
    patternA.setFrameSpeed(12);

    Pattern patternB(Pattern::PatternType::Scrolling, QSize(20,10), 1);
    patternB.setFrameSpeed(34);

    QList<PatternWriter> patternWriters;
    PatternWriter patternWriterA(patternA, fixture, encoding);
    PatternWriter patternWriterB(patternB, fixture, encoding);

    patternWriters.push_back(patternWriterA);
    patternWriters.push_back(patternWriterB);

    BlinkyPendantUploadData blinkyPendantUploadData;
    BlinkyPendantUploadData::DisplayMode displayMode = BlinkyPendantUploadData::DisplayMode::POV;

    QByteArray expectedData;
    expectedData.append(BlinkyPendantUploadData::makePatternTableHeader(2, displayMode));
    expectedData.append(BlinkyPendantUploadData::makePatternTableEntry(0,
                                                                       patternWriterA.getFrameCount(),
                                                                       patternWriterA.getFrameDelay()));

    expectedData.append(BlinkyPendantUploadData::makePatternTableEntry(patternWriterA.getDataAsBinary().length(),
                                                                       patternWriterB.getFrameCount(),
                                                                       patternWriterB.getFrameDelay()));
    expectedData.append(patternWriterA.getDataAsBinary());
    expectedData.append(patternWriterB.getDataAsBinary());

    QCOMPARE(blinkyPendantUploadData.init(displayMode, patternWriters), true);
    QCOMPARE(blinkyPendantUploadData.data, expectedData);
}
