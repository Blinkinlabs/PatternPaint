#include "memorymaptests.h"

#include <QtTest>

#include "memorysection.h"

void MemoryMapTests::constructorTest()
{
    unsigned int start = 10;
    unsigned int size = 20;

    MemoryMap memoryMap(start, size);

    QVERIFY(memoryMap.start == start);
    QVERIFY(memoryMap.size == size);
    QVERIFY(memoryMap.memorySections.isEmpty());
}

void MemoryMapTests::addSectionTest_data()
{
    QTest::addColumn<MemorySection>("memorySection");
    QTest::addColumn<bool>("result");

    QTest::newRow("null length") << MemorySection("",0,QByteArray()) << (unsigned int)0;
    QTest::newRow("1 length") << MemorySection("",0,QByteArray(1,'x')) << (unsigned int)0;
    QTest::newRow("big") << MemorySection("",0,QByteArray(999,'x')) << (unsigned int)998;
}

void MemoryMapTests::addSectionTest()
{
    QFETCH(MemorySection, memorySection);
    QFETCH(bool, result);
    // TODO
}
