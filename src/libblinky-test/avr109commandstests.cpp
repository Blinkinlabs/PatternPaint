#include "avr109commandstests.h"

#include <QtTest>

#include "serialcommand.h"
#include "avr109commands.h"
#include "bytearrayhelpers.h"

// For atmega32u4. TODO: poll this from the library?
#define FLASH_PAGE_SIZE_BYTES 128
#define EEPROM_CHUNK_SIZE_BYTES 32

void Avr109CommandsTests::checkDeviceSignatureTest()
{
    QByteArray expectedData = "s";
    QByteArray expectedResponse = "\x87\x95\x1E";
    QByteArray expectedResponseMask;

    SerialCommand command = Avr109Commands::checkDeviceSignature();
    QVERIFY(command.name == "checkDeviceSignature");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
}

void Avr109CommandsTests::resetTest()
{
    QByteArray expectedData = "E";
    QByteArray expectedResponse = "\r";
    QByteArray expectedResponseMask;

    SerialCommand command = Avr109Commands::reset();
    QVERIFY(command.name == "reset");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
}

void Avr109CommandsTests::chipEraseTest()
{
    QByteArray expectedData = "e";
    QByteArray expectedResponse = "\r";
    QByteArray expectedResponseMask;

    SerialCommand command = Avr109Commands::chipErase();
    QVERIFY(command.name == "chipErase");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
    QVERIFY(command.timeout == 10000);
}

void Avr109CommandsTests::setAddressTest()
{
    int address = 0;

    QByteArray expectedData;
    expectedData.append('A');
    expectedData += ByteArrayCommands::uint16ToByteArrayBig(address);

    QByteArray expectedResponse = "\r";
    QByteArray expectedResponseMask;

    SerialCommand command = Avr109Commands::setAddress(address);
    QVERIFY(command.name == "setAddress");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
}

void Avr109CommandsTests::writeFlashPageTest_data()
{
    QTest::addColumn<QByteArray>("data");

    // Should produce a request with 0 write bytes (it's technically legal?)
    QTest::newRow("Empty data")   << QByteArray();

    // Should prodouce a request that's padded to 2 bytes (word boundary)
    QTest::newRow("1 byte data")   << QByteArray().append('x');

    // Should produce a request with the data as-is
    QTest::newRow("2 byte data")   << QByteArray().append('x').append('y');
    QTest::newRow("1 page data")   << QByteArray(FLASH_PAGE_SIZE_BYTES, 'x');

    // TODO: Oversize data test?
}

void Avr109CommandsTests::writeFlashPageTest()
{
    QFETCH(QByteArray, data);

    SerialCommand command = Avr109Commands::writeFlashPage(data);

    // We expect the response to be padded to a word boundary
    if(data.length() % 2 == 1) {
        data.append(static_cast<unsigned char>(0xFF));
    }

    QByteArray expectedData;
    expectedData.append('B');
    expectedData.append(ByteArrayCommands::uint16ToByteArrayBig(data.length()));
    expectedData.append('F');
    expectedData.append(data);

    QByteArray expectedResponse;
    expectedResponse.append('\r');

    QByteArray expectedResponseMask;

    QVERIFY(command.name == "writeFlashPage");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
}

void Avr109CommandsTests::verifyFlashPageTest_data()
{
    QTest::addColumn<QByteArray>("data");

    // Should produce a request with 0 write bytes (it's technically legal?)
    QTest::newRow("Empty data")   << QByteArray();

    // Should prodouce a request that's padded to 2 bytes (word boundary)
    QTest::newRow("1 byte data")   << QByteArray().append('x');

    // Should produce a request with the data as-is
    QTest::newRow("2 byte data")   << QByteArray().append('x').append('y');
    QTest::newRow("1 page data")   << QByteArray(FLASH_PAGE_SIZE_BYTES, 'x');
}


void Avr109CommandsTests::verifyFlashPageTest()
{
    QFETCH(QByteArray, data);

    SerialCommand command = Avr109Commands::verifyFlashPage(data);

    // We expect the response to be padded to a word boundary
    if(data.length() % 2 == 1) {
        data.append(static_cast<unsigned char>(0xFF));
    }

    QByteArray expectedData;
    expectedData.append('g');
    expectedData.append(ByteArrayCommands::uint16ToByteArrayBig(data.length()));
    expectedData.append('F');

    QByteArray expectedResponse;
    expectedResponse.append(data);

    QByteArray expectedResponseMask;

    QVERIFY(command.name == "verifyFlashPage");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
}

void Avr109CommandsTests::writeEepromBlockTest()
{
    QByteArray data = "0123456789ABCDEF";

    SerialCommand command = Avr109Commands::writeEepromBlock(data);

    QByteArray expectedData;
    expectedData.append('B');
    expectedData.append(ByteArrayCommands::uint16ToByteArrayBig(data.length()));
    expectedData.append('E');
    expectedData.append(data);

    QByteArray expectedResponse;
    expectedResponse.append('\r');

    QByteArray expectedResponseMask;

    QVERIFY(command.name == "writeEepromBlock");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
}

void Avr109CommandsTests::writeFlashBadAddressTest()
{
    QByteArray data(FLASH_PAGE_SIZE_BYTES*2, 'x');

    // Addresses that aren't page aligned should fail
    QVERIFY(Avr109Commands::writeFlash(data, 1).length() == 0);
    QVERIFY(Avr109Commands::writeFlash(data, FLASH_PAGE_SIZE_BYTES + 1).length() == 0);
}

void Avr109CommandsTests::writeFlashTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("address");

    // Test that different starting addresses work correctly
    QTest::newRow("address 0")   << QByteArray() << 0;
    QTest::newRow("address 1*page")   << QByteArray() << FLASH_PAGE_SIZE_BYTES;


    // Test that different data sizes work correctly
    QTest::newRow("no data")   << QByteArray() << 0;
    QTest::newRow("1 byte data")   << QByteArray(1, 'x') << 0;
    QTest::newRow("1 page data")   << QByteArray(FLASH_PAGE_SIZE_BYTES, 'x') << 0;
    QTest::newRow("1 page data + 1 byte")   << QByteArray(FLASH_PAGE_SIZE_BYTES+1, 'x') << 0;
    QTest::newRow("99 page data")   << QByteArray(FLASH_PAGE_SIZE_BYTES*99, 'x') << 0;
    QTest::newRow("99 page data + 1 byte")   << QByteArray(FLASH_PAGE_SIZE_BYTES*99+1, 'x') << 0;
}

void Avr109CommandsTests::writeFlashTest()
{
    QFETCH(QByteArray, data);
    QFETCH(int, address);

    // Build a list of expected output commands
    QList<QByteArray> chunks = ByteArrayCommands::chunkData(data, FLASH_PAGE_SIZE_BYTES);

    QList<SerialCommand> commands = Avr109Commands::writeFlash(data, address);

    // There should be 1 setaddress and 1 writeflashpage instruction per chunk
    QVERIFY(commands.length() == 2*chunks.length());

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {

        // The set address command should come first
        // Note: The address should also be page-aligned, rather than word aligned
        int expectedAddress = address + chunkIndex*FLASH_PAGE_SIZE_BYTES;
        SerialCommand expectedAddressCommand = Avr109Commands::setAddress(expectedAddress >> 1);
        SerialCommand expectedWriteFlashPageCommand = Avr109Commands::writeFlashPage(chunks.at(chunkIndex));

        QVERIFY(commands.at(chunkIndex*2    ) == expectedAddressCommand);
        QVERIFY(commands.at(chunkIndex*2 + 1) == expectedWriteFlashPageCommand);
    }
}

void Avr109CommandsTests::verifyFlashBadAddressTest()
{
    QByteArray data(FLASH_PAGE_SIZE_BYTES*2, 'x');

    // Addresses that aren't word aligned should fail
    QVERIFY(Avr109Commands::verifyFlash(data, 1).length() == 0);
    QVERIFY(Avr109Commands::verifyFlash(data, FLASH_PAGE_SIZE_BYTES + 1).length() == 0);
}


void Avr109CommandsTests::verifyFlashTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("address");

    // Test that different starting addresses work correctly
    QTest::newRow("address 0")   << QByteArray() << 0;
    QTest::newRow("address 1*page")   << QByteArray() << FLASH_PAGE_SIZE_BYTES;


    // Test that different data sizes work correctly
    QTest::newRow("no data")   << QByteArray() << 0;
    QTest::newRow("1 byte data")   << QByteArray(1, 'x') << 0;
    QTest::newRow("1 page data")   << QByteArray(FLASH_PAGE_SIZE_BYTES, 'x') << 0;
    QTest::newRow("1 page data + 1 byte")   << QByteArray(FLASH_PAGE_SIZE_BYTES+1, 'x') << 0;
    QTest::newRow("99 page data")   << QByteArray(FLASH_PAGE_SIZE_BYTES*99, 'x') << 0;
    QTest::newRow("99 page data + 1 byte")   << QByteArray(FLASH_PAGE_SIZE_BYTES*99+1, 'x') << 0;
}

void Avr109CommandsTests::verifyFlashTest()
{

    QFETCH(QByteArray, data);
    QFETCH(int, address);

    QList<SerialCommand> commands = Avr109Commands::verifyFlash(data, address);

    // Build a list of expected output commands
    QList<QByteArray> chunks = ByteArrayCommands::chunkData(data, FLASH_PAGE_SIZE_BYTES);

    // There should be 1 setaddress and 1 verifyflashpage instruction per chunk
    QVERIFY(commands.length() == 2*chunks.length());

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {

        // The set address command should come first
        // Note: The address should also be page-aligned, rather than word aligned
        int expectedAddress = address + chunkIndex*FLASH_PAGE_SIZE_BYTES;
        SerialCommand expectedAddressCommand = Avr109Commands::setAddress(expectedAddress >> 1);
        SerialCommand expectedVerifyFlashPageCommand = Avr109Commands::verifyFlashPage(chunks.at(chunkIndex));

        QVERIFY(commands.at(chunkIndex*2    ) == expectedAddressCommand);
        QVERIFY(commands.at(chunkIndex*2 + 1) == expectedVerifyFlashPageCommand);
    }
}

void Avr109CommandsTests::writeEepromTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("address");

    // Test that different starting addresses work correctly
    QTest::newRow("address 0")   << QByteArray() << 0;
    QTest::newRow("address 1")   << QByteArray() << 1;
    QTest::newRow("address 1*chunk")   << QByteArray() << EEPROM_CHUNK_SIZE_BYTES;


    // Test that different data sizes work correctly
    QTest::newRow("no data")   << QByteArray() << 0;
    QTest::newRow("1 chunk data")   << QByteArray(1, 'x') << 0;
    QTest::newRow("1 chunk data")   << QByteArray(EEPROM_CHUNK_SIZE_BYTES, 'x') << 0;
    QTest::newRow("1 chunk data + 1 byte")   << QByteArray(EEPROM_CHUNK_SIZE_BYTES+1, 'x') << 0;
    QTest::newRow("99 chunk data")   << QByteArray(EEPROM_CHUNK_SIZE_BYTES*99, 'x') << 0;
    QTest::newRow("99 chunk data + 1 byte")   << QByteArray(EEPROM_CHUNK_SIZE_BYTES*99+1, 'x') << 0;
}

void Avr109CommandsTests::writeEepromTest()
{
    QFETCH(QByteArray, data);
    QFETCH(int, address);

    QList<SerialCommand> commands = Avr109Commands::writeEeprom(data, address);


    // The set address command should come first
    // Note: The address has no special aligment requirements
    SerialCommand expectedAddressCommand = Avr109Commands::setAddress(address);

    QVERIFY(commands.at(0) == expectedAddressCommand);

    // Next, there should be 1 or more writeFlashPage commands
    QList<QByteArray> chunks = ByteArrayCommands::chunkData(data, EEPROM_CHUNK_SIZE_BYTES);

    // Verify that there are the correct number of commands
    QVERIFY(commands.length() == (1 + chunks.length()));

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        SerialCommand expectedCommand = Avr109Commands::writeEepromBlock(chunks.at(chunkIndex));

        QVERIFY(commands.at(1+chunkIndex) == expectedCommand);
    }
}

