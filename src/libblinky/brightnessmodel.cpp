#include "brightnessmodel.h"

#include <cmath>

BrightnessModel::~BrightnessModel()
{

}

ExponentialBrightness::ExponentialBrightness(float r, float g, float b) :
    r(r),
    g(g),
    b(b)
{
}

QColor ExponentialBrightness::correct(QColor uncorrected)
{
    float scale = 1;

    return QColor(
        int(255*pow(uncorrected.red()/255.0*scale, r)),
        int(255*pow(uncorrected.green()/255.0*scale, g)),
        int(255*pow(uncorrected.blue()/255.0*scale, b))
        );
}
