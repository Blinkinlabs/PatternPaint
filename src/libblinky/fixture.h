#ifndef FIXTURE_H
#define FIXTURE_H

#include "libblinkyglobal.h"

#include <QObject>
#include <QImage>
#include <QList>
#include <QColor>
#include <QSize>
#include <QSharedPointer>

#include "colormode.h"
#include "brightnessmodel.h"

class LIBBLINKY_EXPORT Fixture : public QObject
{
    Q_OBJECT

public:
    enum class Type {
        CUSTOM,     // Custom list of points
        LINEAR,     // Linear array of points
        MATRIX      // Rectangular matrix of points
    };

    // TODO: The 'size' bit won't make sense for arbitrary fixtures.
    static Fixture *makeFixture(QString type, QSize size);

    Fixture(QObject *parent = 0);

    virtual Fixture::Type getType() const;

    QString getName() const;
    void setName(const QString &newName);

    ColorMode getColorMode() const;
    void setColorMode(const ColorMode &newColorMode);

    QSharedPointer<BrightnessModel> getBrightnessModel() const;
    void setBrightnessModel(BrightnessModel *BrightnessModel);

    void setLocations(const QList<QPoint> &newLocations);
    const QList<QPoint> & getLocations() const;

    // Get the number of LEDs in this fixture
    unsigned int getCount() const;

    // Get the extents of this fixture in drawing pixel coordinates
    // TODO: Make float, handle negative coordinates
    QRect getExtents() const;

    QList<QColor> getColorStream(const QImage &frame) const;

private:
    QString name;

    ColorMode colorMode;
    QSharedPointer<BrightnessModel> brightnessModel;

    QList<QPoint> locations;
    QRect extents;
};

LIBBLINKY_EXPORT QDataStream &operator<<(QDataStream &out, const Fixture::Type &type);
LIBBLINKY_EXPORT QDataStream &operator>>(QDataStream &in, Fixture::Type &type);

#endif // FIXTURE_H
