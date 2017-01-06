#include <QtTest>

#include "avr109commandstests.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    Avr109CommandsTests avr109CommandsTests;
    QTest::qExec(&avr109CommandsTests);



    return 0;
}
