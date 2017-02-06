#ifndef MATRIXFIXTURE_H
#define MATRIXFIXTURE_H

#include <QObject>

#include "fixture.h"

class MatrixFixture : public Fixture
{
    Q_OBJECT

public:
    enum MatrixMode {
        MODE_ZIGZAG,
        MODE_ROWS
    };

    MatrixFixture(QSize size, MatrixMode matrixMode,
                  ColorMode colorMode, BrightnessModel *brightnessModel,
                  QObject *parent = 0);

    ~MatrixFixture();

    QString getName() const;

    QList<QColor> getColorStreamForFrame(const QImage frame) const;

    QList<QPoint> getOutputLocations() const;

    QRect getExtents() const;

    int getLedCount() const;

    QSize getSize() const;
    void setSize(QSize newSize);

    MatrixFixture::MatrixMode getMode() const;

    ColorMode getColorMode() const;
    void setColorMode(ColorMode newColorMode);

    BrightnessModel *getBrightnessModel() const;
    void setBrightnessModel(BrightnessModel *newBrightnessModel);

private:
    QSize size;
    MatrixMode matrixMode;
    ColorMode colorMode;
    BrightnessModel *brightnessModel;

    QList<QPoint> locations;
    QRect extents;
};

#endif // MATRIXFIXTURE_H
