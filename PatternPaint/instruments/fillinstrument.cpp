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

#include "fillinstrument.h"
#include "patterneditor.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

FillInstrument::FillInstrument(QObject *parent) :
    CustomCursorInstrument(":/instruments/images/instruments-icons/cursor_fill.png", parent) {
}

void FillInstrument::mousePressEvent(QMouseEvent *event, PatternEditor& pe, const QPoint& pt)
{
    if(event->button() == Qt::LeftButton)
    {
        mStartPoint = mEndPoint = pt;
        pe.setPaint(true);
        makeUndoCommand(pe);
    }
}

void FillInstrument::mouseMoveEvent(QMouseEvent *, PatternEditor&, const QPoint&) {
}

void FillInstrument::mouseReleaseEvent(QMouseEvent*, PatternEditor& pe, const QPoint&)
{
    if(pe.isPaint()) {
        paint(pe);
        pe.setPaint(false);
    }
}

void FillInstrument::paint(PatternEditor& pe)
{
    QColor switchColor = pe.getPrimaryColor();
    QRgb pixel(pe.getPattern()->pixel(mStartPoint));

    QColor oldColor(pixel);

    if(switchColor != oldColor) {
        fillRecurs(mStartPoint.x(), mStartPoint.y(),
                   switchColor.rgb(), pixel,
                   *pe.getPattern());
    }

    pe.update();
}

void FillInstrument::fillRecurs(int x, int y, QRgb switchColor, QRgb oldColor, QImage& tempImage) {
    int temp_x(x), left_x(0);
    while(true)
    {
        if(tempImage.pixel(temp_x, y) != oldColor)
            break;
        tempImage.setPixel(temp_x, y, switchColor);
        if(temp_x > 0)
        {
            --temp_x;
            left_x = temp_x;
        }
        else
            break;
    }

    int right_x(0);
    temp_x = x + 1;
    while(true)
    {
        if(tempImage.pixel(temp_x, y) != oldColor)
            break;
        tempImage.setPixel(temp_x, y, switchColor);
        if(temp_x < tempImage.width() - 1)
        {
            temp_x++;
            right_x = temp_x;
        }
        else
            break;
    }

    for(int x_(left_x+1); x_ < right_x; ++x_)
    {
        if(y < 1 || y >= tempImage.height() - 1)
            break;
        if(right_x > tempImage.width())
            break;
        QRgb currentColor = tempImage.pixel(x_, y - 1);
        if(currentColor == oldColor && currentColor != switchColor)
            fillRecurs(x_, y - 1, switchColor, oldColor, tempImage);
        currentColor = tempImage.pixel(x_, y + 1);
        if(currentColor == oldColor && currentColor != switchColor)
            fillRecurs(x_, y + 1, switchColor, oldColor, tempImage);
    }
}
