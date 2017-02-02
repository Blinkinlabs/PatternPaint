#include "pencilinstrument.h"

#include "frameeditor.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

PencilInstrument::PencilInstrument(QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor.png", parent)
{
    drawing = false;
}

void PencilInstrument::mousePressEvent(QMouseEvent *event, FrameEditor &editor, const QPoint &pt)
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

void PencilInstrument::mouseMoveEvent(QMouseEvent *event, FrameEditor &editor, const QPoint &pt)
{
    if (!drawing)
        return;

    mEndPoint = pt;
    if (event->buttons() & Qt::LeftButton)
        paint(editor);
    mStartPoint = pt;
}

void PencilInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &editor, const QPoint &)
{
    editor.applyInstrument(toolPreview);
    drawing = false;
}

QCursor PencilInstrument::cursor() const
{
    // TODO: Pull this into the resource file, to keep consistancy across platforms
    return Qt::CrossCursor;
}

void PencilInstrument::paint(FrameEditor &editor)
{
    QPainter painter(&toolPreview);

    painter.setPen(QPen(editor.getPrimaryColor(),
                        editor.getPenSize(),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if (mStartPoint != mEndPoint)
        painter.drawLine(mStartPoint, mEndPoint);

    if (mStartPoint == mEndPoint)
        painter.drawPoint(mStartPoint);
}
