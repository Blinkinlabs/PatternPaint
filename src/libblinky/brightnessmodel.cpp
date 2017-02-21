#include "brightnessmodel.h"

BrightnessModel::~BrightnessModel()
{

}

QColor BrightnessModel::correct(const QColor &uncorrected) const
{
    return uncorrected;
}
