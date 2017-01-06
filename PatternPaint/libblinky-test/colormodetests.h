#ifndef COLORMODETESTS_H
#define COLORMODETESTS_H

#include <QObject>

class ColorModeTests : public QObject
{
    Q_OBJECT
private slots:
    void colorModesTest_data();
    void colorModesTest();

    void colorToBytesTest_data();
    void colorToBytesTest();
};

#endif // COLORMODETESTS_H
