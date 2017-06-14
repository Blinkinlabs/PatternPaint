#ifndef FLASHSECTIONTESTS_H
#define FLASHSECTIONTESTS_H

#include "multitests.h"

#include <QObject>

class MemorySectionTests : public QObject
{
    Q_OBJECT
private slots:
    void EmptyConstructorTest();
    void FullConstructorTest();

    void extentTest_data();
    void extentTest();

    void overlapsTest_data();
    void overlapsTest();
};

TEST_DECLARE(MemorySectionTests)

#endif // FLASHSECTIONTESTS_H
