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
    if(brightnessModel != NULL)
        delete brightnessModel;
}

QList<QColor> Fixture::getColorStreamForFrame(const QImage frame) const
{
    QList<QColor> colorStream;

    if (frame.isNull())
        return colorStream;

    QList<QPoint> locations = getOutputLocations();

    foreach(QPoint point, locations) {
        QColor pixel = frame.pixel(point);
        colorStream.append(brightnessModel->correct(pixel));
    }

    return colorStream;
}

QList<QPoint> Fixture::getOutputLocations() const
{
    return locations;
}

QRect Fixture::getExtents() const
{
    return extents;
}

int Fixture::getLedCount() const
{
    return locations.count();
}

ColorMode Fixture::getColorMode() const
{
    return colorMode;
}

void Fixture::setColorMode(const ColorMode &newColorMode)
{
    colorMode = newColorMode;
}

BrightnessModel *Fixture::getBrightnessModel() const
{
    return brightnessModel;
}

void Fixture::setBrightnessModel(BrightnessModel *newBrightnessModel)
{
    brightnessModel = newBrightnessModel;
}
