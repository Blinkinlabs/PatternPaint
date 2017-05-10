#include "blinkypendantuploaddatatests.h"
#include "blinkypendantuploaddata.h"
#include "linearfixture.h"

#include <QTest>


void BlinkyPendantUploadDataTests::makePatternHeaderTableTest()
{
    uint8_t patternCount = 12;
    uint8_t displayMode = 34;

    QByteArray expectedResponse;
    expectedResponse.append('\x31');
    expectedResponse.append('\x23');
    expectedResponse.append((char)patternCount);
    expectedResponse.append((char)displayMode);

    QVERIFY(BlinkyPendantUploadData::makePatternTableHeader(patternCount,displayMode) == expectedResponse);
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

    QVERIFY(BlinkyPendantUploadData::makePatternTableEntry(305419896, 39612, 4660) == expectedResponse);
}

void BlinkyPendantUploadDataTests::noPatternsFailsTest()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    QList<PatternWriter> patternWriters;

    QVERIFY(blinkyPendantUploadData.init(patternWriters) == false);
    QVERIFY(blinkyPendantUploadData.errorString == "No Patterns detected!");
}

void BlinkyPendantUploadDataTests::maxPatternsSucceedsTest()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(10);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    for(int i = 0; i < 255; i++)
        patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QVERIFY(blinkyPendantUploadData.init(patternWriters) == true);
}

void BlinkyPendantUploadDataTests::tooManyPatternsFailsTest()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(10);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    for(int i = 0; i < 256; i++)
        patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QVERIFY(blinkyPendantUploadData.init(patternWriters) == false);
    QVERIFY(blinkyPendantUploadData.errorString == "Too many patterns, cannot fit in pattern table.");
}


void BlinkyPendantUploadDataTests::wrongEncodingFails()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(10);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB565_RLE;

    patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QVERIFY(blinkyPendantUploadData.init(patternWriters) == false);
    QVERIFY(blinkyPendantUploadData.errorString == "Wrong encoding type- must be RGB24!");
}

void BlinkyPendantUploadDataTests::wrongLEDCountFails()
{
    BlinkyPendantUploadData blinkyPendantUploadData;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(9);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QVERIFY(blinkyPendantUploadData.init(patternWriters) == false);
    QVERIFY(blinkyPendantUploadData.errorString == "Wrong pattern size- must be 10 pixels high!");
}
