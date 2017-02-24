#include "colorpickerinstrument.h"

ColorpickerInstrument::ColorpickerInstrument(InstrumentConfiguration *instrumentConfiguration, QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor_pipette.png",
                       instrumentConfiguration,
                       parent)
{
    drawing = false;
}

void ColorpickerInstrument::mousePressEvent(QMouseEvent *event,
                                            const QImage &frameData,
                                            const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        startPoint = endPoint = pt;
        paint(frameData);
        drawing = true;
    }
}

void ColorpickerInstrument::mouseMoveEvent(QMouseEvent *,
                                           const QImage &frameData,
                                           const QPoint &pt)
{
    if(!drawing)
        return;

    startPoint = endPoint = pt;
    paint(frameData);
}

void ColorpickerInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &, const QImage &, const QPoint &)
{
    drawing = false;
}

void ColorpickerInstrument::paint(const QImage &frameData)
{
    if (startPoint.x() < 0 || startPoint.y() < 0
        || startPoint.x() > frameData.width()
        || startPoint.y() > frameData.height())
        return;

    QRgb pixel(frameData.pixel(startPoint));
    QColor getColor(pixel);
    emit pickedColor(getColor);
}
