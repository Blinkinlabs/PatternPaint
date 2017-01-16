#ifndef BRIGHTNESSMODEL_H
#define BRIGHTNESSMODEL_H

#include <QColor>

class BrightnessModel
{
public:
    virtual ~BrightnessModel();

    /// Perform a rough brightness correction (from screen space to LED space)
    /// on a given color value. Note that it drops the alpha channel.
    /// @param uncorrected 32-bit RGB color value in screen space
    /// @return 32-bit RGB color value converted to LED space
    virtual QColor correct(QColor uncorrected) = 0;
};

class ExponentialBrightness : public BrightnessModel
{
public:
    ExponentialBrightness(float r, float g, float b);

    QColor correct(QColor uncorrected);

private:
    float r;
    float g;
    float b;
};

#endif // BRIGHTNESSMODEL_H
