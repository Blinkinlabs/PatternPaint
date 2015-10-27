#include "fixture.h"


MatrixFixture::MatrixFixture(QSize size) :
    size(size)
{
}

QList<QColor> MatrixFixture::getColorStreamForFrame(const QImage frame)
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

int MatrixFixture::getLedCount()
{
    return size.height()*size.width();
}
