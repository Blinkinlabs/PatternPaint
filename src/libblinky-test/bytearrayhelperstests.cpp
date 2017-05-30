#include "bytearrayhelperstests.h"
#include "bytearrayhelpers.h"

#include <QtTest>

void ByteArrayHelpersTests::uint16ToByteArrayLittleTest_data()
{
    QTest::addColumn<int>("value");
    QTest::addColumn<QByteArray>("result");

    QTest::newRow("zero")       << 0           << QByteArray().append('\x00').append('\x00');
    QTest::newRow("one")        << 1           << QByteArray().append('\x01').append('\x00');
    QTest::newRow("small")      << 300          << QByteArray().append('\x2C').append('\x01');
    QTest::newRow("max")        << ((1 << 16) - 1) << QByteArray().append('\xFF').append('\xFF');
}

void ByteArrayHelpersTests::uint16ToByteArrayLittleTest()
{
    QFETCH(int, value);
    QFETCH(QByteArray, result);

    QCOMPARE(ByteArrayCommands::uint16ToByteArrayLittle(value), result);
}

void ByteArrayHelpersTests::uint16ToByteArrayBigTest_data()
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

void ByteArrayHelpersTests::uint16ToByteArrayBigTest()
{
    QFETCH(int, value);
    QFETCH(QByteArray, result);

    QCOMPARE(ByteArrayCommands::uint16ToByteArrayBig(value), result);
}

void ByteArrayHelpersTests::uint32ToByteArrayBigTest_data()
{
    QTest::addColumn<uint32_t>("value");
    QTest::addColumn<QByteArray>("result");

    QTest::newRow("zero")       << 0u         << QByteArray().append('\x00').append('\x00').append('\x00').append('\x00');
    QTest::newRow("one")        << 1u         << QByteArray().append('\x00').append('\x00').append('\x00').append('\x01');
    QTest::newRow("small")      << 300u       << QByteArray().append('\x00').append('\x00').append('\x01').append('\x2C');
    QTest::newRow("almost max") << (std::numeric_limits<uint32_t>::max() - 1)
                                << QByteArray().append('\xFF').append('\xFF').append('\xFF').append('\xFE');
    QTest::newRow("max")        << std::numeric_limits<uint32_t>::max()
                                << QByteArray().append('\xFF').append('\xFF').append('\xFF').append('\xFF');
}

void ByteArrayHelpersTests::uint32ToByteArrayBigTest()
{
    QFETCH(uint32_t, value);
    QFETCH(QByteArray, result);

    QCOMPARE(ByteArrayCommands::uint32ToByteArrayBig(value), result);
}

void ByteArrayHelpersTests::uint32ToByteArrayLittleTest_data()
{
    QTest::addColumn<uint32_t>("value");
    QTest::addColumn<QByteArray>("result");

    QTest::newRow("zero")       << 0u         << QByteArray().append('\x00').append('\x00').append('\x00').append('\x00');
    QTest::newRow("one")        << 1u         << QByteArray().append('\x01').append('\x00').append('\x00').append('\x00');
    QTest::newRow("small")      << 300u       << QByteArray().append('\x2C').append('\x01').append('\x00').append('\x00');
    QTest::newRow("almost max") << (std::numeric_limits<uint32_t>::max() - 1)
                                << QByteArray().append('\xFE').append('\xFF').append('\xFF').append('\xFF');
    QTest::newRow("max")        << std::numeric_limits<uint32_t>::max()
                                << QByteArray().append('\xFF').append('\xFF').append('\xFF').append('\xFF');
}

void ByteArrayHelpersTests::uint32ToByteArrayLittleTest()
{
    QFETCH(uint32_t, value);
    QFETCH(QByteArray, result);

    QCOMPARE(ByteArrayCommands::uint32ToByteArrayLittle(value), result);
}

void ByteArrayHelpersTests::byteArrayToUint32LittleTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<uint32_t>("result");

    QTest::newRow("empty Byte Array")       << QByteArray() << 0u;
    QTest::newRow("too small Byte Array")       << QByteArray(3, 'x') << 0u;
    QTest::newRow("too big Byte Array")       << QByteArray(5, 'x') << 0u;

    QTest::newRow("zero")       << QByteArray().append('\x00').append('\x00').append('\x00').append('\x00') << 0u;
    QTest::newRow("one")        << QByteArray().append('\x01').append('\x00').append('\x00').append('\x00') << 1u;
    QTest::newRow("small")      << QByteArray().append('\x2C').append('\x01').append('\x00').append('\x00') << 300u;
    QTest::newRow("almost max") << QByteArray().append('\xFE').append('\xFF').append('\xFF').append('\xFF')
                                << (std::numeric_limits<uint32_t>::max() - 1);
    QTest::newRow("max")        << QByteArray().append('\xFF').append('\xFF').append('\xFF').append('\xFF')
                                << std::numeric_limits<uint32_t>::max();
}

void ByteArrayHelpersTests::byteArrayToUint32LittleTest()
{
    QFETCH(QByteArray, data);
    QFETCH(uint32_t, result);

    QCOMPARE(ByteArrayCommands::byteArrayToUint32Little(data), result);
}

void ByteArrayHelpersTests::byteArrayToUint32BigTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<uint32_t>("result");

    QTest::newRow("empty Byte Array")       << QByteArray() << 0u;
    QTest::newRow("too small Byte Array")       << QByteArray(3, 'x') << 0u;
    QTest::newRow("too big Byte Array")       << QByteArray(5, 'x') << 0u;

    QTest::newRow("zero")       << QByteArray().append('\x00').append('\x00').append('\x00').append('\x00') << 0u;
    QTest::newRow("one")        << QByteArray().append('\x00').append('\x00').append('\x00').append('\x01') << 1u;
    QTest::newRow("small")      << QByteArray().append('\x00').append('\x00').append('\x01').append('\x2C') << 300u;
    QTest::newRow("almost max") << QByteArray().append('\xFF').append('\xFF').append('\xFF').append('\xFE')
                                << (std::numeric_limits<uint32_t>::max() - 1);
    QTest::newRow("max")        << QByteArray().append('\xFF').append('\xFF').append('\xFF').append('\xFF')
                                << std::numeric_limits<uint32_t>::max();
}

void ByteArrayHelpersTests::byteArrayToUint32BigTest()
{
    QFETCH(QByteArray, data);
    QFETCH(uint32_t, result);

    QCOMPARE(ByteArrayCommands::byteArrayToUint32Big(data), result);
}

void ByteArrayHelpersTests::chunkDataTest_data()
{
    const unsigned int FLASH_PAGE_SIZE_BYTES = 128;

    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<unsigned int>("chunkSize");
    QTest::addColumn<QList<QByteArray> >("expectedChunks");

    QTest::newRow("zero chunk size, no data")   << QByteArray() << 0u << QList<QByteArray> ();
    QTest::newRow("zero chunk size, data")   << QByteArray(3, 'x') << 0u << QList<QByteArray> ();

    // Create a dataset that is not divisible by the chunk size
    QByteArray oddSizeData;
    int oddSizeDataLength = FLASH_PAGE_SIZE_BYTES*1.5;
    unsigned int oddSizeDataChunkSize = FLASH_PAGE_SIZE_BYTES;
    for(int i = 0; i < oddSizeDataLength; i++)
        oddSizeData.append(static_cast<unsigned char>(i & 0xFF));

    QList<QByteArray> oddSizeChunks;
    oddSizeChunks.append(oddSizeData.mid(0, oddSizeDataChunkSize));
    oddSizeChunks.append(oddSizeData.mid(oddSizeDataChunkSize, (oddSizeDataLength-oddSizeDataChunkSize)));
    QTest::newRow("odd chunk size")   << oddSizeData << oddSizeDataChunkSize << oddSizeChunks;


    // Create a largeish dataset with a length divisible by 1 and FLASH_PAGE_SIZE_BYTES
    QByteArray largeData;
    for(int i = 0; i < (2^14); i++)
        largeData.append(static_cast<unsigned char>(i & 0xFF));

    // Test that a chunk size of 1 works correctly
    QList<QByteArray> chunkSize1;
    for(int i = 0; i < largeData.length(); i += 1)
        chunkSize1.append(largeData.mid(i, 1));

    QTest::newRow("1 byte chunk size") << largeData << 1u << chunkSize1;

    // Test that a chunk size of 1 page works correctly
    QList<QByteArray> chunkSizePage;
    for(int i = 0; i < largeData.length(); i += FLASH_PAGE_SIZE_BYTES)
        chunkSizePage.append(largeData.mid(i, FLASH_PAGE_SIZE_BYTES));

    QTest::newRow("1 page chunk size") << largeData << FLASH_PAGE_SIZE_BYTES << chunkSizePage;
}

void ByteArrayHelpersTests::chunkDataTest()
{
    QFETCH(QByteArray, data);
    QFETCH(unsigned int, chunkSize);
    QFETCH(QList<QByteArray>, expectedChunks);

    QCOMPARE(ByteArrayCommands::chunkData(data, chunkSize), expectedChunks);
}

void ByteArrayHelpersTests::padToBoundaryTest_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<unsigned int>("boundarySize");
    QTest::addColumn<QByteArray>("expectedResult");

    QTest::newRow("zero pad size, no data") << QByteArray() << 0u << QByteArray();
    QTest::newRow("zero pad size, some data") << QByteArray(3, 'x') << 0u << QByteArray(3, 'x');
    QTest::newRow("too big pad size some data") << QByteArray(1, 'x')
                                      << static_cast<unsigned int>(std::numeric_limits<int>::max()) + 1 << QByteArray(1, 'x');

    QTest::newRow("1 byte pad size, 1 byte data") << QByteArray(1, 'x') << 1u << QByteArray(1,'x');
    QTest::newRow("2 byte pad size, 1 byte data") << QByteArray(1, 'x') << 2u << QByteArray(1,'x').append(0xFF);
    QTest::newRow("2 byte pad size, 3 byte data") << QByteArray(3, 'x') << 2u << QByteArray(3,'x').append(0xFF);

    // Create a largeish dataset
    QByteArray largeData;
    for(int i = 0; i < 1234; i++)
        largeData.append(static_cast<unsigned char>(i & 0xFF));

    // Test that a pad size of 1 works correctly
    QTest::newRow("1 byte pad size, 1234 data")   << largeData << 1u << largeData;
    QTest::newRow("1400 byte pad size, 1234 data")   << largeData << 1400u << QByteArray(largeData).append(166, 0xFF);
    QTest::newRow("1000 byte pad size, 1234 data")   << largeData << 1000u << QByteArray(largeData).append(766, 0xFF);
}

void ByteArrayHelpersTests::padToBoundaryTest()
{
    QFETCH(QByteArray, data);
    QFETCH(unsigned int, boundarySize);
    QFETCH(QByteArray, expectedResult);

    ByteArrayCommands::padToBoundary(data, boundarySize);
    QCOMPARE(data, expectedResult);
}
