#ifndef EXPONENTIALBRIGHTNESS_H
#define EXPONENTIALBRIGHTNESS_H

#include "libblinkyglobal.h"

#include "brightnessmodel.h"

// TODO: Use an actual minimum float value
#define ZERO_VALUE float(.000001)

class LIBBLINKY_EXPORT ExponentialBrightness : public BrightnessModel
{
public:
    ExponentialBrightness(float rFactor, float gFactor, float bFactor);

    QColor correct(const QColor &uncorrected) const;

    float rFactor() const;
    float gFactor() const;
    float bFactor() const;

private:
    float rFactor_;
    float gFactor_;
    float bFactor_;
};

#endif // EXPONENTIALBRIGHTNESS_H
