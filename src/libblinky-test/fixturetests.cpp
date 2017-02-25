#include <QTest>
#include <QSignalSpy>

#include "fixturetests.h"
#include "fixture.h"

#include "exponentialbrightness.h"


void FixtureTests::setNameTest()
{
    QString name = "TestName";

    Fixture fixture;
    fixture.setName(name);

    QVERIFY(fixture.getName() == name);
}

void FixtureTests::setColorModeTest()
{
    ColorMode colorMode = ColorMode::GRB;

    Fixture fixture;
    fixture.setColorMode(colorMode);

    QVERIFY(fixture.getColorMode() == colorMode);
}

void FixtureTests::setBrightnessModelTest()
{
    BrightnessModel *brightnessModel = new ExponentialBrightness(1,1,1);

    Fixture fixture;
    fixture.setBrightnessModel(brightnessModel);

    QVERIFY(fixture.getBrightnessModel() == brightnessModel);
}

void FixtureTests::setLocationsTest()
{
    QList<QPoint> locations;
    for(int i = 0; i < 100; i++)
        locations.append(QPoint(i,i*10));

    Fixture fixture;

    QSignalSpy spy(&fixture, SIGNAL(locationsChanged(void)));

    fixture.setLocations(locations);

    QVERIFY(fixture.getLocations() == locations);
    QVERIFY(fixture.getCount() == 100);

    QCOMPARE(spy.count(), 1);
}

void FixtureTests::getExtentsTest_data()
{
    QTest::addColumn<QList<QPoint> >("locations");
    QTest::addColumn<QRect>("extents");

    QTest::newRow("empty list") << QList<QPoint>()
                                << QRect(QPoint(std::numeric_limits<int>::max(),std::numeric_limits<int>::max()),
                                         QPoint(std::numeric_limits<int>::min(),std::numeric_limits<int>::min()));
    QTest::newRow("1 point") << QList<QPoint>({{1,1}})
                                << QRect(QPoint(1,1),QPoint(1,1));
    QTest::newRow("2 points") << QList<QPoint>({{1,-1},{-1,1}})
                                << QRect(QPoint(-1,-1),QPoint(1,1));
    QTest::newRow("3 points") << QList<QPoint>({{1,-1},{-1,1},{0,2}})
                                << QRect(QPoint(-1,-1),QPoint(1,2));
}

void FixtureTests::getExtentsTest()
{
    QFETCH(QList<QPoint>, locations);
    QFETCH(QRect, extents);

    Fixture fixture;
    fixture.setLocations(locations);

    QVERIFY(fixture.getExtents() == extents);

}

void FixtureTests::getColorStreamNullBrightnessModelTest()
{
    // Don't expect usable data, but at least don't crash.

    Fixture fixture;
    fixture.setLocations(QList<QPoint>({{1,1}}));

    QList<QColor> colorStream = fixture.getColorStream(QImage());

    QVERIFY(colorStream.count() == 0);
}

void FixtureTests::getColorStreamNullFrameTest()
{
    // A null frame is equivalent to being out of bounds, so
    // we expect blank data back.

    Fixture fixture;
    fixture.setLocations(QList<QPoint>({{1,1}}));
    fixture.setBrightnessModel(new ExponentialBrightness(1,1,1));

    QList<QColor> colorStream = fixture.getColorStream(QImage());

    QVERIFY(colorStream.count() == 1);
    QVERIFY(colorStream.at(0) == QColor(0,0,0,255));
}

void FixtureTests::getColorStreamBadLocationsTest()
{
    QImage image(2,2,QImage::Format_ARGB32_Premultiplied);
    image.fill(QColor(1,2,3,255));

    QList<QPoint> locations;
    locations.append(QPoint(-1,0)); // Negative out of bounds
    locations.append(QPoint(0,0));  // In bounds
    locations.append(QPoint(1,1));  // In bounds
    locations.append(QPoint(2,2));  // Positive out of bounds

    Fixture fixture;
    fixture.setLocations(locations);
    fixture.setBrightnessModel(new ExponentialBrightness(1,1,1));

    QList<QColor> colorStream = fixture.getColorStream(image);

    QVERIFY(colorStream.count() == 4);
    QVERIFY(colorStream.at(0) == QColor(0,0,0,255));
    QVERIFY(colorStream.at(1) == QColor(1,2,3,255));
    QVERIFY(colorStream.at(2) == QColor(1,2,3,255));
    QVERIFY(colorStream.at(3) == QColor(0,0,0,255));
}

void FixtureTests::getColorStreamBrightnessModelAppliedTest()
{
    QImage image(1,1,QImage::Format_ARGB32_Premultiplied);
    image.fill(QColor(128,128,128,255));

    QList<QPoint> locations;
    locations.append(QPoint(0,0));  // In bounds

    Fixture fixture;
    fixture.setLocations(locations);
    fixture.setBrightnessModel(new ExponentialBrightness(1,2,3));

    QList<QColor> colorStream = fixture.getColorStream(image);

    QVERIFY(colorStream.count() == 1);
    QVERIFY(colorStream.at(0) == QColor(128,64,32,255));

}
