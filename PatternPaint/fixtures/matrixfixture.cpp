#include "matrixfixture.h"

#include <limits>

MatrixFixture::MatrixFixture(
    QSize size, ColorMode colorMode, BrightnessModel *brightnessModel, QObject *parent) :
    Fixture(parent),
    colormode(colorMode),
    brightnessModel(brightnessModel)
{
    setSize(size);
}

MatrixFixture::~MatrixFixture()
{
    if(brightnessModel != NULL)
        free(brightnessModel);
}

QString MatrixFixture::getName() const
{
    return QString("Matrix");
}

QList<QColor> MatrixFixture::getColorStreamForFrame(const QImage frame) const
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

QList<QPoint> MatrixFixture::getOutputLocations() const
{
    return locations;
}

QRect MatrixFixture::getExtents() const
{
    return extents;
}

int MatrixFixture::getLedCount() const
{
    return size.height()*size.width();
}

QSize MatrixFixture::getSize() const
{
    return size;
}

void MatrixFixture::setSize(QSize newSize)
{
    size = newSize;

    locations.clear();

    extents.setLeft(std::numeric_limits<int>::max());
    extents.setRight(std::numeric_limits<int>::min());
    extents.setTop(std::numeric_limits<int>::max());
    extents.setBottom(std::numeric_limits<int>::min());

    for (int x = 0; x < size.width(); x++) {
        for (int y = 0; y < size.height(); y++) {
            QPoint point (x, x%2 ? size.height()-1-y : y);
            locations.append(point);

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
}

ColorMode MatrixFixture::getColorMode() const
{
    return colormode;
}

void MatrixFixture::setColorMode(ColorMode newColorMode)
{
    colormode = newColorMode;
}

BrightnessModel *MatrixFixture::getBrightnessModel() const
{
    return brightnessModel;
}

void MatrixFixture::setBrightnessModel(BrightnessModel *newBrightnessModel)
{
    brightnessModel = newBrightnessModel;
}
