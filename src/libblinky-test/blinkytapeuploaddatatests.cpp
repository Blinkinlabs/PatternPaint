#include "blinkytapeuploaddatatests.h"
#include "blinkytapeuploaddata.h"

#include "firmwarestore.h"
#include "linearfixture.h"

#include <QTest>

void BlinkyTapeUploadDataTests::makePatternHeaderTableTest()
{
    QByteArray expectedResponse;
    expectedResponse.append('\x12');
    expectedResponse.append('\x56');
    expectedResponse.append('\x34');

    QCOMPARE(makePatternTableHeader(18,13398), expectedResponse);
}

void BlinkyTapeUploadDataTests::makePatternTableEntryTest()
{
    QByteArray expectedResponse;
    expectedResponse.append('\x00');
    expectedResponse.append('\x34');
    expectedResponse.append('\x12');
    expectedResponse.append('\x78');
    expectedResponse.append('\x56');
    expectedResponse.append('\xBC');
    expectedResponse.append('\x9A');
    expectedResponse.append('\xC2');
    expectedResponse.append('\xD3');

    QCOMPARE(makePatternTableEntry(PatternWriter::Encoding::RGB24, 4660, 22136, 39612, 54210), expectedResponse);
}

void BlinkyTapeUploadDataTests::makeBrightnessTest_data()
{
    QTest::addColumn<int>("maxBrightness");
    QTest::addColumn<bool>("fixedBrightness");
    QTest::addColumn<QByteArray>("expectedResponse");

    QTest::newRow("0") << 0 << false << QByteArray(8, (char)0);
    QTest::newRow("-1") << -1 << false << QByteArray(8, (char)0);

    QTest::newRow("1") << 1 << false << QByteArray()
                            .append((char)3)
                            .append((char)2)
                            .append((char)1)
                            .append((char)0)
                            .append((char)0)
                            .append((char)0)
                            .append((char)1)
                            .append((char)2);

    QTest::newRow("100") << 100 << false << QByteArray()
                            .append((char)255)
                            .append((char)191)
                            .append((char)114)
                            .append((char)41)
                            .append((char)20)
                            .append((char)41)
                            .append((char)114)
                            .append((char)191);

    QTest::newRow("101") << 101 << false << QByteArray()
                            .append((char)255)
                            .append((char)191)
                            .append((char)114)
                            .append((char)41)
                            .append((char)20)
                            .append((char)41)
                            .append((char)114)
                            .append((char)191);

    QTest::newRow("1") << 1 << true << QByteArray()
                            .append((char)3)
                            .append((char)3)
                            .append((char)3)
                            .append((char)3)
                            .append((char)3)
                            .append((char)3)
                            .append((char)3)
                            .append((char)3);

    QTest::newRow("1") << 100 << true << QByteArray()
                            .append((char)255)
                            .append((char)255)
                            .append((char)255)
                            .append((char)255)
                            .append((char)255)
                            .append((char)255)
                            .append((char)255)
                            .append((char)255);
}

void BlinkyTapeUploadDataTests::makeBrightnessTest()
{
    QFETCH(int, maxBrightness);
    QFETCH(bool, fixedBrightness);
    QFETCH(QByteArray, expectedResponse);

    QCOMPARE(makeBrightnessTable(maxBrightness, fixedBrightness), expectedResponse);
}


void BlinkyTapeUploadDataTests::badFirmwareNameTest()
{
    BlinkyTapeUploadData blinkyTapeUploadData;
    QList<PatternWriter> patternWriters;

    QCOMPARE(blinkyTapeUploadData.init("notagoodfirmwarename", patternWriters), false);
    QCOMPARE(blinkyTapeUploadData.errorString, QString("Firmware read failed!"));
}

void BlinkyTapeUploadDataTests::noPatternsFailsTest()
{
    BlinkyTapeUploadData blinkyTapeUploadData;
    QList<PatternWriter> patternWriters;

    QCOMPARE(blinkyTapeUploadData.init(BLINKYTAPE_DEFAULT_FIRMWARE_NAME, patternWriters), false);
    QCOMPARE(blinkyTapeUploadData.errorString, QString("No Patterns detected!"));
}

void BlinkyTapeUploadDataTests::maxPatternsSucceedsTest()
{
    BlinkyTapeUploadData blinkyTapeUploadData;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(0);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    // blinkytapeuploaddata.c 'MAX_PATTERN_COUNT'
    for(int i = 0; i < 50; i++)
        patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QCOMPARE(blinkyTapeUploadData.init(BLINKYTAPE_DEFAULT_FIRMWARE_NAME, patternWriters), true);
}

void BlinkyTapeUploadDataTests::tooManyPatternsFailsTest()
{
    BlinkyTapeUploadData blinkyTapeUploadData;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(0);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    // blinkytapeuploaddata.c 'MAX_PATTERN_COUNT'
    for(int i = 0; i < 51; i++)
        patternWriters.push_back(PatternWriter(pattern, fixture, encoding));

    QCOMPARE(blinkyTapeUploadData.init(BLINKYTAPE_DEFAULT_FIRMWARE_NAME, patternWriters), false);
    QCOMPARE(blinkyTapeUploadData.errorString, QString("Too many patterns, cannot fit in pattern table."));
}

void BlinkyTapeUploadDataTests::padsFirmwareSectionTest()
{
    MemorySection firmwareSection = FirmwareStore::getFirmwareData(BLINKYTAPE_DEFAULT_FIRMWARE_NAME);

    BlinkyTapeUploadData blinkyTapeUploadData;
    QList<PatternWriter> patternWriters;

    Pattern pattern(Pattern::PatternType::Scrolling, QSize(1,1), 1);
    LinearFixture fixture(0);
    PatternWriter::Encoding encoding = PatternWriter::Encoding::RGB24;

    patternWriters.push_back(PatternWriter(pattern, fixture, encoding));
    QCOMPARE(blinkyTapeUploadData.init(BLINKYTAPE_DEFAULT_FIRMWARE_NAME, patternWriters), true);
    QVERIFY(blinkyTapeUploadData.flashData.length() > 0);
    QCOMPARE(blinkyTapeUploadData.flashData.at(0).name, firmwareSection.name);

    // Note: This doesn't prove anything if the original firmware was already bounded to a 128 barrier.
    QCOMPARE((blinkyTapeUploadData.flashData.at(0).data.length() % 128), 0);
}

