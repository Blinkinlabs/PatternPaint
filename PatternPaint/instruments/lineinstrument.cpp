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

void LineInstrument::mousePressEvent(QMouseEvent *event, PatternEditor& pe)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        mStartPoint = mEndPoint = event->pos();
        pe.setPaint(true);
        mImageCopy = pe.getPatternAsImage();
        makeUndoCommand(pe);
    }
}

void LineInstrument::mouseMoveEvent(QMouseEvent *event, PatternEditor& pe)
{
    if(pe.isPaint())
    {
        mEndPoint = event->pos();
        pe.setImage(mImageCopy);
        if(event->buttons() & Qt::LeftButton)  paint(pe);
    }
}

void LineInstrument::mouseReleaseEvent(QMouseEvent *event, PatternEditor& pe)
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
    QPainter painter(pe.getDevice());
    painter.setPen(QPen(pe.getPrimaryColor(), pe.getPenSize() ,
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if(mStartPoint != mEndPoint)
    {
        painter.drawLine(QPoint(mStartPoint.x()/pe.scaleX(), mStartPoint.y()/pe.scaleY()),
                         QPoint(mEndPoint.x()/pe.scaleX(), mEndPoint.y()/pe.scaleY()));
    }

    if(mStartPoint == mEndPoint)
    {
        painter.drawPoint(QPoint(mStartPoint.x()/pe.scaleX(), mStartPoint.y()/pe.scaleY()));
    }

    painter.end();
}
