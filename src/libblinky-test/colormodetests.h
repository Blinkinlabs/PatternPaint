#ifndef COLORMODETESTS_H
#define COLORMODETESTS_H

#include "multitests.h"

#include <QObject>

class ColorModeTests : public QObject
{
    Q_OBJECT
private slots:
    void colorModesTest_data();
    void colorModesTest();

    void colorModeValidTest_data();
    void colorModeValidTest();

    void colorToBytesTest_data();
    void colorToBytesTest();

    void colorToBytesBadModeFailsTest();
};

TEST_DECLARE(ColorModeTests)

#endif // COLORMODETESTS_H
