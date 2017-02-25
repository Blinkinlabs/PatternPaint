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
        paint(frameData, pt);
        drawing = true;
    }
}

void ColorpickerInstrument::mouseMoveEvent(QMouseEvent *,
                                           const QImage &frameData,
                                           const QPoint &pt)
{
    if(!drawing) {
        updatePreview(frameData, pt);
        return;
    }

    paint(frameData, pt);
}

void ColorpickerInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &, const QImage &, const QPoint &)
{
    drawing = false;
}

void ColorpickerInstrument::paint(const QImage &frameData, const QPoint &point)
{
    if (point.x() < 0 || point.y() < 0
        || point.x() > frameData.width()
        || point.y() > frameData.height())
        return;

    QRgb pixel(frameData.pixel(point));
    QColor getColor(pixel);
    emit pickedColor(getColor);
}
