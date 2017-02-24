#include "pencilinstrument.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

PencilInstrument::PencilInstrument(InstrumentConfiguration *instrumentConfiguration, QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor.png",
                       instrumentConfiguration,
                       parent)
{
    cursor = QCursor(Qt::CrossCursor);
    drawing = false;
}

bool PencilInstrument::hasPreview() const
{
    return true;
}

void PencilInstrument::mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        preview = QImage(frameData.size(),
                             QImage::Format_ARGB32_Premultiplied);
        preview.fill(QColor(0, 0, 0, 0));

        startPoint = pt;
        paint(pt);
        drawing = true;
    }
}

void PencilInstrument::mouseMoveEvent(QMouseEvent *, const QImage &frameData, const QPoint &pt)
{
    // If we aren't drawing, we're in preview mode- clear the frame before doing anything else.
    if (!drawing) {
        if(preview.size() != frameData.size())
            preview = QImage(frameData.size(),
                             QImage::Format_ARGB32_Premultiplied);

        preview.fill(QColor(0,0,0,0));
        startPoint = pt;
    }

    paint(pt);
    startPoint = pt;
}

void PencilInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QImage &, const QPoint &)
{
    editor.applyInstrument(preview);
    drawing = false;
}

void PencilInstrument::paint(const QPoint &newPoint)
{
    QPainter painter(&preview);

    painter.setPen(QPen(instrumentConfiguration->getToolColor(),
                        instrumentConfiguration->getPenSize(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if (startPoint != newPoint)
        painter.drawLine(startPoint, newPoint);

    if (startPoint == newPoint)
        painter.drawPoint(startPoint);
}
