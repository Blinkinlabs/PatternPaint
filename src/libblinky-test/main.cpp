#include <QtTest>

#include "avr109commandstests.h"
#include "blinkypendantcommandstests.h"
#include "colormodetests.h"
#include "serialcommandtests.h"
#include "lightbuddycommandstests.h"
#include "bytearrayhelperstests.h"
#include "memorysectiontests.h"
#include "memorymaptests.h"
#include "exponentialbrightnesstests.h"
#include "fixturetests.h"
#include "matrixfixturetests.h"
#include "linearfixturetests.h"

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

    MemorySectionTests flashSectionTests;
    result |= QTest::qExec(&flashSectionTests);

    MemoryMapTests memoryMapTests;
    result |= QTest::qExec(&memoryMapTests);

    ExponentialBrightnessTests exponentialBrightnessTests;
    result |= QTest::qExec(&exponentialBrightnessTests);

    BlinkyPendantCommandsTests blinkyPendantCommandsTests;
    result |= QTest::qExec(&blinkyPendantCommandsTests);

    LightBuddyCommandsTests lightBuddyCommandsTests;
    result |= QTest::qExec(&lightBuddyCommandsTests);

    FixtureTests fixtureTests;
    result |= QTest::qExec(&fixtureTests);

    MatrixFixtureTests matrixFixtureTests;
    result |= QTest::qExec(&matrixFixtureTests);

    LinearFixtureTests linearFixtureTests;
    result |= QTest::qExec(&linearFixtureTests);

    return result;
}
