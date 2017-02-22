#include "blinkypendantcommandstests.h"

#include <QtTest>

#include "blinkypendantcommands.h"
#include "bytearrayhelpers.h"

#define COMMAND_START_WRITE 0x01
#define COMMAND_WRITE       0x02
#define COMMAND_STOP_WRITE  0x03

#define PAGE_SIZE_BYTES     1024    // Size unit for writes to flash
#define CHUNK_SIZE_BYTES    64      // Number of bytes the write command requires per call

void BlinkyPendantCommandsTests::commandHeaderTest()
{
    QByteArray expectedResponse(10, (char)0xFF);

    QVERIFY(BlinkyPendantCommands::commandHeader() == expectedResponse);
}

void BlinkyPendantCommandsTests::startWriteTest()
{
    QByteArray expectedData;
    expectedData.append(BlinkyPendantCommands::commandHeader());
    expectedData.append((char)COMMAND_START_WRITE);

    QByteArray expectedResponse;
    expectedResponse.append('P');
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0x00);

    SerialCommand command = BlinkyPendantCommands::startWrite();

    QVERIFY(command.name == "startWrite");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
    QVERIFY(command.timeout == 2000);
}

void BlinkyPendantCommandsTests::stopWriteTest()
{
    QByteArray expectedData;
    expectedData.append(BlinkyPendantCommands::commandHeader());
    expectedData.append((char)COMMAND_STOP_WRITE);

    QByteArray expectedResponse;
    expectedResponse.append('P');
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0x00);

    SerialCommand command = BlinkyPendantCommands::stopWrite();

    QVERIFY(command.name == "stopWrite");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
}

void BlinkyPendantCommandsTests::writeFlashChunkTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("paddedData");

    // Should produce a request padded to CHUNK_SIZE_BYTES
    QTest::newRow("Empty data")   << QByteArray()
                                  << QByteArray(CHUNK_SIZE_BYTES, (char)0xFF);

    // Should prodouce a request that's padded to CHUNK_SIZE_BYTES
    QTest::newRow("1 byte data")   << QByteArray().append('x')
                                   << QByteArray().append('x').append(QByteArray(CHUNK_SIZE_BYTES-1, (char)0xFF));

    // Should prodouce a request that's padded to CHUNK_SIZE_BYTES
    QTest::newRow("64 byte data")   << QByteArray().append('x').append(QByteArray(CHUNK_SIZE_BYTES-1, 'y'))
                                    << QByteArray().append('x').append(QByteArray(CHUNK_SIZE_BYTES-1, 'y'));

    // Should prodouce a request that's chopped to CHUNK_SIZE_BYTES
    QTest::newRow("65 byte data")   << QByteArray().append(QByteArray(CHUNK_SIZE_BYTES+1, 'y'))
                                    << QByteArray().append(QByteArray(CHUNK_SIZE_BYTES, 'y'));
}

void BlinkyPendantCommandsTests::writeFlashChunkTest()
{
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, paddedData);

    SerialCommand command = BlinkyPendantCommands::writeFlashChunk(data);

    QByteArray expectedData;
    expectedData.append(BlinkyPendantCommands::commandHeader());
    expectedData.append(COMMAND_WRITE);
    expectedData.append(paddedData);

    QByteArray expectedResponse;
    expectedResponse.append('P');
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0x00);

    QVERIFY(command.name == "writeFlashPage");
    QVERIFY(command.data == expectedData);
    QVERIFY(command.expectedResponse == expectedResponse);
    QVERIFY(command.expectedResponseMask == expectedResponseMask);
}

void BlinkyPendantCommandsTests::writeFlashTest_data()
{
    QTest::addColumn<QByteArray>("data");

    // Test that different data sizes work correctly
    QTest::newRow("no data")   << QByteArray();
    QTest::newRow("1 byte data")   << QByteArray(1, 'x');
    QTest::newRow("1 page data")   << QByteArray(PAGE_SIZE_BYTES, 'x');
    QTest::newRow("1 page data + 1 byte")   << QByteArray(PAGE_SIZE_BYTES+1, 'x');
    QTest::newRow("99 page data")   << QByteArray(PAGE_SIZE_BYTES*99, 'x');
    QTest::newRow("99 page data + 1 byte")   << QByteArray(PAGE_SIZE_BYTES*99+1, 'x');
}

void BlinkyPendantCommandsTests::writeFlashTest()
{
    QFETCH(QByteArray, data);

    QByteArray paddedData(data);

    while((paddedData.length() % PAGE_SIZE_BYTES) != 0)
        paddedData.append((char)0xFF);

    // Build a list of expected output commands
    QList<QByteArray> chunks = chunkData(paddedData, CHUNK_SIZE_BYTES);

    QList<SerialCommand> commands = BlinkyPendantCommands::writeFlash(data);

    qDebug() <<chunks.length() << commands.length();

    // There should be 1 writeFlashChunk instruction per chunk
    QVERIFY(commands.length() == chunks.length());

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        SerialCommand expectedWriteFlashChunkCommand = BlinkyPendantCommands::writeFlashChunk(chunks.at(chunkIndex));

        QVERIFY(commands.at(chunkIndex) == expectedWriteFlashChunkCommand);
    }
}
