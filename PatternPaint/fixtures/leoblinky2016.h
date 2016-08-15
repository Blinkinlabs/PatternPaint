#ifndef LEOBLINKY2016_H
#define LEOBLINKY2016_H

#include <QObject>

#include "fixture.h"

class LeoBlinky2016 : public Fixture
{
    Q_OBJECT

public:
    LeoBlinky2016(QSize size, ColorMode colorMode, BrightnessModel *brightnessModel,
                  QObject *parent = 0);

    ~LeoBlinky2016();

    QString getName() const;

    QList<QColor> getColorStreamForFrame(const QImage frame) const;

    QList<QPoint> getOutputLocations() const;

    QRect getExtents() const;

    int getLedCount() const;

    QSize getSize() const;
    void setSize(QSize newSize);

    ColorMode getColorMode() const;
    void setColorMode(ColorMode newColorMode);

    BrightnessModel *getBrightnessModel() const;
    void setBrightnessModel(BrightnessModel *newBrightnessModel);

private:
    QSize size;
    ColorMode colormode;
    BrightnessModel *brightnessModel;

    QList<QPoint> locations;
    QRect extents;
};

#endif // LEOBLINKY2016_H
