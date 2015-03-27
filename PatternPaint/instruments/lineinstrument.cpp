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
}

void LineInstrument::mousePressEvent(QMouseEvent *event, PatternEditor& pe, const QPoint& pt)
{
    if(event->button() == Qt::LeftButton)
    {
        mStartPoint = mEndPoint = pt;
        pe.setPaint(true);
        mImageCopy = pe.getPatternAsImage();
        makeUndoCommand(pe);
    }
}

void LineInstrument::mouseMoveEvent(QMouseEvent*, PatternEditor& pe, const QPoint& pt)
{
    if(pe.isPaint()) {
        mEndPoint = pt;
        pe.setImage(mImageCopy);
        paint(pe);
    }
}

void LineInstrument::mouseReleaseEvent(QMouseEvent *event, PatternEditor& pe, const QPoint&)
{
    if(pe.isPaint())
    {
        pe.setImage(mImageCopy);
        if(event->button() == Qt::LeftButton)  paint(pe);
        pe.setPaint(false);
    }
}

void LineInstrument::paint(PatternEditor& pe)
{
    QPainter painter(pe.getPattern());
    painter.setPen(QPen(pe.getPrimaryColor(), pe.getPenSize() ,
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if(mStartPoint != mEndPoint) {
        painter.drawLine(mStartPoint, mEndPoint);
    }

    if(mStartPoint == mEndPoint) {
        painter.drawPoint(mStartPoint);
    }

    painter.end();
}
