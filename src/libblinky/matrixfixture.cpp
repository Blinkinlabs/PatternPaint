#include "matrixfixture.h"

#include <limits>

MatrixFixture::MatrixFixture(QSize size, MatrixMode matrixMode, QObject *parent) :
    Fixture(parent),
    matrixMode(matrixMode)
{
    setSize(size);

    switch(matrixMode) {
    case MODE_ZIGZAG:
        setName("Matrix-Zigzag");
        break;
    case MODE_ROWS:
    default:
        setName("Matrix-Rows");
        break;
    }
}

MatrixFixture::~MatrixFixture()
{

}

QSize MatrixFixture::getSize() const
{
    return size;
}

void MatrixFixture::setSize(QSize newSize)
{
    size = newSize;

    QList<QPoint> newLocations;

    for (int x = 0; x < size.width(); x++) {
        for (int y = 0; y < size.height(); y++) {
            QPoint point;

            switch(matrixMode) {
            case MODE_ZIGZAG:
                point = QPoint(x, x%2 ? size.height()-1-y : y);
                break;
            case MODE_ROWS:
                point = QPoint(x, y);
                break;
            }

            newLocations.append(point);
        }
    }

    setLocations(newLocations);
}

MatrixFixture::MatrixMode MatrixFixture::getMode() const
{
    return matrixMode;
}


