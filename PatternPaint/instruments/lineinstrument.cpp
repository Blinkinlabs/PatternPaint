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
#include "patterneditor.h"

#include <QPen>
#include <QPainter>
#include <QImage>

LineInstrument::LineInstrument(QObject *parent) :
    AbstractInstrument(parent)
{
    drawing = false;
}

void LineInstrument::mousePressEvent(QMouseEvent *event, PatternEditor& editor, const QPoint& pt)
{
    if(event->button() == Qt::LeftButton)
    {
        toolPreview = QImage(editor.getPatternAsImage().width(),
                             editor.getPatternAsImage().height(),
                             QImage::Format_ARGB32_Premultiplied);
        toolPreview.fill(COLOR_CLEAR);

        mStartPoint = mEndPoint = pt;
        paint(editor);
        drawing = true;
    }
}

void LineInstrument::mouseMoveEvent(QMouseEvent*, PatternEditor& editor, const QPoint& pt)
{
    if(drawing) {
        mEndPoint = pt;
        toolPreview.fill(COLOR_CLEAR);
        paint(editor);
    }
}

void LineInstrument::mouseReleaseEvent(QMouseEvent *, PatternEditor& editor, const QPoint&)
{
    editor.applyInstrument(toolPreview);
    drawing = false;
}

void LineInstrument::paint(PatternEditor& editor)
{
    QPainter painter(&toolPreview);

    painter.setPen(QPen(editor.getPrimaryColor(), editor.getPenSize() ,
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));


    if(mStartPoint != mEndPoint) {
        painter.drawLine(mStartPoint, mEndPoint);
    }

    if(mStartPoint == mEndPoint) {
        painter.drawPoint(mStartPoint);
    }
}
