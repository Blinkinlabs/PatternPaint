#include "eightbyeightcommandstests.h"

#include <QTest>

#include "eightbyeightcommands.h"
#include "bytearrayhelpers.h"


// TODO: Pull these from the library?
#define CHUNK_SIZE_BYTES 64

void EightByEightCommandsTests::commandHeaderTest()
{
    QByteArray expectedResponse(12, (char)0xFF);

    QCOMPARE(EightByEightCommands::commandHeader(), expectedResponse);
}

void EightByEightCommandsTests::formatFilesystemTest()
{
    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_Format_Filesystem);
    expectedData.append((char)0x02);
    expectedData.append('E');
    expectedData.append('e');

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);

    SerialCommand command = EightByEightCommands::formatFilesystem();

    QCOMPARE(command.name, QString("formatFilesystem"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
    QCOMPARE(command.timeout, 20000);
}

void EightByEightCommandsTests::openFileTest()
{
    QString name("fileName");
    EightByEightCommands::FileMode mode = EightByEightCommands::FileMode_Read;

    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_Open_File);
    expectedData.append((char)(1 + name.length() + 1));
    expectedData.append((char)mode);
    expectedData.append(name.toUtf8());
    expectedData.append((char)0x00);

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);

    SerialCommand command = EightByEightCommands::openFile(name, mode);

    QCOMPARE(command.name, QString("openFile"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
    QCOMPARE(command.timeout, 5000);
}

void EightByEightCommandsTests::openFileNameTooLongCropsTest()
{
    QString name(31, 'x');  // Max filename length is 30 characters
    QString expectedName(30, 'x');

    EightByEightCommands::FileMode mode = EightByEightCommands::FileMode_Read;

    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_Open_File);
    expectedData.append((char)(1 + expectedName.length() + 1));
    expectedData.append((char)mode);
    expectedData.append(expectedName.toUtf8());
    expectedData.append((char)0x00);

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);

    SerialCommand command = EightByEightCommands::openFile(expectedName, mode);

    QCOMPARE(command.name, QString("openFile"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}

void EightByEightCommandsTests::writeChunkTest_data()
{
    QTest::addColumn<QByteArray>("data");

    // Should produce a request with 0 write bytes (it's technically legal?)
    QTest::newRow("Empty data")   << QByteArray();

    // Should produce a request with the data as-is
    QTest::newRow("1 byte data")   << QByteArray().append('x');
    QTest::newRow("2 byte data")   << QByteArray().append('x').append('y');
    QTest::newRow("1 page data")   << QByteArray(CHUNK_SIZE_BYTES, 'x');

    // TODO: Oversize data test?
}

void EightByEightCommandsTests::writeChunkTest()
{
    QFETCH(QByteArray, data);

    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_Write);
    expectedData.append((char)data.length());
    expectedData.append(data);

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x01);
    expectedResponse.append((char)data.length());

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);

    SerialCommand command = EightByEightCommands::writeChunk(data);

    QCOMPARE(command.name, QString("writeChunk"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}

void EightByEightCommandsTests::writeTest_data()
{
    QTest::addColumn<QByteArray>("data");

    // Test that different data sizes work correctly
    QTest::newRow("no data")   << QByteArray();
    QTest::newRow("1 byte data")   << QByteArray(1, 'x');
    QTest::newRow("1 chunk data")   << QByteArray(CHUNK_SIZE_BYTES, 'x');
    QTest::newRow("1 chunk data + 1 byte")   << QByteArray(CHUNK_SIZE_BYTES+1, 'x');
    QTest::newRow("99 chunk data")   << QByteArray(CHUNK_SIZE_BYTES*99, 'x');
    QTest::newRow("99 chunk data + 1 byte")   << QByteArray(CHUNK_SIZE_BYTES*99+1, 'x');
}

void EightByEightCommandsTests::writeTest()
{
    QFETCH(QByteArray, data);

    // Build a list of expected output commands
    QList<QByteArray> chunks = ByteArrayCommands::chunkData(data, CHUNK_SIZE_BYTES);

    QList<SerialCommand> commands = EightByEightCommands::write(data);

    // There should be 1 verifyChunk instruction per chunk
    QCOMPARE(commands.length(), chunks.length());

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        SerialCommand expectedVerifyChunkCommand = EightByEightCommands::writeChunk(chunks.at(chunkIndex));

        QCOMPARE(commands.at(chunkIndex), expectedVerifyChunkCommand);
    }
}

void EightByEightCommandsTests::verifyChunkTest_data()
{
    QTest::addColumn<QByteArray>("data");

    // Should produce a request with 0 write bytes (it's technically legal?)
    QTest::newRow("Empty data")   << QByteArray();

    // Should produce a request with the data as-is
    QTest::newRow("1 byte data")   << QByteArray().append('x');
    QTest::newRow("2 byte data")   << QByteArray().append('x').append('y');
    QTest::newRow("1 page data")   << QByteArray(CHUNK_SIZE_BYTES, 'x');

    // TODO: Oversize data test?
}

void EightByEightCommandsTests::verifyChunkTest()
{
    QFETCH(QByteArray, data);

    SerialCommand command = EightByEightCommands::verifyChunk(data);

    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_Read);
    expectedData.append((char)1);
    expectedData.append((char)data.length());

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)data.length());
    expectedResponse.append(data);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append(QByteArray(data.length(),(char)0xFF));

    QCOMPARE(command.name, QString("verifyChunk"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}

void EightByEightCommandsTests::verifyTest_data()
{
    QTest::addColumn<QByteArray>("data");

    // Test that different data sizes work correctly
    QTest::newRow("no data")   << QByteArray();
    QTest::newRow("1 byte data")   << QByteArray(1, 'x');
    QTest::newRow("1 chunk data")   << QByteArray(CHUNK_SIZE_BYTES, 'x');
    QTest::newRow("1 chunk data + 1 byte")   << QByteArray(CHUNK_SIZE_BYTES+1, 'x');
    QTest::newRow("99 chunk data")   << QByteArray(CHUNK_SIZE_BYTES*99, 'x');
    QTest::newRow("99 chunk data + 1 byte")   << QByteArray(CHUNK_SIZE_BYTES*99+1, 'x');
}

void EightByEightCommandsTests::verifyTest()
{
    QFETCH(QByteArray, data);

    // Build a list of expected output commands
    QList<QByteArray> chunks = ByteArrayCommands::chunkData(data, CHUNK_SIZE_BYTES);

    QList<SerialCommand> commands = EightByEightCommands::verify(data);

    // There should be 1 verifyChunk instruction per chunk
    QCOMPARE(commands.length(), chunks.length());

    // Verify each chunk is correct
    for(int chunkIndex = 0; chunkIndex < chunks.length(); chunkIndex++) {
        SerialCommand expectedVerifyChunkCommand = EightByEightCommands::verifyChunk(chunks.at(chunkIndex));

        QCOMPARE(commands.at(chunkIndex), expectedVerifyChunkCommand);
    }
}

void EightByEightCommandsTests::closeFileTest()
{
    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_Close_File);
    expectedData.append((char)0x00);

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);

    SerialCommand command = EightByEightCommands::closeFile();

    QCOMPARE(command.name, QString("closeFile"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}

void EightByEightCommandsTests::lockFileAccessTest()
{
    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_Lock_File_Access);
    expectedData.append((char)0x00);

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);

    SerialCommand command = EightByEightCommands::lockFileAccess();

    QCOMPARE(command.name, QString("lockFileAccess"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}

void EightByEightCommandsTests::unlockFileAccessTest()
{
    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_Unlock_File_Access);
    expectedData.append((char)0x00);

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x00);

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);

    SerialCommand command = EightByEightCommands::unlockFileAccess();

    QCOMPARE(command.name, QString("unlockFileAccess"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}

void EightByEightCommandsTests::getFirmwareVersionTest()
{
    QByteArray expectedData;
    expectedData.append(EightByEightCommands::commandHeader());
    expectedData.append((char)EightByEightCommands::Command_GetFirmwareVersion);
    expectedData.append((char)0x00);

    QByteArray expectedResponse;
    expectedResponse.append((char)0x00);
    expectedResponse.append((char)0x04);
    expectedResponse.append(QByteArray(4,(char)0x00));

    QByteArray expectedResponseMask;
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append((char)0xFF);
    expectedResponseMask.append(QByteArray(4,(char)0x00));

    SerialCommand command = EightByEightCommands::getFirmwareVersion();

    QCOMPARE(command.name, QString("getFirmwareVersion"));
    QCOMPARE(command.data, expectedData);
    QCOMPARE(command.expectedResponse, expectedResponse);
    QCOMPARE(command.expectedResponseMask, expectedResponseMask);
}
