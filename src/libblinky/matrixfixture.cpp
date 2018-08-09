#include "matrixfixture.h"

#include <limits>

MatrixFixture::MatrixFixture(QSize size, Mode matrixMode, QObject *parent) :
    Fixture(parent)
{
    switch(matrixMode) {
    case Mode::MODE_ZIGZAG:
        setName("Matrix-Zigzag");
        break;
    case Mode::MODE_COLS:
        setName("Matrix-Cols");
        break;
    case Mode::MODE_ROWS:
    default:
        setName("Matrix-Rows");
        break;
    }

    setMode(matrixMode);
    setSize(size);
}


Fixture::Type MatrixFixture::getType() const
{
    return Fixture::Type::MATRIX;
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

MatrixFixture::Mode MatrixFixture::getMode() const
{
    return matrixMode;
}

void MatrixFixture::setMode(MatrixFixture::Mode newMatrixMode)
{
    matrixMode = newMatrixMode;
    recalculateLocations();
}

void MatrixFixture::recalculateLocations()
{
    QList<QPoint> newLocations;

    if (matrixMode == Mode::MODE_ROWS) {
        for (int x = 0; x < size.width(); x++) {
            for (int y = 0; y < size.height(); y++) {
                newLocations.append(QPoint(x, y));
            }
        }
    }

    else if(matrixMode == Mode::MODE_COLS) {
        for (int y = 0; y < size.height(); y++) {
            for (int x = 0; x < size.width(); x++) {
                newLocations.append(QPoint(x, y));
            }
        }
    }

    else if(matrixMode == Mode::MODE_ZIGZAG) {
        for (int x = 0; x < size.width(); x++) {
            for (int y = 0; y < size.height(); y++) {
                newLocations.append(QPoint(x, x%2 ? size.height()-1-y : y));
            }
         }
     }


    setLocations(newLocations);
}

QDataStream &operator <<(QDataStream &out, const MatrixFixture &fixture)
{
    out << (qint32)1;   // Current version

    out << fixture.getName();
    out << fixture.getColorMode();
    //out << fixture.brightnessModel;
    out << fixture.size;
    out << fixture.matrixMode;

    return out;
}

QDataStream &operator >>(QDataStream &in, MatrixFixture &fixture)
{
    qint32 version;
    in >> version;
    if(version == 1) {
        QString name;
        in >> name;
        fixture.setName(name);

        ColorMode colorMode;
        in >> colorMode;
        fixture.setColorMode(colorMode);

        //in >> fixture.brightnessModel;

        in >> fixture.size;
        in >> fixture.matrixMode;

        fixture.recalculateLocations();
    }
    else {
        // Mark the data stream as corrupted, so that the array read will
        // unwind correctly.
        in.setStatus(QDataStream::ReadCorruptData);
    }

    return in;
}

QDataStream &operator<<(QDataStream &out, const MatrixFixture::Mode &mode)
{
    out << static_cast<qint32>(mode);

    return out;
}

QDataStream &operator>>(QDataStream &in, MatrixFixture::Mode &mode)
{
    qint32 value;

    in >> value;
    mode = static_cast<MatrixFixture::Mode>(value);

    return in;
}
