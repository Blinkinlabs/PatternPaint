#include <QtTest>

#include "avr109commandstests.h"
#include "colormodetests.h"
#include "serialcommandtests.h"
#include "bytearrayhelperstests.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    int result = 0;

    Avr109CommandsTests avr109CommandsTests;
    result |= QTest::qExec(&avr109CommandsTests);

    ColorModeTests colorModeTests;
    result |= QTest::qExec(&colorModeTests);

    ByteArrayHelpersTests byteArrayHelpersTests;
    result |= QTest::qExec(&byteArrayHelpersTests);

    SerialCommandTests serialCommandTests;
    result |= QTest::qExec(&serialCommandTests);

    return result;
}