#include "matrixfixture.h"

#include <limits>

MatrixFixture::MatrixFixture(QSize size, MatrixMode matrixMode, QObject *parent) :
    Fixture(parent)
{
    switch(matrixMode) {
    case MODE_ZIGZAG:
        setName("Matrix-Zigzag");
        break;
    case MODE_COLS:
        setName("Matrix-Cols");
        break;
    case MODE_ROWS:
    default:
        setName("Matrix-Rows");
        break;
    }

    setMode(matrixMode);
    setSize(size);
}

QSize MatrixFixture::getSize() const
{
    return size;
}

void MatrixFixture::setSize(QSize newSize)
{
    size = newSize;
    recalculateLocations();
}

MatrixFixture::MatrixMode MatrixFixture::getMode() const
{
    return matrixMode;
}

void MatrixFixture::setMode(MatrixFixture::MatrixMode newMatrixMode)
{
    matrixMode = newMatrixMode;
    recalculateLocations();
}

void MatrixFixture::recalculateLocations()
{
    QList<QPoint> newLocations;

    for (int x = 0; x < size.width(); x++) {
        for (int y = 0; y < size.height(); y++) {
            QPoint point;

            switch(matrixMode) {
            case MODE_ZIGZAG:
                point = QPoint(x, x%2 ? size.height()-1-y : y);
                break;
            case MODE_COLS:
                point = QPoint(y, x);
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
