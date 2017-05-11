#ifndef COLORMODETESTS_H
#define COLORMODETESTS_H

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

#endif // COLORMODETESTS_H
