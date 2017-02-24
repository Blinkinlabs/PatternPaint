#ifndef BRIGHTNESSMODEL_H
#define BRIGHTNESSMODEL_H

#include <QColor>
#include <QObject>

class BrightnessModel : public QObject
{
    Q_OBJECT
public:
    virtual ~BrightnessModel();

    /// Perform a brightness correction (from screen space to LED space)
    /// on a given color value. Note that it drops the alpha channel.
    /// @param uncorrected 32-bit RGB color value in screen space
    /// @return 32-bit RGB color value converted to LED space
    virtual QColor correct(const QColor &uncorrected) const;
};


#endif // BRIGHTNESSMODEL_H
