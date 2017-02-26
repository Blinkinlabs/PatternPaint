#include "firmwarereadertests.h"
#include "firmwarereader.h"

#include <QTest>


void FirmwareReaderTests::parseHexLineBadFormatTest_data()
{
    QTest::addColumn<QString>("line");

    QTest::newRow("Empty line") << QString();
    QTest::newRow("Short line") << QString(10, 'x');
    QTest::newRow("bad colon")        << QString("-00000001FF");
    QTest::newRow("invalid length field") << QString(":zz000001FF");
    QTest::newRow("undersized length") << QString(":0000000001FF");
    QTest::newRow("oversized length") << QString(":01000001FF");
    QTest::newRow("invalid address field") << QString(":00xxxx01FF");
    QTest::newRow("invalid type field") << QString(":000000xxFF");
    QTest::newRow("invalid checksum field") << QString(":00000001xx");
    QTest::newRow("bad checksum") << QString(":00000001FE");
}

void FirmwareReaderTests::parseHexLineBadFormatTest()
{
    QFETCH(QString, line);

    unsigned int address;
    unsigned int type;
    QByteArray data;

    QVERIFY(parseHexLine(line, address, type, data) == false);
}

void FirmwareReaderTests::parseHexLineAddressTest_data()
{
    QTest::addColumn<QString>("line");
    QTest::addColumn<unsigned int>("expectedAddress");

    QTest::newRow("zero") << QString(":0000000000") << 0u;
    QTest::newRow("one") << QString(":00000100FF") << 1u;
    QTest::newRow("max") << QString(":00FFFF0002") << 65535u;
}

void FirmwareReaderTests::parseHexLineAddressTest()
{
    QFETCH(QString, line);
    QFETCH(unsigned int, expectedAddress);

    unsigned int address;
    unsigned int type;
    QByteArray data;

    QVERIFY(parseHexLine(line, address, type, data));
    QVERIFY(address == expectedAddress);
}

void FirmwareReaderTests::parseHexLineTypeTest_data()
{
    QTest::addColumn<QString>("line");
    QTest::addColumn<unsigned int>("expectedType");

    QTest::newRow("0x00") << QString(":0000000000") << 0u;
    QTest::newRow("0x01") << QString(":00000001FF") << 1u;
    QTest::newRow("0xFF") << QString(":000000FF01") << 255u;
}

void FirmwareReaderTests::parseHexLineTypeTest()
{
    QFETCH(QString, line);
    QFETCH(unsigned int, expectedType);

    unsigned int address;
    unsigned int type;
    QByteArray data;

    QVERIFY(parseHexLine(line, address, type, data));
    QVERIFY(type == expectedType);
}

void FirmwareReaderTests::parseHexLineDataTest_data()
{
    QTest::addColumn<QString>("line");
    QTest::addColumn<QByteArray>("expectedData");

    QTest::newRow("empty") << QString(":0000000000") << QByteArray();
    QTest::newRow("one byte, 0x00") << QString(":0100000000FF") << QByteArray(1, static_cast<unsigned char>(0x00));

    QString testHexString(":FF000000");
    QByteArray testByteArray;
    for(unsigned int i = 0; i < 255; i++) {
        testByteArray.append(static_cast<unsigned char>(i));
        testHexString.append(QString("%1").arg(i, 2, 16, QChar('0')));
    }
    testHexString.append("80");

    QTest::newRow("Max") << testHexString << testByteArray;
}

void FirmwareReaderTests::parseHexLineDataTest()
{
    QFETCH(QString, line);
    QFETCH(QByteArray, expectedData);

    unsigned int address;
    unsigned int type;
    QByteArray data;

    QVERIFY(parseHexLine(line, address, type, data));
    QVERIFY(data == expectedData);
}
