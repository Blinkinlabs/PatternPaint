#ifndef MEMORYMAPTESTS_H
#define MEMORYMAPTESTS_H

#include "multitests.h"

#include <QObject>

class MemoryMapTests : public QObject
{
    Q_OBJECT
private slots:
    void constructorTest();

    void addSectionTest_data();
    void addSectionTest();
};

TEST_DECLARE(MemoryMapTests)

#endif // MEMORYMAPTESTS_H
