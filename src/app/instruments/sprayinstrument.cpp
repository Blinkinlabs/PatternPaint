#include "sprayinstrument.h"

#include <QPen>
#include <QPainter>
#include <math.h>

SprayInstrument::SprayInstrument(InstrumentConfiguration *instrumentConfiguration, QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor_spray.png",
                       instrumentConfiguration,
                       parent)
{
    drawing = false;
}

void SprayInstrument::mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        preview = QImage(frameData.size(),
                             QImage::Format_ARGB32_Premultiplied);
        preview.fill(QColor(0, 0, 0, 0));
        drawing = true;

        paint(pt);
    }
}

void SprayInstrument::mouseMoveEvent(QMouseEvent *, const QImage &, const QPoint &pt)
{
    if (!drawing)
        return;

    paint(pt);
}

void SprayInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QImage &, const QPoint &)
{
    editor.applyInstrument(preview);
    drawing = false;
}

void SprayInstrument::paint(const QPoint &newPoint)
{
    QPainter painter(&preview);

    painter.setPen(QPen(instrumentConfiguration->getToolColor(),
                        instrumentConfiguration->getPenSize(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    int x;
    int y;
    for (int i(0); i < 12; i++) {
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 3:
            x = (qrand() % 5 - 2)
                * sqrt(instrumentConfiguration->getPenSize());
            y = (qrand() % 5 - 2)
                * sqrt(instrumentConfiguration->getPenSize());
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            x = (qrand() % 10 - 4)
                * sqrt(instrumentConfiguration->getPenSize());
            y = (qrand() % 10 - 4)
                * sqrt(instrumentConfiguration->getPenSize());
            break;
        case 8:
        case 9:
        case 10:
        case 11:
            x = (qrand() % 15 - 7)
                * sqrt(instrumentConfiguration->getPenSize());
            y = (qrand() % 15 - 7)
                * sqrt(instrumentConfiguration->getPenSize());
            break;
        default:
            return;
        }

        painter.drawPoint(newPoint.x() + x, newPoint.y() + y);
    }
}
