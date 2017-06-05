#ifndef COLORMODE_H
#define COLORMODE_H

#include "libblinkyglobal.h"

#include <QMetaType>
#include <QString>
#include <QColor>

enum ColorMode {
    RGB,
    RBG,
    GRB,
    GBR,
    BRG,
    BGR,
    COLOR_MODE_COUNT,
};

Q_DECLARE_METATYPE(ColorMode)

struct LIBBLINKY_EXPORT ColorModeNames {
    ColorMode colorMode;
    QString name;
};


LIBBLINKY_EXPORT QByteArray colorToBytes(ColorMode colorMode, const QColor &color);

LIBBLINKY_EXPORT bool colorModeValid(ColorMode colorMode);

LIBBLINKY_EXPORT extern const ColorModeNames colorModeNames[COLOR_MODE_COUNT];

#endif // COLORMODE_H
