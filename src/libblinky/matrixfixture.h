#ifndef MATRIXFIXTURE_H
#define MATRIXFIXTURE_H

#include "libblinkyglobal.h"

#include <QObject>

#include "fixture.h"

class LIBBLINKY_EXPORT MatrixFixture : public Fixture
{
    Q_OBJECT

public:
    enum class Mode {
        MODE_ZIGZAG,
        MODE_ROWS,
        MODE_COLS
    };

    MatrixFixture(QSize size, Mode matrixMode,
                  QObject *parent = 0);


    Fixture::Type getType() const override;

    QSize getSize() const;
    void setSize(QSize newSize);

    MatrixFixture::Mode getMode() const;
    void setMode(MatrixFixture::Mode newMode);

    friend QDataStream &operator<<(QDataStream &out, const MatrixFixture &fixture);
    friend QDataStream &operator>>(QDataStream &in, MatrixFixture &fixture);

private:
    QSize size;
    Mode matrixMode;

    void recalculateLocations();
};

LIBBLINKY_EXPORT QDataStream &operator<<(QDataStream &out, const MatrixFixture &fixture);
LIBBLINKY_EXPORT QDataStream &operator>>(QDataStream &in, MatrixFixture &fixture);

LIBBLINKY_EXPORT QDataStream &operator<<(QDataStream &out, const MatrixFixture::Mode &mode);
LIBBLINKY_EXPORT QDataStream &operator>>(QDataStream &in, MatrixFixture::Mode &mode);

#endif // MATRIXFIXTURE_H
