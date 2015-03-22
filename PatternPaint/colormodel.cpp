#include "colormodel.h"

#include <cmath>

QRgb ColorModel::correctBrightness(QRgb uncorrected)
{
    return qRgb(
        int(255*pow(qRed(  uncorrected)/255.0, 1.8)),
        int(255*pow(qGreen(uncorrected)/255.0, 1.8)),
        int(255*pow(qBlue( uncorrected)/255.0, 2.1))
        );
}
