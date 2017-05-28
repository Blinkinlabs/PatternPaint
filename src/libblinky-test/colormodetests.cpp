#include <QtTest>

#include "colormodetests.h"
#include "colormode.h"

void ColorModeTests::colorModesTest_data()
{
    QTest::addColumn<ColorMode>("colorMode");
    QTest::addColumn<QString>("name");

    QTest::newRow("RGB")   << ColorMode::RGB << QString("RGB");
    QTest::newRow("RBG")   << ColorMode::RBG << QString("RBG");
    QTest::newRow("GRB")   << ColorMode::GRB << QString("GRB");
    QTest::newRow("GBR")   << ColorMode::GBR << QString("GBR");
    QTest::newRow("BRG")   << ColorMode::BRG << QString("BRG");
    QTest::newRow("BGR")   << ColorMode::BGR << QString("BGR");
}

void ColorModeTests::colorModesTest()
{
    QFETCH(ColorMode, colorMode);
    QFETCH(QString, name);

    QCOMPARE(colorModeNames[colorMode].name, name);
}

void ColorModeTests::colorModeValidTest_data()
{
    QTest::addColumn<ColorMode>("colorMode");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("RGB")   << ColorMode::RGB << true;
    QTest::newRow("RBG")   << ColorMode::RBG << true;
    QTest::newRow("GRB")   << ColorMode::GRB << true;
    QTest::newRow("GBR")   << ColorMode::GBR << true;
    QTest::newRow("BRG")   << ColorMode::BRG << true;
    QTest::newRow("BGR")   << ColorMode::BGR << true;

    QTest::newRow("RGB-1")   << (ColorMode)(ColorMode::RGB - 1) << false;
    QTest::newRow("BGR+1")   << (ColorMode)(ColorMode::BGR + 1) << false;
}

void ColorModeTests::colorModeValidTest()
{
    QFETCH(ColorMode, colorMode);
    QFETCH(bool, expectedResult);

    QCOMPARE(colorModeValid(colorMode), expectedResult);
}


void ColorModeTests::colorToBytesTest_data()
{
    QTest::addColumn<ColorMode>("colorMode");
    QTest::addColumn<QColor>("color");
    QTest::addColumn<QByteArray>("bytes");

    QTest::newRow("RGB")   << ColorMode::RGB << QColor(1,2,3) << QByteArray().append(1).append(2).append(3);
    QTest::newRow("RBG")   << ColorMode::RBG << QColor(1,2,3) << QByteArray().append(1).append(3).append(2);
    QTest::newRow("GRB")   << ColorMode::GRB << QColor(1,2,3) << QByteArray().append(2).append(1).append(3);
    QTest::newRow("GBR")   << ColorMode::GBR << QColor(1,2,3) << QByteArray().append(2).append(3).append(1);
    QTest::newRow("BRG")   << ColorMode::BRG << QColor(1,2,3) << QByteArray().append(3).append(1).append(2);
    QTest::newRow("BGR")   << ColorMode::BGR << QColor(1,2,3) << QByteArray().append(3).append(2).append(1);
}

void ColorModeTests::colorToBytesTest()
{
    QFETCH(ColorMode, colorMode);
    QFETCH(QColor, color);
    QFETCH(QByteArray, bytes);

    QCOMPARE(colorToBytes(colorMode, color), bytes);
}

void ColorModeTests::colorToBytesBadModeFailsTest()
{
    QCOMPARE(colorToBytes(COLOR_MODE_COUNT, QColor(1,2,3)).length(), 0);
}
