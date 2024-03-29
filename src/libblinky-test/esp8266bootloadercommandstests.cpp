#include "esp8266bootloadercommandstests.h"

#include <QtTest>
#include <QByteArray>
#include <QDebug>

#include "esp8266bootloadercommands.h"
#include "bytearrayhelpers.h"

void Esp8266BootloaderCommandsTests::calculateChecksumTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<unsigned char>("expectedChecksum");

    QTest::newRow("empty data") << QByteArray() << (unsigned char)0xEF;
    QTest::newRow("lots of zeros") << QByteArray(100, (char)0) << (unsigned char)0xEF;
    QTest::newRow("even # of 0xFF") << QByteArray(100, (char)0xFF) << (unsigned char)0xEF;
    QTest::newRow("odd # of 0xFF") << QByteArray(101, (char)0xFF) << (unsigned char)0x10;
    QTest::newRow("1 0x10") << QByteArray(1, (char)0x10) << (unsigned char)0xFF;
}

void Esp8266BootloaderCommandsTests::calculateChecksumTest()
{
    QFETCH(QByteArray, data);
    QFETCH(unsigned char, expectedChecksum);

    QCOMPARE(Esp8266BootloaderCommands::calculateChecksum(data), expectedChecksum);
}

void Esp8266BootloaderCommandsTests::buildCommandTest_data()
{
    QTest::addColumn<Esp8266BootloaderCommands::Opcode>("opcode");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<unsigned char>("checksum");
    QTest::addColumn<QByteArray>("expectedCommand");

    QTest::newRow("empty data")
            << Esp8266BootloaderCommands::Opcode_FlashDownloadStart
            << QByteArray()
            << (unsigned char) 0x12
            << QByteArray().append((char)0x00)
               .append(Esp8266BootloaderCommands::Opcode_FlashDownloadStart)
               .append(ByteArrayHelpers::uint16ToByteArrayLittle(0))
               .append(ByteArrayHelpers::uint32ToByteArrayLittle(0x12));

    QTest::newRow("length 100")
            << Esp8266BootloaderCommands::Opcode_SyncFrame
            << QByteArray(100, (char)0xFF)
            << (unsigned char) 0x34
            << QByteArray().append((char)0x00)
               .append(Esp8266BootloaderCommands::Opcode_SyncFrame)
               .append(ByteArrayHelpers::uint16ToByteArrayLittle(100))
               .append(ByteArrayHelpers::uint32ToByteArrayLittle(0x34))
               .append(QByteArray(100, (char)0xFF));
}

void Esp8266BootloaderCommandsTests::buildCommandTest()
{
    QFETCH(Esp8266BootloaderCommands::Opcode, opcode);
    QFETCH(QByteArray, data);
    QFETCH(unsigned char, checksum);
    QFETCH(QByteArray, expectedCommand);

    QCOMPARE(Esp8266BootloaderCommands::buildCommand(opcode, data, checksum), expectedCommand);
}

void Esp8266BootloaderCommandsTests::slipEncodeTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("expectedEncoding");

    QTest::newRow("empty data")
            << QByteArray()
            << QByteArray().append((char)0xC0).append((char)0xC0);

    QTest::newRow("10 0xFF")
            << QByteArray(10, (char)0xFF)
            << QByteArray().append((char)0xC0).append(QByteArray(10, (char)0xFF)).append((char)0xC0);

    QTest::newRow("1 0xC0")
            << QByteArray().append((char)0xC0)
            << QByteArray().append((char)0xC0).append((char)0xDB).append((char)0xDC).append((char)0xC0);

    QTest::newRow("1 0xDB")
            << QByteArray().append((char)0xDB)
            << QByteArray().append((char)0xC0).append((char)0xDB).append((char)0xDD).append((char)0xC0);

    QTest::newRow("1 0xC0 0xDB")
            << QByteArray().append((char)0xC0).append((char)0xDB)
            << QByteArray().append((char)0xC0).append((char)0xDB).append((char)0xDC).append((char)0xDB).append((char)0xDD).append((char)0xC0);

    QTest::newRow("1 0xDB 0xC0")
            << QByteArray().append((char)0xDB).append((char)0xC0)
            << QByteArray().append((char)0xC0).append((char)0xDB).append((char)0xDD).append((char)0xDB).append((char)0xDC).append((char)0xC0);
}

void Esp8266BootloaderCommandsTests::slipEncodeTest()
{
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, expectedEncoding);

    QCOMPARE(Esp8266BootloaderCommands::slipEncode(data), expectedEncoding);
}

void Esp8266BootloaderCommandsTests::slipDecodeTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("expectedDecoding");

    QTest::newRow("bad packet: empty")
            << QByteArray()
            << QByteArray();

    QTest::newRow("bad packet: too small")
            << QByteArray().append((char)0xFF)
            << QByteArray();

    QTest::newRow("bad packet: bad start")
            << QByteArray().append((char)0xFF).append((char)0xFF).append((char)0xC0)
            << QByteArray();

    QTest::newRow("bad packet: bad end")
            << QByteArray().append((char)0xC0).append((char)0xFF).append((char)0xFF)
            << QByteArray();

    QTest::newRow("empty data")
            << QByteArray().append((char)0xC0).append((char)0xC0)
            << QByteArray();

    QTest::newRow("10 0xFF")
            << QByteArray().append((char)0xC0).append(QByteArray(10, (char)0xFF)).append((char)0xC0)
            << QByteArray(10, (char)0xFF);

    QTest::newRow("1 0xC0")
            << QByteArray().append((char)0xC0).append((char)0xDB).append((char)0xDC).append((char)0xC0)
            << QByteArray().append((char)0xC0);

    QTest::newRow("1 0xDB")
            << QByteArray().append((char)0xC0).append((char)0xDB).append((char)0xDD).append((char)0xC0)
            << QByteArray().append((char)0xDB);

    QTest::newRow("1 0xC0 0xDB")
            << QByteArray().append((char)0xC0).append((char)0xDB).append((char)0xDC).append((char)0xDB).append((char)0xDD).append((char)0xC0)
            << QByteArray().append((char)0xC0).append((char)0xDB);

    QTest::newRow("1 0xDB 0xC0")
            << QByteArray().append((char)0xC0).append((char)0xDB).append((char)0xDD).append((char)0xDB).append((char)0xDC).append((char)0xC0)
            << QByteArray().append((char)0xDB).append((char)0xC0);
}

void Esp8266BootloaderCommandsTests::slipDecodeTest()
{
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, expectedDecoding);

    QCOMPARE(Esp8266BootloaderCommands::slipDecode(data), expectedDecoding);
}

void Esp8266BootloaderCommandsTests::slipEncodeDecodeTest()
{
    QByteArray message;

    for(int index = 0; index < 9999; index++) {
        message.append((char)(index % 0xFF));
    }

    QCOMPARE(Esp8266BootloaderCommands::slipDecode(Esp8266BootloaderCommands::slipEncode(message)), message);
}
