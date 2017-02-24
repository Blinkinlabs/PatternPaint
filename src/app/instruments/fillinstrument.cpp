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

void FillInstrument::mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        drawing = true;
        preview = frameData;

        paint(pt);
    }
}

void FillInstrument::mouseMoveEvent(QMouseEvent *, const QImage &, const QPoint &)
{
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

QList<QPoint> neighbors(const QPoint &pt, const QImage &img)
{
    QList<QPoint> res;
    if (pt.x() > 0) res << QPoint(pt.x()-1, pt.y());
    if (pt.y() > 0) res << QPoint(pt.x(), pt.y()-1);
    if (pt.x() < img.width() - 1) res << QPoint(pt.x()+1, pt.y());
    if (pt.y() < img.height() - 1) res << QPoint(pt.x(), pt.y()+1);
    return res;
}

void FillInstrument::fill(const QPoint &pt, QRgb newColor, QRgb oldColor, QImage &pattern)
{
    if (pt.x() >= pattern.width() || pt.y() >= pattern.height())
        return;

    if (pattern.pixel(pt) != oldColor)
        return;
    pattern.setPixel(pt, newColor);

    foreach (const QPoint &p, neighbors(pt, pattern))
        if (pattern.pixel(p) == oldColor) fill(p, newColor, oldColor, pattern);
}
