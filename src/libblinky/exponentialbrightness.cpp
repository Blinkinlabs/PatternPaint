#include "exponentialbrightness.h"
#include <QDebug>

#include <cmath>

ExponentialBrightness::ExponentialBrightness(float r, float g, float b) :
    rFactor_(r),
    gFactor_(g),
    bFactor_(b)
{
    // Avoid zero-value correction factors
    if(rFactor_ == 0) {
        rFactor_ = ZERO_VALUE;
    }
    if(gFactor_ == 0) {
        gFactor_ = ZERO_VALUE;
    }
    if(bFactor_ == 0) {
        bFactor_ = ZERO_VALUE;
    }
}

float ExponentialBrightness::rFactor() const {
    return rFactor_;
}

float ExponentialBrightness::gFactor() const {
    return gFactor_;
}

float ExponentialBrightness::bFactor() const {
    return bFactor_;
}

QColor ExponentialBrightness::correct(const QColor &uncorrected) const
{
    int r = round(255*pow(uncorrected.redF(), rFactor_));
    int g = round(255*pow(uncorrected.greenF(), gFactor_));
    int b = round(255*pow(uncorrected.blueF(), bFactor_));

    return QColor(r,g,b);
}
