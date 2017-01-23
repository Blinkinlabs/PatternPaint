#include "memorymaptests.h"

#include <QtTest>

#include "memorysection.h"

void MemoryMapTests::nameSetTest()
{
    unsigned int start = 10;
    unsigned int size = 20;

    MemoryMap memoryMap(start, size);

    QVERIFY(memoryMap.start == start);
    QVERIFY(memoryMap.size == size);
}
