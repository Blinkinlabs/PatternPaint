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

    QVERIFY(brightness.rFactor() == rFactor);
    QVERIFY(brightness.gFactor() == gFactor);
    QVERIFY(brightness.bFactor() == bFactor);
}

void ExponentialBrightnessTests::constructorZeroTest()
{
    float rFactor = 0;
    float gFactor = 0;
    float bFactor = 0;

    ExponentialBrightness brightness(rFactor,gFactor,bFactor);

    QVERIFY(brightness.rFactor() == FLT_MIN);
    QVERIFY(brightness.gFactor() == FLT_MIN);
    QVERIFY(brightness.bFactor() == FLT_MIN);
}


void ExponentialBrightnessTests::correctTest_data()
{
    QTest::addColumn<float>("rFactor");
    QTest::addColumn<float>("gFactor");
    QTest::addColumn<float>("bFactor");
    QTest::addColumn<QColor>("inputColor");
    QTest::addColumn<QColor>("resultColor");

    QTest::newRow("zero factor")
            << float(0.0) << float(0.0) << float(0.0)
            << QColor(255,255,255)
            << QColor(1,1,1);
}

void ExponentialBrightnessTests::correctTest()
{
    QFETCH(float, rFactor);
    QFETCH(float, gFactor);
    QFETCH(float, bFactor);
    QFETCH(QColor, inputColor);
    QFETCH(QColor, resultColor);

    ExponentialBrightness model(rFactor, gFactor, bFactor);

    qDebug() << resultColor << model.correct(inputColor);

    QVERIFY(model.correct(inputColor) == resultColor);
}
