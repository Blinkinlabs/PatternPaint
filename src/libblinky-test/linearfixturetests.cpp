#include <QTest>

#include "linearfixturetests.h"
#include "linearfixture.h"

void LinearFixtureTests::constructorTest()
{
    LinearFixture linearFixture(0);

    QVERIFY(linearFixture.getLength() == 0);
    QVERIFY(linearFixture.getCount() == 0);
}

void LinearFixtureTests::setLengthTest_data()
{
    QTest::addColumn<unsigned int>("length");

    QTest::newRow("0") << 0u;
    QTest::newRow("1") << 1u;
    QTest::newRow("10") << 10u;
}

void LinearFixtureTests::setLengthTest()
{
    QFETCH(unsigned int, length);

    LinearFixture linearFixture(0);

    linearFixture.setLength(length);

    QVERIFY(linearFixture.getLength() == length);
    QVERIFY(linearFixture.getCount() == length);

    for(unsigned int offset = 0; offset < length; offset++) {
        QVERIFY(linearFixture.getLocations().at(offset) == QPoint(0,offset));
    }
}
