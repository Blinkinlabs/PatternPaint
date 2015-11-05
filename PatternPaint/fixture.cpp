#include "fixture.h"

Fixture::Fixture(QObject *parent) :
    QObject(parent)
{
}

MatrixFixture::MatrixFixture(QSize size, QObject *parent) :
    size(size),
    Fixture(parent)
{
}

QList<QColor> MatrixFixture::getColorStreamForFrame(const QImage frame) const
{
    QList<QColor> colorStream;

    if(frame.isNull()) {
        return colorStream;
    }

    for(int x = 0; x <size.width(); x++) {
        for(int y = 0; y < size.height(); y++) {
            colorStream.append(frame.pixel(x,x%2?size.height()-1-y:y));
        }
    }

    return colorStream;
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
}



