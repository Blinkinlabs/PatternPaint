#include "leoblinky2016.h"

LeoBlinky2016::LeoBlinky2016(
        ColorMode colorMode, BrightnessModel *brightnessModel, QObject *parent) :
        Fixture(parent),
        colormode(colorMode),
        brightnessModel(brightnessModel)
{
    setSize(QSize());
}

LeoBlinky2016::~LeoBlinky2016()
{
    if(brightnessModel != NULL)
        free(brightnessModel);
}

QString LeoBlinky2016::getName() const
{
    return QString("Matrix");
}

QList<QColor> LeoBlinky2016::getColorStreamForFrame(const QImage frame) const
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

QList<QPoint> LeoBlinky2016::getOutputLocations() const
{
    return locations;
}

QRect LeoBlinky2016::getExtents() const
{
    return extents;
}

int LeoBlinky2016::getLedCount() const
{
    return locations.length();
}

QSize LeoBlinky2016::getSize() const
{
    return size;
}

void LeoBlinky2016::setSize(QSize newSize)
{
    Q_UNUSED(newSize);

    // Fix the size to 100x100
    size = QSize(100,100);

    locations.clear();

    extents.setLeft(std::numeric_limits<int>::max());
    extents.setRight(std::numeric_limits<int>::min());
    extents.setTop(std::numeric_limits<int>::max());
    extents.setBottom(std::numeric_limits<int>::min());

    const int POINTS_COUNT = 56;
    double points[POINTS_COUNT*2] = {
        95.00,	50.00,  //	LED1_1 xx
        78.05,	14.81,  //	LED1_2
        39.99,	6.13,   //	LED1_3
        29.39,	14.27,  //	LED1_4
        21.95,	14.81,  //	LED1_5
        9.46,	30.48,  //	LED1_6
        8.78,	50.00,  //	LED1_7
        5.00,	50.00,  //	LED1_8
        21.95,	85.18,  //	LED1_9
        60.01,	93.87,  //	LED1_10
        70.61,	85.73,  //	LED1_11
        78.05,	85.18,  //	LED1_12
        90.54,	69.52,  //	LED1_49
        91.22,	50.00,  //	LED1_50
        90.54,	30.48,  //	LED1_13 xx
        70.61,	14.27,  //	LED1_14
        60.02,	6.13,   //	LED1_15
        39.65,	14.27,  //	LED1_16
        24.26,	23.18,  //	LED1_17
        13.91,	41.09,  //	LED1_18
        13.91,	58.91,  //	LED1_19
        9.46,	69.52,  //	LED1_20
        29.39,	85.73,  //	LED1_21
        39.99,	93.87,  //	LED1_22
        60.26,	85.73,  //	LED1_23
        75.74,	76.82,  //	LED1_24
        86.09,	58.91,  //	LED1_51
        86.09,	41.09,  //	LED1_52
        75.74,	23.18,  //	LED1_25 xx
        60.35,	32.18,  //	LED1_26
        60.26,	14.27,  //	LED1_27
        44.87,	23.18,  //	LED1_28
        29.39,	32.18,  //	LED1_29
        24.26,	41.09,  //	LED1_30
        24.26,	58.91,  //	LED1_31
        24.26,	76.82,  //	LED1_32
        39.65,	67.82,  //	LED1_33
        39.65,	85.73,  //	LED1_34
        55.13,	76.82,  //	LED1_35
        70.61,	67.82,  //	LED1_36
        75.74,	58.91,  //	LED1_53
        75.74,	41.09,  //	LED1_54
        70.61,	32.18,  //	LED1_37 xxx
        55.13,	41.09,  //	LED1_38
        55.13,	23.18,  //	LED1_39
        44.87,	41.09,  //	LED1_40
        39.65,	32.18,  //	LED1_41
        29.39,	50.00,  //	LED1_42
        39.65,	50.00,  //	LED1_43
        29.39,	67.82,  //	LED1_44
        44.87,	59.00,  //	LED1_45
        44.87,	76.82,  //	LED1_46
        55.13,	58.91,  //	LED1_47
        60.26,	67.82,  //	LED1_48
        70.61,	50.00,  //	LED1_55
        60.26,	50.00,  //	LED1_56
    };

    for(int i = 0; i < POINTS_COUNT; i++) {
//        QPoint point (50+points[i*2], 50-points[i*2+1]);
        QPoint point (points[i*2], points[i*2+1]);
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

ColorMode LeoBlinky2016::getColorMode() const
{
    return colormode;
}

void LeoBlinky2016::setColorMode(ColorMode newColorMode)
{
    colormode = newColorMode;
}

BrightnessModel *LeoBlinky2016::getBrightnessModel() const
{
    return brightnessModel;
}

void LeoBlinky2016::setBrightnessModel(BrightnessModel *newBrightnessModel)
{
    brightnessModel = newBrightnessModel;
}
