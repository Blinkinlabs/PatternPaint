#ifndef FIXTURE_H
#define FIXTURE_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QColor>
#include <QSize>

#include "colormode.h"
#include "brightnessmodel.h"

class Fixture : public QObject
{
    Q_OBJECT

public:
    Fixture(QObject *parent = 0);

    virtual QString getName() const = 0;

    virtual QList<QColor> getColorStreamForFrame(const QImage frame) const = 0;

    virtual int getLedCount() const = 0;

    // TODO: Push these to individual classes?
    virtual QSize getSize() const = 0;
    virtual void setSize(QSize newSize) = 0;

    virtual ColorMode getColorMode() const = 0;
    virtual void setColorMode(ColorMode newColorMode) = 0;

    virtual BrightnessModel *getBrightnessModel() const = 0;
    virtual void setBrightnessModel(BrightnessModel *BrightnessModel) = 0;
};

#endif // FIXTURE_H
