#include "exponentialbrightness.h"

#include <cmath>
#include <cfloat>

BrightnessModel::~BrightnessModel()
{

}

ExponentialBrightness::ExponentialBrightness(float r, float g, float b) :
    rFactor_(r),
    gFactor_(g),
    bFactor_(b)
{
    // Avoid zero-value correction factors
    if(rFactor_ == 0) {
        rFactor_ = FLT_MIN;
    }
    if(gFactor_ == 0) {
        gFactor_ = FLT_MIN;
    }
    if(bFactor_ == 0) {
        bFactor_ = FLT_MIN;
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
    return QColor(
        255*pow(uncorrected.redF()/255, rFactor_),
        255*pow(uncorrected.greenF()/255, gFactor_),
        255*pow(uncorrected.blueF()/255, bFactor_)
        );
}
