#include "pencilinstrument.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

PencilInstrument::PencilInstrument(InstrumentConfiguration *instrumentConfiguration, QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor.png",
                       instrumentConfiguration,
                       parent)
{
    drawing = false;
}

void PencilInstrument::mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        preview = QImage(frameData.size(),
                             QImage::Format_ARGB32_Premultiplied);
        preview.fill(QColor(0, 0, 0, 0));

        startPoint = pt;
        endPoint = pt;

        paint();
        drawing = true;
    }
}

void PencilInstrument::mouseMoveEvent(QMouseEvent *event, const QImage &frameData, const QPoint &pt)
{
    if (!drawing)
        return;

    endPoint = pt;
    if (event->buttons() & Qt::LeftButton)
        paint();
    startPoint = pt;
}

void PencilInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QImage &frameData, const QPoint &)
{
    editor.applyInstrument(preview);
    drawing = false;
}

QCursor PencilInstrument::cursor() const
{
    // TODO: Pull this into the resource file, to keep consistancy across platforms
    return Qt::CrossCursor;
}

void PencilInstrument::paint()
{
    QPainter painter(&preview);

    painter.setPen(QPen(instrumentConfiguration->getToolColor(),
                        instrumentConfiguration->getPenSize(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if (startPoint != endPoint)
        painter.drawLine(startPoint, endPoint);

    if (startPoint == endPoint)
        painter.drawPoint(startPoint);
}
