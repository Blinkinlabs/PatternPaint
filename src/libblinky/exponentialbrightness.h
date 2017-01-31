#ifndef EXPONENTIALBRIGHTNESS_H
#define EXPONENTIALBRIGHTNESS_H

#include "brightnessmodel.h"

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

#endif // EXPONENTIALBRIGHTNESS_H
