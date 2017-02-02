#include "sprayinstrument.h"

#include "frameeditor.h"

#include <QPen>
#include <QPainter>
#include <math.h>

SprayInstrument::SprayInstrument(QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor_spray.png", parent)
{
    drawing = false;
}

void SprayInstrument::mousePressEvent(QMouseEvent *event, FrameEditor &editor, const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        toolPreview = QImage(editor.getPatternAsImage().width(),
                             editor.getPatternAsImage().height(),
                             QImage::Format_ARGB32_Premultiplied);
        toolPreview.fill(QColor(0, 0, 0, 0));
        drawing = true;

        mStartPoint = mEndPoint = pt;
        paint(editor);
    }
}

void SprayInstrument::mouseMoveEvent(QMouseEvent *, FrameEditor &editor, const QPoint &pt)
{
    if (!drawing)
        return;

    mEndPoint = pt;
    paint(editor);
    mStartPoint = pt;
}

void SprayInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QPoint &)
{
    editor.applyInstrument(toolPreview);
    drawing = false;
}

void SprayInstrument::paint(FrameEditor &editor)
{
    QPainter painter(&toolPreview);

    painter.setPen(QPen(editor.getPrimaryColor(), editor.getPenSize(), Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));

    int x;
    int y;
    for (int i(0); i < 12; i++) {
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 3:
            x = (qrand() % 5 - 2)
                * sqrt(editor.getPenSize());
            y = (qrand() % 5 - 2)
                * sqrt(editor.getPenSize());
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            x = (qrand() % 10 - 4)
                * sqrt(editor.getPenSize());
            y = (qrand() % 10 - 4)
                * sqrt(editor.getPenSize());
            break;
        case 8:
        case 9:
        case 10:
        case 11:
            x = (qrand() % 15 - 7)
                * sqrt(editor.getPenSize());
            y = (qrand() % 15 - 7)
                * sqrt(editor.getPenSize());
            break;
        default:
            return;
        }

        painter.drawPoint(mEndPoint.x() + x, mEndPoint.y() + y);
    }
}
