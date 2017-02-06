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
    // TODO: The 'size' bit won't make sense for arbitrary fixtures.
    static Fixture *makeFixture(QString type, QSize size);

    Fixture(QObject *parent = 0);

    virtual ~Fixture();

    /// Get a user-readable description of the fixture
    /// @return
    virtual QString getName() const = 0;

    virtual QList<QColor> getColorStreamForFrame(const QImage frame) const;

    // Get the cooridnates of each LED in this fixture, in order of their address
    virtual QList<QPoint> getOutputLocations() const;

    // Get the extents of this fixture in drawing pixel coordinates
    // TODO: Make float, handle negative coordinates
    virtual QRect getExtents() const;

    // Get the number of LEDs in this fixture
    virtual int getLedCount() const;

    virtual ColorMode getColorMode() const;
    virtual void setColorMode(const ColorMode &newColorMode);

    virtual BrightnessModel *getBrightnessModel() const;
    virtual void setBrightnessModel(BrightnessModel *BrightnessModel);

protected:
    ColorMode colorMode;
    BrightnessModel *brightnessModel;

    QList<QPoint> locations;
    QRect extents;
};

#endif // FIXTURE_H
