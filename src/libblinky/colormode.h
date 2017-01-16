#ifndef COLORMODE_H
#define COLORMODE_H

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

struct ColorModes {
    ColorMode colorMode;
    QString name;
};


QByteArray colorToBytes(ColorMode colorMode, const QColor &color);

extern ColorModes colorModes[COLOR_MODE_COUNT];

#endif // COLORMODE_H