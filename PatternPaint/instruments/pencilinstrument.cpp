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

#include "pencilinstrument.h"
#include "patterneditor.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

PencilInstrument::PencilInstrument(QObject *parent) :
    AbstractInstrument(parent)
{
}

void PencilInstrument::mousePressEvent(QMouseEvent *event, PatternEditor& pe)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        mStartPoint = mEndPoint = event->pos();
        pe.setPaint(true);  // ?
        makeUndoCommand(pe);
    }
}

void PencilInstrument::mouseMoveEvent(QMouseEvent *event, PatternEditor& pe)
{
    if(pe.isPaint())
    {
        mEndPoint = event->pos();
        if(event->buttons() & Qt::LeftButton)
        {
            paint(pe, false);
        }
        else if(event->buttons() & Qt::RightButton)
        {
            paint(pe, true);
        }
        mStartPoint = event->pos();
    }
}

void PencilInstrument::mouseReleaseEvent(QMouseEvent *event, PatternEditor& pe)
{
    if(pe.isPaint())
    {
        mEndPoint = event->pos();
        if(event->button() == Qt::LeftButton)
        {
            paint(pe, false);
        }
        else if(event->button() == Qt::RightButton)
        {
            paint(pe, true);
        }

        pe.setPaint(false); // ?
    }
}

void PencilInstrument::paint(PatternEditor& pe, bool isSecondaryColor, bool)
{
    QPainter painter(pe.getDevice());
    if(isSecondaryColor)
    {
#if 0
        painter.setPen(QPen(DataSingleton::Instance()->getSecondaryColor(),
                            DataSingleton::Instance()->getPenSize() * pe.getZoomFactor(),
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
#endif
    }
    else
    {
        qDebug() << "pencil paint " << pe.getPrimaryColor();
        painter.setPen(QPen(pe.getPrimaryColor(),
                            pe.getPenSize() * pe.getZoomFactor(),
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    }

    if(mStartPoint != mEndPoint)
    {
        painter.drawLine(QPoint(mStartPoint.x()/pe.scaleX(), mStartPoint.y()/pe.scaleY()),
                         QPoint(mEndPoint.x()/pe.scaleX(), mEndPoint.y()/pe.scaleY()));
    }

    if(mStartPoint == mEndPoint)
    {
        qDebug() << "draw point " << mStartPoint;
        painter.drawPoint(mStartPoint.x()/pe.scaleX(), mStartPoint.y()/pe.scaleY());
    }
    //pe.setEdited(true);
    //    int rad(DataSingleton::Instance()->getPenSize() + round(sqrt((mStartPoint.x() - mEndPoint.x()) *
    //                                                                 (mStartPoint.x() - mEndPoint.x()) +
    //                                                                 (mStartPoint.y() - mEndPoint.y()) *
    //                                                                 (mStartPoint.y() - mEndPoint.y()))));
    //    mPImageArea->update(QRect(mStartPoint, mEndPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    painter.end();
    pe.update();
}
