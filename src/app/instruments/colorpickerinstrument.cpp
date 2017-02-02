#include "colorpickerinstrument.h"

#include "frameeditor.h"

ColorpickerInstrument::ColorpickerInstrument(QObject *parent) :
    AbstractInstrument(":/instruments/images/instruments-icons/cursor_pipette.png", parent)
{
    drawing = false;
}

void ColorpickerInstrument::mousePressEvent(QMouseEvent *event, FrameEditor &editor,
                                            const QPoint &pt)
{
    if (event->button() == Qt::LeftButton) {
        mStartPoint = mEndPoint = pt;
        paint(editor);
        drawing = true;
    }
}

void ColorpickerInstrument::mouseMoveEvent(QMouseEvent *, FrameEditor &editor, const QPoint &pt)
{
    if(!drawing)
        return;

    mStartPoint = mEndPoint = pt;
    paint(editor);
}

void ColorpickerInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor &, const QPoint &)
{
    drawing = false;
}

void ColorpickerInstrument::paint(FrameEditor &editor)
{
    bool inArea = true;

    if (mStartPoint.x() < 0 || mStartPoint.y() < 0
        || mStartPoint.x() > editor.width()
        || mStartPoint.y() > editor.height())
        inArea = false;

    if (inArea) {
        QRgb pixel(editor.getPatternAsImage().pixel(mStartPoint));
        QColor getColor(pixel);
        emit pickedColor(getColor);
    }
}
