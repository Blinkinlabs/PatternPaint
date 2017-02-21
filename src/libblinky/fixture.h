#ifndef FIXTURE_H
#define FIXTURE_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QColor>
#include <QSize>
#include <QSharedPointer>

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

    virtual QString getName() const;
    virtual void setName(const QString &newName);

    virtual ColorMode getColorMode() const;
    virtual void setColorMode(const ColorMode &newColorMode);

    virtual QSharedPointer<BrightnessModel> getBrightnessModel() const;
    virtual void setBrightnessModel(BrightnessModel *BrightnessModel);

    virtual void setLocations(const QList<QPoint> &newLocations);
    virtual const QList<QPoint> & getLocations() const;

    // Get the number of LEDs in this fixture
    virtual int getCount() const;

    // Get the extents of this fixture in drawing pixel coordinates
    // TODO: Make float, handle negative coordinates
    virtual QRect getExtents() const;

    virtual QList<QColor> getColorStream(const QImage &frame) const;

private:
    QString name;

    ColorMode colorMode;
    QSharedPointer<BrightnessModel> brightnessModel;

    QList<QPoint> locations;
    QRect extents;
};

#endif // FIXTURE_H
