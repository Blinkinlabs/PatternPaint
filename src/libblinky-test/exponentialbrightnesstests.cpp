#include <QtTest>
#include <QDebug>
#include <cfloat>

#include "exponentialbrightnesstests.h"
#include "exponentialbrightness.h"

void ExponentialBrightnessTests::constructorTest()
{
    float rFactor = .1;
    float gFactor = .2;
    float bFactor = .3;

    ExponentialBrightness brightness(rFactor,gFactor,bFactor);

    QCOMPARE(brightness.rFactor(), rFactor);
    QCOMPARE(brightness.gFactor(), gFactor);
    QCOMPARE(brightness.bFactor(), bFactor);
}

void ExponentialBrightnessTests::constructorZeroTest()
{
    float rFactor = 0;
    float gFactor = 0;
    float bFactor = 0;

    ExponentialBrightness brightness(rFactor,gFactor,bFactor);

    QCOMPARE(brightness.rFactor(), ZERO_VALUE);
    QCOMPARE(brightness.gFactor(), ZERO_VALUE);
    QCOMPARE(brightness.bFactor(), ZERO_VALUE);
}


void ExponentialBrightnessTests::correctTest_data()
{
    QTest::addColumn<float>("rFactor");
    QTest::addColumn<float>("gFactor");
    QTest::addColumn<float>("bFactor");
    QTest::addColumn<QColor>("inputColor");
    QTest::addColumn<QColor>("resultColor");

    QTest::newRow("linear, 0")
            << float(1.0) << float(1.0) << float(1.0)
            << QColor(0,0,0)
            << QColor(0,0,0);
    QTest::newRow("linear, 128")
            << float(1.0) << float(1.0) << float(1.0)
            << QColor(128,128,128)
            << QColor(128,128,128);
    QTest::newRow("linear, 255")
            << float(1.0) << float(1.0) << float(1.0)
            << QColor(255,255,255)
            << QColor(255,255,255);
    QTest::newRow("different:1,2,3, 128")
            << float(1.0) << float(2.0) << float(3.0)
            << QColor(128,128,128)
            << QColor(128,64,32);
}

void ExponentialBrightnessTests::correctTest()
{
    QFETCH(float, rFactor);
    QFETCH(float, gFactor);
    QFETCH(float, bFactor);
    QFETCH(QColor, inputColor);
    QFETCH(QColor, resultColor);

    ExponentialBrightness model(rFactor, gFactor, bFactor);

    QCOMPARE(model.correct(inputColor), resultColor);
}
