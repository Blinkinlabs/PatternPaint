#include "colormode.h"

const ColorModeNames colorModeNames[COLOR_MODE_COUNT] = {
    {RGB, "RGB"},
    {RBG, "RBG"},
    {GRB, "GRB"},
    {GBR, "GBR"},
    {BRG, "BRG"},
    {BGR, "BGR"},
};

QByteArray colorToBytes(ColorMode colorMode, const QColor &color)
{
    QByteArray bytes;

    // TODO: Optimize me?
    switch (colorMode) {
    case RGB:
        bytes.append(color.red());
        bytes.append(color.green());
        bytes.append(color.blue());
        break;
    case RBG:
        bytes.append(color.red());
        bytes.append(color.blue());
        bytes.append(color.green());
        break;
    case GRB:
        bytes.append(color.green());
        bytes.append(color.red());
        bytes.append(color.blue());
        break;
    case GBR:
        bytes.append(color.green());
        bytes.append(color.blue());
        bytes.append(color.red());
        break;
    case BRG:
        bytes.append(color.blue());
        bytes.append(color.red());
        bytes.append(color.green());
        break;
    case BGR:
        bytes.append(color.blue());
        bytes.append(color.green());
        bytes.append(color.red());
        break;

    default:
        break;
    }

    return bytes;
}

bool colorModeValid(ColorMode colorMode) {
    switch (colorMode) {
    case RGB:
    case RBG:
    case GRB:
    case GBR:
    case BRG:
    case BGR:
        return true;
        break;

    default:
        return false;
        break;
    }
}
