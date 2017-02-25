#include "fixture.h"
#include "linearfixture.h"
#include "matrixfixture.h"


#include <QDebug>

Fixture *Fixture::makeFixture(QString type, QSize size)
{
    Fixture *fixture;

    if(type == "Matrix-Zigzag") {
        fixture = new MatrixFixture(size, MatrixFixture::MODE_ZIGZAG);
    }
    else if(type == "Matrix-Rows") {
        fixture = new MatrixFixture(size, MatrixFixture::MODE_ROWS);
    }
    else if(type == "Linear") {
        fixture = new LinearFixture(size.height());
    }
    else {
        qCritical() << "Unrecognized fixture type:" << type;
        fixture = new LinearFixture(1);
    }

    return fixture;
}

Fixture::Fixture(QObject *parent) :
    QObject(parent)
{

}

Fixture::~Fixture()
{
}

void Fixture::setName(const QString &newName)
{
    name = newName;
}

QString Fixture::getName() const
{
    return name;
}

QList<QColor> Fixture::getColorStream(const QImage &frame) const
{
    QList<QColor> colorStream;

    if(brightnessModel.isNull())
        return colorStream;

    for (QPoint point : locations) {
        QColor pixel(0,0,0,255);

        if(frame.rect().contains(point))
            pixel = frame.pixel(point);

        colorStream.append(brightnessModel->correct(pixel));
    }

    return colorStream;
}

void Fixture::setLocations(const QList<QPoint> &newLocations)
{
    locations = newLocations;

    // Re-calculate extents
    extents.setLeft(std::numeric_limits<int>::max());
    extents.setRight(std::numeric_limits<int>::min());
    extents.setTop(std::numeric_limits<int>::max());
    extents.setBottom(std::numeric_limits<int>::min());

    for(QPoint point : locations) {
        if(extents.left() > point.x())
            extents.setLeft(point.x());
        if(extents.right() < point.x())
            extents.setRight(point.x());
        if(extents.top() > point.y())
            extents.setTop(point.y());
        if(extents.bottom() < point.y())
            extents.setBottom(point.y());
    }
}

const QList<QPoint> & Fixture::getLocations() const
{
    return locations;
}

unsigned int Fixture::getCount() const
{
    return locations.count();
}

QRect Fixture::getExtents() const
{
    return extents;
}

ColorMode Fixture::getColorMode() const
{
    return colorMode;
}

void Fixture::setColorMode(const ColorMode &newColorMode)
{
    colorMode = newColorMode;
}

QSharedPointer<BrightnessModel> Fixture::getBrightnessModel() const
{
    return brightnessModel;
}

void Fixture::setBrightnessModel(BrightnessModel *newBrightnessModel)
{
    brightnessModel = QSharedPointer<BrightnessModel>(newBrightnessModel);
}
