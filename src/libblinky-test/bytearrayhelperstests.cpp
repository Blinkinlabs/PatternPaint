#include "bytearrayhelperstests.h"
#include "bytearrayhelpers.h"

#include <QtTest>

void ByteArrayHelpersTests::uint16ToByteArrayTest_data()
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

void ByteArrayHelpersTests::uint16ToByteArrayTest()
{
    QFETCH(int, value);
    QFETCH(QByteArray, result);

    QVERIFY(uint16ToByteArray(value) == result);
}

void ByteArrayHelpersTests::chunkDataTest_data()
{
    const int FLASH_PAGE_SIZE_BYTES = 128;

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

void ByteArrayHelpersTests::chunkDataTest()
{
    QFETCH(QByteArray, data);
    QFETCH(int, chunkSize);
    QFETCH(QList<QByteArray>, expectedChunks);

    QList<QByteArray> chunks = chunkData(data, chunkSize);

    QVERIFY(chunks == expectedChunks);
}
