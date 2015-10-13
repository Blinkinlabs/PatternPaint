#include "colormodel.h"

#include <cmath>

QColor ColorModel::correctBrightness(QColor uncorrected)
{
    return QColor(
        int(255*pow(uncorrected.red()/255.0, 1.8)),
        int(255*pow(uncorrected.green()/255.0, 1.8)),
        int(255*pow(uncorrected.blue()/255.0, 2.1))
        );
}
