#ifndef COLORMODEL_H
#define COLORMODEL_H

#include <QColor>

class ColorModel
{
public:
    /// Perform a rough brightness correction (from screen space to LED space)
    /// on a given color value. Note that it drops the alpha channel.
    /// @param 32-bit RGB color value in screen space
    /// @return 32-bit RGB color value converted to LED space
    static QRgb correctBrightness(QRgb uncorrected);
};

#endif // COLORMODEL_H
