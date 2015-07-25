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

#include "sprayinstrument.h"
#include "patterneditor.h"

#include <QPen>
#include <QPainter>
#include <math.h>

SprayInstrument::SprayInstrument(QObject *parent) :
    CustomCursorInstrument(":/instruments/images/instruments-icons/cursor_spray.png", parent)
{
    drawing = false;
}

void SprayInstrument::mousePressEvent(QMouseEvent *event, PatternEditor& editor, const QPoint& pt)
{
    if(event->button() == Qt::LeftButton)
    {
        toolPreview = QImage(editor.getPatternAsImage().width(),
                             editor.getPatternAsImage().height(),
                             QImage::Format_ARGB32_Premultiplied);
        toolPreview.fill(COLOR_CLEAR);
        drawing = true;

        mStartPoint = mEndPoint = pt;
        paint(editor);
    }
}

void SprayInstrument::mouseMoveEvent(QMouseEvent*, PatternEditor& editor, const QPoint& pt)
{
    if(drawing) {
        mEndPoint = pt;
        paint(editor);
        mStartPoint = pt;
    }
}

void SprayInstrument::mouseReleaseEvent(QMouseEvent*, PatternEditor& editor, const QPoint&)
{
    editor.applyInstrument(toolPreview);
    drawing = false;
}

void SprayInstrument::paint(PatternEditor& editor)
{
    QPainter painter(&toolPreview);

    painter.setPen(QPen(editor.getPrimaryColor(), editor.getPenSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    int x;
    int y;
    for(int i(0); i < 12; i++) {
        switch(i) {
        case 0: case 1: case 2: case 3:
            x = (qrand() % 5 - 2)
                    * sqrt(editor.getPenSize());
            y = (qrand() % 5 - 2)
                    * sqrt(editor.getPenSize());
            break;
        case 4: case 5: case 6: case 7:
            x = (qrand() % 10 - 4)
                    * sqrt(editor.getPenSize());
            y = (qrand() % 10 - 4)
                    * sqrt(editor.getPenSize());
            break;
        case 8: case 9: case 10: case 11:
            x = (qrand() % 15 - 7)
                    * sqrt(editor.getPenSize());
            y = (qrand() % 15 - 7)
                    * sqrt(editor.getPenSize());
            break;
        default:
            return;
        }

        painter.drawPoint(mEndPoint.x() + x, mEndPoint.y() + y);
    }
}
