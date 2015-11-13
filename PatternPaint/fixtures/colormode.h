#ifndef COLORMODE_H
#define COLORMODE_H

#include <QMetaType>
#include <QString>

enum ColorMode {
    RGB,
    GRB,
    COLOR_MODE_COUNT,
};

struct ColorModes {
    ColorMode colorMode;
    QString name;
};

Q_DECLARE_METATYPE(ColorMode)

extern ColorModes colorModes[COLOR_MODE_COUNT];

#endif // COLORMODE_H
