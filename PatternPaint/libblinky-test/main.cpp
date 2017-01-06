#include <QtTest>

#include "avr109commandstests.h"
#include "colormodetests.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    Avr109CommandsTests avr109CommandsTests;
    QTest::qExec(&avr109CommandsTests);

    ColorModeTests colorModeTests;
    QTest::qExec(&colorModeTests);

    return 0;
}
