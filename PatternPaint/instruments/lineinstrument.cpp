/*
 * This source file is part of EasyPaint.
 *
 * Copyright (c) 2012 EasyPaint <https://github.com/Gr1N/EasyPaint>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "lineinstrument.h"
#include "frameeditor.h"

#include <QPen>
#include <QPainter>
#include <QImage>
#include <QDebug>

LineInstrument::LineInstrument(QObject *parent) :
    AbstractInstrument(parent)
{
    drawing = false;
}

void LineInstrument::mousePressEvent(QMouseEvent *event, FrameEditor& editor, const QPoint& pt)
{
    if(event->button() == Qt::LeftButton)
    {
        toolPreview = QImage(editor.getPatternAsImage().width(),
                             editor.getPatternAsImage().height(),
                             QImage::Format_ARGB32_Premultiplied);
        toolPreview.fill(QColor(0,0,0,0));

        mStartPoint = mEndPoint = pt;
        paint(editor);
        drawing = true;
    }
}

void LineInstrument::mouseMoveEvent(QMouseEvent*, FrameEditor& editor, const QPoint& pt)
{
    if(drawing) {
        mEndPoint = pt;
        toolPreview.fill(QColor(0,0,0,0));
        paint(editor);
    }
}

void LineInstrument::mouseReleaseEvent(QMouseEvent *, FrameEditor& editor, const QPoint&)
{
    editor.applyInstrument(toolPreview);
    drawing = false;
}

void LineInstrument::paint(FrameEditor& editor)
{
    QPainter painter(&toolPreview);


    painter.setPen(QPen(editor.getPrimaryColor(), editor.getPenSize() ,
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));


    if(mStartPoint != mEndPoint) {

#if defined(LINE_INSTRUMENT_WORKAROUND)
        // Workaround for bug in Qt: see https://github.com/Blinkinlabs/PatternPaint/issues/66
        QPoint newStartPoint(mStartPoint);
        QPoint newEndPoint(mEndPoint);

        int deltaX = (mEndPoint-mStartPoint).x();
        int deltaY = (mEndPoint-mStartPoint).y();

        qDebug() << "deltaX=" << deltaX << " deltaY=" << deltaY;

        if((deltaX > 0 && deltaY < 0) || (deltaX < 0 && deltaY > 0)) {
            if(abs(deltaX) >=abs(deltaY)) {
                qDebug() << "A";
                newStartPoint += QPoint(0,1);
                newEndPoint += QPoint(0,1);
            }
            else {
                qDebug() << "B";
                newStartPoint += QPoint(1,0);
                newEndPoint += QPoint(1,0);
            }
        }

        painter.drawLine(newStartPoint, newEndPoint);
#else
        painter.drawLine(mStartPoint, mEndPoint);
#endif

    }

    if(mStartPoint == mEndPoint) {
        painter.drawPoint(mStartPoint);
    }
}
