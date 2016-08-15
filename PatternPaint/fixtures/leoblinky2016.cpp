#include "leoblinky2016.h"

LeoBlinky2016::LeoBlinky2016(
        QSize size, ColorMode colorMode, BrightnessModel *brightnessModel, QObject *parent) :
        Fixture(parent),
        colormode(colorMode),
        brightnessModel(brightnessModel)
{
    setSize(size);
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
    // What to do with this? scale the points to the size?

    size = newSize;

    locations.clear();

    extents.setLeft(std::numeric_limits<int>::max());
    extents.setRight(std::numeric_limits<int>::min());
    extents.setTop(std::numeric_limits<int>::max());
    extents.setBottom(std::numeric_limits<int>::min());

    const int POINTS_COUNT = 56;
    double points[POINTS_COUNT*2] = {
        11.4	,	0	, //	LED1_1
        22.9	,	0	, //	LED1_2
        11.4	,	-19.8	, //	LED1_3
        5.7	,	-9.9	, //	LED1_4
        -5.7	,	-29.8	, //	LED1_5
        -5.7	,	-10	, //	LED1_6
        -22.9	,	-19.8	, //	LED1_7
        -11.5	,	0	, //	LED1_8
        -22.9	,	0	, //	LED1_9
        -11.5	,	19.8	, //	LED1_10
        -5.7	,	9.9	, //	LED1_11
        5.7	,	29.8	, //	LED1_12
        5.7	,	9.9	, //	LED1_49
        22.9	,	19.8	, //	LED1_50

        28.6	,	9.9	, //	LED1_13
        28.6	,	-9.9	, //	LED1_14
        22.9	,	-19.8	, //	LED1_15
        5.7	,	-29.8	, //	LED1_16
        -11.5	,	-39.7	, //	LED1_17
        -11.5	,	-19.8	, //	LED1_18
        -28.6	,	-29.8	, //	LED1_19
        -28.6	,	-9.9	, //	LED1_20
        -28.6	,	9.9	, //	LED1_21
        -22.9	,	19.8	, //	LED1_22
        -5.7	,	29.8	, //	LED1_23
        11.4	,	39.7	, //	LED1_24
        11.5	,	19.8	, //	LED1_51
        28.6	,	29.8	, //	LED1_52

        40.1	,	9.9	, //	LED1_25
        40.1	,	-9.9	, //	LED1_26
        28.6	,	-29.8	, //	LED1_27
        11.4	,	-39.7	, //	LED1_28
        -11.12	,	-48.74	, //	LED1_29
        -22.9	,	-39.7	, //	LED1_30
        -45.05	,	-21.69	, //	LED1_31
        -40.1	,	-9.9	, //	LED1_32
        -40.1	,	9.9	, //	LED1_33
        -28.6	,	29.8	, //	LED1_34
        -11.5	,	39.7	, //	LED1_35
        11.13	,	48.74	, //	LED1_36
        22.9	,	39.7	, //	LED1_53
        45.05	,	21.69	, //	LED1_54

        45.8	,	0	, //	LED1_37
        45.05	,	-21.69	, //	LED1_38
        31.17	,	-39.09	, //	LED1_39
        22.9	,	-39.7	, //	LED1_40
        11.12	,	-48.74	, //	LED1_41
        -31.17	,	-39.09	, //	LED1_42
        -50	,	0	, //	LED1_43
        -45.8	,	0	, //	LED1_44
        -45.05	,	21.69	, //	LED1_45
        -31.17	,	39.1	, //	LED1_46
        -22.9	,	39.7	, //	LED1_47
        -11.12	,	48.74	, //	LED1_48
        31.17	,	39.1	, //	LED1_55
        50	,	0	, //	LED1_56

    };

    for(int i = POINTS_COUNT-1; i > -1; i--) {
        QPoint point (50+points[i*2], 50-points[i*2+1]);
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
