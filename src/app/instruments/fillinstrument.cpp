#include "fillinstrument.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

FillInstrument::FillInstrument(InstrumentConfiguration *instrumentConfiguration, QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor_fill.png",
                       instrumentConfiguration,
                       parent)
{
    drawing = false;
}

void FillInstrument::mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &point)
{
    if (event->button() == Qt::LeftButton) {
        drawing = true;
        preview = frameData;

        paint(point);
    }
}

void FillInstrument::mouseMoveEvent(QMouseEvent *, const QImage &frameData, const QPoint &point)
{
    if(!drawing) {
        updatePreview(frameData, point);
        return;
    }
}

void FillInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QImage &, const QPoint &)
{
    editor.applyInstrument(preview);
    drawing = false;
}

void FillInstrument::paint(const QPoint &point)
{
    QColor switchColor = instrumentConfiguration->getToolColor();
    QRgb pixel(preview.pixel(point));

    QColor oldColor(pixel);

    if (switchColor.rgb() != oldColor.rgb())
        fill(point, switchColor.rgb(), pixel, preview);
}

QList<QPoint> neighbors(const QPoint &point, const QImage &img)
{
    QList<QPoint> res;
    if (point.x() > 0) res << QPoint(point.x()-1, point.y());
    if (point.y() > 0) res << QPoint(point.x(), point.y()-1);
    if (point.x() < img.width() - 1) res << QPoint(point.x()+1, point.y());
    if (point.y() < img.height() - 1) res << QPoint(point.x(), point.y()+1);
    return res;
}

void FillInstrument::fill(const QPoint &point, QRgb newColor, QRgb oldColor, QImage &pattern)
{
    if (point.x() >= pattern.width() || point.y() >= pattern.height())
        return;

    if (pattern.pixel(point) != oldColor)
        return;
    pattern.setPixel(point, newColor);

    for (const QPoint &p : neighbors(point, pattern))
        if (pattern.pixel(p) == oldColor) fill(p, newColor, oldColor, pattern);
}
