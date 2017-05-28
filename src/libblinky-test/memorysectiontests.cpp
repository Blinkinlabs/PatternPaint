#include "memorysectiontests.h"

#include <QtTest>

#include "memorysection.h"

void MemorySectionTests::EmptyConstructorTest()
{
    MemorySection testSection;

    QVERIFY(testSection.name.isNull());
    QVERIFY(testSection.address == 0);
    QVERIFY(testSection.data.isNull());
}

void MemorySectionTests::FullConstructorTest()
{
    QString name("name");
    unsigned int address = 32;
    QByteArray data('x', 32);

    MemorySection testSection(name, address, data);

    QCOMPARE(testSection.name, name);
    QCOMPARE(testSection.address, address);
    QCOMPARE(testSection.data, data);
}

void MemorySectionTests::extentTest_data()
{
    QTest::addColumn<MemorySection>("memorySection");
    QTest::addColumn<unsigned int>("extent");

    QTest::newRow("null length") << MemorySection("",0,QByteArray()) << (unsigned int)0;
    QTest::newRow("1 length") << MemorySection("",0,QByteArray(1,'x')) << (unsigned int)0;
    QTest::newRow("big") << MemorySection("",0,QByteArray(999,'x')) << (unsigned int)998;
    QTest::newRow("null length + offset") << MemorySection("",1,QByteArray()) << (unsigned int)1;
    QTest::newRow("big + offset") << MemorySection("",1,QByteArray(999,'x')) << (unsigned int)999;
}

void MemorySectionTests::extentTest()
{
    QFETCH(MemorySection, memorySection);
    QFETCH(unsigned int, extent);

    QCOMPARE(memorySection.extent(), extent);
}


void MemorySectionTests::overlapsTest_data()
{
    QTest::addColumn<MemorySection>("a");
    QTest::addColumn<MemorySection>("b");
    QTest::addColumn<bool>("result");

    QTest::newRow("a null, no overlap")
            << MemorySection("",0,QByteArray())
            << MemorySection("",0,QByteArray(1,'x'))
            << false;
    QTest::newRow("b null, no overlap")
            << MemorySection("",0,QByteArray(1,'x'))
            << MemorySection("",0,QByteArray())
            << false;
    QTest::newRow("identical, overlaps")
            << MemorySection("",10,QByteArray(1,'x'))
            << MemorySection("",10,QByteArray(1,'x'))
            << true;
    QTest::newRow("a before  b, no overlap")
            << MemorySection("",10,QByteArray(1,'x'))
            << MemorySection("",11,QByteArray(1,'x'))
            << false;
    QTest::newRow("b before  a, no overlap")
            << MemorySection("",11,QByteArray(1,'x'))
            << MemorySection("",10,QByteArray(1,'x'))
            << false;
    QTest::newRow("a before  b, small overlap")
            << MemorySection("",10,QByteArray(2,'x'))
            << MemorySection("",11,QByteArray(2,'x'))
            << true;
    QTest::newRow("b before  a, small overlap")
            << MemorySection("",11,QByteArray(2,'x'))
            << MemorySection("",10,QByteArray(2,'x'))
            << true;
    QTest::newRow("a inside  b, small")
            << MemorySection("",11,QByteArray(1,'x'))
            << MemorySection("",10,QByteArray(3,'x'))
            << true;
    QTest::newRow("b inside  a")
            << MemorySection("",10,QByteArray(3,'x'))
            << MemorySection("",11,QByteArray(1,'x'))
            << true;
}

void MemorySectionTests::overlapsTest()
{
    QFETCH(MemorySection, a);
    QFETCH(MemorySection, b);
    QFETCH(bool, result);

    QCOMPARE(a.overlaps(b), result);
}
