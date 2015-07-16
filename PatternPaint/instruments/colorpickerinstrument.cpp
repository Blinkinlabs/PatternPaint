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
    CustomCursorInstrument(":/instruments/images/instruments-icons/cursor_pipette.png", parent) {
}

void ColorpickerInstrument::mousePressEvent(QMouseEvent *event, PatternEditor&, const QPoint& pt)
{
    if(event->button() == Qt::LeftButton) {
        mStartPoint = mEndPoint = pt;
        drawing = true;
    }
}

void ColorpickerInstrument::mouseMoveEvent(QMouseEvent*, PatternEditor& editor, const QPoint& pt)
{
    if(drawing) {
        mStartPoint = mEndPoint = pt;
        paint(editor);
    }
}

void ColorpickerInstrument::mouseReleaseEvent(QMouseEvent *, PatternEditor& editor, const QPoint& pt)
{
    drawing = false;
}

void ColorpickerInstrument::paint(PatternEditor& editor)
{
    bool inArea = true;

    if(mStartPoint.x() < 0 || mStartPoint.y() < 0
            || mStartPoint.x() > editor.width()
            || mStartPoint.y() > editor.height())
        inArea = false;

    if(inArea) {
        QRgb pixel(editor.getPatternAsImage().pixel(mStartPoint));
        QColor getColor(pixel);
        emit pickedColor(getColor);
    }
}
