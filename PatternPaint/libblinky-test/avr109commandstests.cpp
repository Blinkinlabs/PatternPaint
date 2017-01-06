#include <QtTest>
#include "avr109commandstests.h"

#include "serialcommand.h"
#include "avr109commands.h"

// For atmega32u4. TODO: poll this from the library?
#define FLASH_PAGE_SIZE_BYTES 128
#define EEPROM_CHUNK_SIZE_BYTES 32

void Avr109CommandsTests::int16ToByteArrayTest_data()
{
    QTest::addColumn<int>("value");
    QTest::addColumn<QByteArray>("result");

    QTest::newRow("negative")   << -1          << QByteArray();
    QTest::newRow("too big")    << (1 << 16)   << QByteArray();
    QTest::newRow("zero")       << 0           << QByteArray().append('\x00').append('\x00');
    QTest::newRow("one")        << 1           << QByteArray().append('\x00').append('\x01');
    QTest::newRow("small")      << 300          << QByteArray().append('\x01').append('\x2C');
    QTest::newRow("max")        << ((1 << 16) - 1) << QByteArray().append('\xFF').append('\xFF');
}

void Avr109CommandsTests::int16ToByteArrayTest()
{
    QFETCH(int, value);
    QFETCH(QByteArray, result);

    QVERIFY(Avr109Commands::int16ToByteArray(value) == result);
}

void Avr109CommandsTests::chunkDataTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("chunkSize");
    QTest::addColumn<QList<QByteArray> >("expectedChunks");

    QTest::newRow("negative chunk size")   << QByteArray() << -1 << QList<QByteArray> ();
    QTest::newRow("zero chunk size")   << QByteArray() << 0 << QList<QByteArray> ();

    // Create a dataset that is not divisible by the chunk size
    QByteArray oddSizeData;
    int oddSizeDataLength = FLASH_PAGE_SIZE_BYTES*1.5;
    int oddSizeDataChunkSize = FLASH_PAGE_SIZE_BYTES;
    for(int i = 0; i < oddSizeDataLength; i++) {
        oddSizeData.append(static_cast<unsigned char>(i & 0xFF));
    }
    QList<QByteArray> oddSizeChunks;
    oddSizeChunks.append(oddSizeData.mid(0, oddSizeDataChunkSize));
    oddSizeChunks.append(oddSizeData.mid(oddSizeDataChunkSize, (oddSizeDataLength-oddSizeDataChunkSize)));
    QTest::newRow("odd chunk size")   << oddSizeData << oddSizeDataChunkSize << oddSizeChunks;


    // Create a largeish dataset with a length divisible by 1 and FLASH_PAGE_SIZE_BYTES
    QByteArray largeData;
    for(int i = 0; i < (2^14); i++) {
        largeData.append(static_cast<unsigned char>(i & 0xFF));
    }

    // Test that a chunk size of 1 works correctly
    QList<QByteArray> chunkSize1;
    for(int i = 0; i < largeData.length(); i += 1) {
        chunkSize1.append(largeData.mid(i, 1));
    }
    QTest::newRow("1 byte chunk size")   << largeData << 1 << chunkSize1;

    // Test that a chunk size of 1 page works correctly
    QList<QByteArray> chunkSizePage;
    for(int i = 0; i < largeData.length(); i += FLASH_PAGE_SIZE_BYTES) {
        chunkSizePage.append(largeData.mid(i, FLASH_PAGE_SIZE_BYTES));
    }
    QTest::newRow("1 page chunk size")   << largeData << FLASH_PAGE_SIZE_BYTES << chunkSizePage;
}

void Avr109CommandsTests::chunkDataTest()
{
    QFETCH(QByteArray, data);
    QFETCH(int, chunkSize);
    QFETCH(QList<QByteArray>, expectedChunks);

    QList<QByteArray> chunks = Avr109Commands::chunkData(data, chunkSize);

    QVERIFY(chunks == expectedChunks);
}

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

void Avr109CommandsTests::setAddressTest()
{
    int address = 0;

    QByteArray expectedData;
    expectedData.append('A');
    expectedData += Avr109Commands::int16ToByteArray(address);

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
    expectedData.append(Avr109Commands::int16ToByteArray(data.length()));
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
    expectedData.append(Avr109Commands::int16ToByteArray(data.length()));
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
    expectedData.append(Avr109Commands::int16ToByteArray(data.length()));
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
}

void Avr109CommandsTests::writeFlashTest()
{
    QFETCH(QByteArray, data);
    QFETCH(int, address);

    QList<SerialCommand> commands = Avr109Commands::writeFlash(data, address);


    // The set address command should come first
    // Note: The address should also be page-aligned, rather than word aligned
    SerialCommand expectedAddressCommand = Avr109Commands::setAddress(address >> 1);

    QVERIFY(commands.at(0).name == expectedAddressCommand.name);
    QVERIFY(commands.at(0).data == expectedAddressCommand.data);
    QVERIFY(commands.at(0).expectedResponse == expectedAddressCommand.expectedResponse);
    QVERIFY(commands.at(0).expectedResponseMask == expectedAddressCommand.expectedResponseMask);


    // Next, there should be 1 or more writeFlashPage commands
    QList<QByteArray> chunks = Avr109Commands::chunkData(data, FLASH_PAGE_SIZE_BYTES);

    // Verify that there are the correct number of commands
    QVERIFY(commands.length() == (1 + chunks.length()));

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        SerialCommand expectedCommand = Avr109Commands::writeFlashPage(chunks.at(chunkIndex));

        QVERIFY(commands.at(1+chunkIndex).name == expectedCommand.name);
        QVERIFY(commands.at(1+chunkIndex).data == expectedCommand.data);
        QVERIFY(commands.at(1+chunkIndex).expectedResponse == expectedCommand.expectedResponse);
        QVERIFY(commands.at(1+chunkIndex).expectedResponseMask == expectedCommand.expectedResponseMask);
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
}

void Avr109CommandsTests::verifyFlashTest()
{
    QFETCH(QByteArray, data);
    QFETCH(int, address);

    QList<SerialCommand> commands = Avr109Commands::verifyFlash(data, address);


    // The set address command should come first
    // Note: The address should be word aligned
    SerialCommand expectedAddressCommand = Avr109Commands::setAddress(address >> 1);

    QVERIFY(commands.at(0).name == expectedAddressCommand.name);
    QVERIFY(commands.at(0).data == expectedAddressCommand.data);
    QVERIFY(commands.at(0).expectedResponse == expectedAddressCommand.expectedResponse);
    QVERIFY(commands.at(0).expectedResponseMask == expectedAddressCommand.expectedResponseMask);


    // Next, there should be 1 or more writeFlashPage commands
    QList<QByteArray> chunks = Avr109Commands::chunkData(data, FLASH_PAGE_SIZE_BYTES);

    // Verify that there are the correct number of commands
    QVERIFY(commands.length() == (1 + chunks.length()));

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        SerialCommand expectedCommand = Avr109Commands::verifyFlashPage(chunks.at(chunkIndex));

        QVERIFY(commands.at(1+chunkIndex).name == expectedCommand.name);
        QVERIFY(commands.at(1+chunkIndex).data == expectedCommand.data);
        QVERIFY(commands.at(1+chunkIndex).expectedResponse == expectedCommand.expectedResponse);
        QVERIFY(commands.at(1+chunkIndex).expectedResponseMask == expectedCommand.expectedResponseMask);
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
}

void Avr109CommandsTests::writeEepromTest()
{
    QFETCH(QByteArray, data);
    QFETCH(int, address);

    QList<SerialCommand> commands = Avr109Commands::writeEeprom(data, address);


    // The set address command should come first
    // Note: The address has no special aligment requirements
    SerialCommand expectedAddressCommand = Avr109Commands::setAddress(address);

    QVERIFY(commands.at(0).name == expectedAddressCommand.name);
    QVERIFY(commands.at(0).data == expectedAddressCommand.data);
    QVERIFY(commands.at(0).expectedResponse == expectedAddressCommand.expectedResponse);
    QVERIFY(commands.at(0).expectedResponseMask == expectedAddressCommand.expectedResponseMask);


    // Next, there should be 1 or more writeFlashPage commands
    QList<QByteArray> chunks = Avr109Commands::chunkData(data, EEPROM_CHUNK_SIZE_BYTES);

    // Verify that there are the correct number of commands
    QVERIFY(commands.length() == (1 + chunks.length()));

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        SerialCommand expectedCommand = Avr109Commands::writeEepromBlock(chunks.at(chunkIndex));

        QVERIFY(commands.at(1+chunkIndex).name == expectedCommand.name);
        QVERIFY(commands.at(1+chunkIndex).data == expectedCommand.data);
        QVERIFY(commands.at(1+chunkIndex).expectedResponse == expectedCommand.expectedResponse);
        QVERIFY(commands.at(1+chunkIndex).expectedResponseMask == expectedCommand.expectedResponseMask);
    }
}

