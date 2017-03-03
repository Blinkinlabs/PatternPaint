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

void PencilInstrument::mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &point)
{
    if (event->button() == Qt::LeftButton) {
        preview = QImage(frameData.size(),
                             QImage::Format_ARGB32_Premultiplied);
        preview.fill(QColor(0, 0, 0, 0));

        startPoint = point;
        paint(point);
        drawing = true;
    }
}

void PencilInstrument::mouseMoveEvent(QMouseEvent *, const QImage &frameData, const QPoint &point)
{
    // If we aren't drawing, we're in preview mode- clear the frame before doing anything else.
    if (!drawing) {
        updatePreview(frameData, point);
        return;
    }

    paint(point);
    startPoint = point;
}

void PencilInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QImage &frameData, const QPoint &point)
{
    if (!drawing)
        return;

    editor.applyInstrument(preview);
    drawing = false;

    updatePreview(frameData, point);
}

void PencilInstrument::updatePreview(const QImage &frameData, QPoint point)
{
    if(preview.size() != frameData.size())
        preview = QImage(frameData.size(),
                         QImage::Format_ARGB32_Premultiplied);
    preview.fill(QColor(0, 0, 0, 0));

    QPainter painter(&preview);
    QColor previewColor;

    if(frameData.pixelColor(point).lightness() < 128) {
        previewColor = QColor(128+20,128+20,128+20,128);
    }
    else {
        previewColor = QColor(128-20,128-20,128-20,128);
    }

    painter.setPen(QPen(previewColor,
                        instrumentConfiguration->getPenSize(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPoint(point);

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
