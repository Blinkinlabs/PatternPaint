#include "lightbuddycommandstests.h"

#include <QtTest>

#include "lightbuddycommands.h"
#include "bytearrayhelpers.h"

#define COMMAND_RELOAD_ANIMATIONS 0x02
#define COMMAND_NEW_FILE    0x18
#define COMMAND_WRITE_PAGE  0x19
#define COMMAND_ERASE_FLASH 0x20

#define PAGE_SIZE_BYTES 256

void LightBuddyCommandsTests::commandHeaderTest()
{
    QByteArray expectedResponse(12, (char)0xFF);

    QCOMPARE(LightBuddyCommands::commandHeader(), expectedResponse);
}

void LightBuddyCommandsTests::eraseFlashTest()
{
    QByteArray expectedData;
    expectedData.append(LightBuddyCommands::commandHeader());
    expectedData.append((char)COMMAND_ERASE_FLASH);
    expectedData.append('E');
    expectedData.append('e');

    QByteArray expectedResponse;
    expectedResponse.append('P');
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0x00);
    expectedResponseMask.append((char)0x00);

    SerialCommand command = LightBuddyCommands::eraseFlash();

    QCOMPARE(command.name, QString("eraseFlash"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
    QCOMPARE(command.timeout, 20000);
}

void LightBuddyCommandsTests::fileNewTest_data()
{
    QTest::addColumn<uint32_t>("size");

    QTest::newRow("0") << 0u;
    QTest::newRow("1") << 1u;
    QTest::newRow("max uint32_t") << UINT32_MAX;
}

void LightBuddyCommandsTests::fileNewTest()
{
    QFETCH(uint32_t, size);

    QByteArray expectedData;
    expectedData.append(LightBuddyCommands::commandHeader());
    expectedData.append((char)COMMAND_NEW_FILE);
    expectedData.append((char)0x12);
    expectedData.append(ByteArrayHelpers::uint32ToByteArrayBig(size));

    QByteArray expectedResponse;
    expectedResponse.append('P');
    expectedResponse.append((char)0x03);
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0x00);
    expectedResponseMask.append((char)0x00);
    expectedResponseMask.append((char)0x00);
    expectedResponseMask.append((char)0x00);

    SerialCommand command = LightBuddyCommands::fileNew(size);

    QCOMPARE(command.name, QString("fileNew"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
    QCOMPARE(command.timeout, 5000);
}

void LightBuddyCommandsTests::writePageTest_data()
{
    QTest::addColumn<uint32_t>("sector");
    QTest::addColumn<uint32_t>("offset");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("paddedData");

    QTest::newRow("1 2, 256 byte data") << 1u << 2u << QByteArray(PAGE_SIZE_BYTES, 'x') << QByteArray(PAGE_SIZE_BYTES, 'x');
    QTest::newRow("3 4, 256 byte data") << 3u << 4u << QByteArray(PAGE_SIZE_BYTES, 'x') << QByteArray(PAGE_SIZE_BYTES, 'x');

    QTest::newRow("1 2, 0 byte data") << 1u << 2u << QByteArray() << QByteArray(PAGE_SIZE_BYTES, (char)0xFF);
    QTest::newRow("1 2, 1 byte data") << 1u << 2u << QByteArray(1, 'x') << QByteArray(1,'x').append(QByteArray(PAGE_SIZE_BYTES-1, (char)0xFF));
    QTest::newRow("1 2, 257 byte data") << 1u << 2u << QByteArray(257, 'x') << QByteArray(256, 'x');
}

void LightBuddyCommandsTests::writePageTest()
{
    QFETCH(uint32_t, sector);
    QFETCH(uint32_t, offset);
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, paddedData);

    QByteArray expectedData;
    expectedData.append(LightBuddyCommands::commandHeader());
    expectedData.append((char)COMMAND_WRITE_PAGE);
    expectedData.append(ByteArrayHelpers::uint32ToByteArrayBig(sector));
    expectedData.append(ByteArrayHelpers::uint32ToByteArrayBig(offset));
    expectedData.append(paddedData);

    QByteArray expectedResponse;
    expectedResponse.append('P');
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0x00);

    SerialCommand command = LightBuddyCommands::writePage(sector, offset, data);

    QCOMPARE(command.name, QString("writePage"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}

void LightBuddyCommandsTests::reloadAnimationsTest()
{
    QByteArray expectedData;
    expectedData.append(LightBuddyCommands::commandHeader());
    expectedData.append((char)COMMAND_RELOAD_ANIMATIONS);

    QByteArray expectedResponse;
    expectedResponse.append('P');
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0x00);

    SerialCommand command = LightBuddyCommands::reloadAnimations();

    QCOMPARE(command.name, QString("reloadAnimations"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}
