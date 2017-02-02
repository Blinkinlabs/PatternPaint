#include "lineinstrument.h"

#include "frameeditor.h"

#include <QPen>
#include <QPainter>
#include <QImage>
#include <QDebug>

LineInstrument::LineInstrument(QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor.png", parent)
{
    drawing = false;
}

void LineInstrument::mousePressEvent(QMouseEvent *event, FrameEditor &editor, const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        toolPreview = QImage(editor.getPatternAsImage().width(),
                             editor.getPatternAsImage().height(),
                             QImage::Format_ARGB32_Premultiplied);
        toolPreview.fill(QColor(0, 0, 0, 0));

        mStartPoint = mEndPoint = pt;
        paint(editor);
        drawing = true;
    }
}

void LineInstrument::mouseMoveEvent(QMouseEvent *, FrameEditor &editor, const QPoint &pt)
{
    if (!drawing)
        return;

    mEndPoint = pt;
    toolPreview.fill(QColor(0, 0, 0, 0));
    paint(editor);
}

void LineInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QPoint &)
{
    editor.applyInstrument(toolPreview);
    drawing = false;
}

QCursor LineInstrument::cursor() const
{
    // TODO: Pull this into the resource file, to keep consistancy across platforms
    return Qt::CrossCursor;
}

void LineInstrument::paint(FrameEditor &editor)
{
    QPainter painter(&toolPreview);

    painter.setPen(QPen(editor.getPrimaryColor(), editor.getPenSize(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if (mStartPoint != mEndPoint) {
#if defined(LINE_INSTRUMENT_WORKAROUND)
        // Workaround for bug in Qt: see https://github.com/Blinkinlabs/PatternPaint/issues/66
        QPoint newStartPoint(mStartPoint);
        QPoint newEndPoint(mEndPoint);

        int deltaX = (mEndPoint-mStartPoint).x();
        int deltaY = (mEndPoint-mStartPoint).y();

        qDebug() << "deltaX=" << deltaX << " deltaY=" << deltaY;

        if ((deltaX > 0 && deltaY < 0) || (deltaX < 0 && deltaY > 0)) {
            if (abs(deltaX) >= abs(deltaY)) {
                qDebug() << "A";
                newStartPoint += QPoint(0, 1);
                newEndPoint += QPoint(0, 1);
            } else {
                qDebug() << "B";
                newStartPoint += QPoint(1, 0);
                newEndPoint += QPoint(1, 0);
            }
        }

        painter.drawLine(newStartPoint, newEndPoint);
#else
        painter.drawLine(mStartPoint, mEndPoint);
#endif
    }

    if (mStartPoint == mEndPoint)
        painter.drawPoint(mStartPoint);
}
