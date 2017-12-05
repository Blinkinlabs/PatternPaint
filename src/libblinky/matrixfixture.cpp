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

    if (matrixMode == MODE_ROWS) {
        for (int x = 0; x < size.width(); x++) {
            for (int y = 0; y < size.height(); y++) {
                newLocations.append(QPoint(x, y));
            }
        }
    }

    else if(matrixMode == MODE_COLS) {
        for (int y = 0; y < size.height(); y++) {
            for (int x = 0; x < size.width(); x++) {
                newLocations.append(QPoint(x, y));
            }
        }
    }

    else if(matrixMode == MODE_ZIGZAG) {
        for (int x = 0; x < size.width(); x++) {
            for (int y = 0; y < size.height(); y++) {
                newLocations.append(QPoint(x, x%2 ? size.height()-1-y : y));
            }
         }
     }


    setLocations(newLocations);
}
