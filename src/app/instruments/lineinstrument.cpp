#include "lineinstrument.h"

#include <QPen>
#include <QPainter>
#include <QImage>
#include <QDebug>

LineInstrument::LineInstrument(InstrumentConfiguration *instrumentConfiguration, QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor.png",
                       instrumentConfiguration,
                       parent)
{
    cursor = QCursor(Qt::CrossCursor);
    drawing = false;
}

bool LineInstrument::hasPreview() const
{
    return true;
}

void LineInstrument::mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        preview = QImage(frameData.size(),
                             QImage::Format_ARGB32_Premultiplied);
        preview.fill(QColor(0, 0, 0, 0));

        firstPoint = pt;
        paint(pt);
        drawing = true;
    }
}

void LineInstrument::mouseMoveEvent(QMouseEvent *, const QImage &frameData, const QPoint &pt)
{
    // If we aren't drawing, we're in preview mode- clear the frame before doing anything else.
    if (!drawing) {
        if(preview.size() != frameData.size())
            preview = QImage(frameData.size(),
                             QImage::Format_ARGB32_Premultiplied);

        preview.fill(QColor(0,0,0,0));
        firstPoint = pt;
    }

    preview.fill(QColor(0, 0, 0, 0));
    paint(pt);
}

void LineInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QImage &, const QPoint &)
{
    editor.applyInstrument(preview);
    drawing = false;
}

void LineInstrument::paint(const QPoint &newPoint)
{
    QPainter painter(&preview);

    painter.setPen(QPen(instrumentConfiguration->getToolColor(),
                        instrumentConfiguration->getPenSize(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if (firstPoint != newPoint)
        painter.drawLine(firstPoint, newPoint);

    if (firstPoint == newPoint)
        painter.drawPoint(newPoint);
}
