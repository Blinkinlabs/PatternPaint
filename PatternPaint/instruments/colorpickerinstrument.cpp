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

#include "colorpickerinstrument.h"
#include "patterneditor.h"

ColorpickerInstrument::ColorpickerInstrument(QObject *parent) :
    AbstractInstrument(parent), mpm(":/instruments/images/instruments-icons/cursor_pipette.png")
{
    mcur = QCursor(mpm);
}

void ColorpickerInstrument::mousePressEvent(QMouseEvent *event, PatternEditor& pe)
{
    if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton)
    {
        pe.setPaint(true);
    }
}

void ColorpickerInstrument::mouseMoveEvent(QMouseEvent *event, PatternEditor& pe)
{
    QRgb pixel(pe.getDevice()->pixel(event->pos()));
    QColor getColor(pixel);
    //pe.emitColor(getColor);
}

void ColorpickerInstrument::mouseReleaseEvent(QMouseEvent *event, PatternEditor& pe)
{
    if(pe.isPaint())
    {
        mStartPoint = mEndPoint = event->pos();
        if(event->button() == Qt::LeftButton)
        {
            paint(pe, false);
            //pe.emitPrimaryColorView();
        }
        else if(event->button() == Qt::RightButton)
        {
            paint(pe, true);
            //PatternEditor.emitSecondaryColorView();
        }
        pe.setPaint(false);
        //pe.emitRestorePreviousInstrument();
    }
}

void ColorpickerInstrument::paint(PatternEditor& pe, bool isSecondaryColor, bool)
{
    bool inArea(true);
    if(mStartPoint.x() < 0 || mStartPoint.y() < 0
            || mStartPoint.x() > pe.width()
            || mStartPoint.y() > pe.height())
        inArea = false;

    if(inArea)
    {
        QRgb pixel(pe.getDevice()->pixel(QPoint(mStartPoint.x()/pe.scaleX(), mStartPoint.y()/pe.scaleY())));
        QColor getColor(pixel);
        emit pickedColor(getColor);
    }
}
