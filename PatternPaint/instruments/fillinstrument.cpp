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
#include "frameeditor.h"

#include <QPen>
#include <QPainter>
#include <QDebug>

FillInstrument::FillInstrument(QObject *parent) :
    CustomCursorInstrument(":/instruments/images/instruments-icons/cursor_fill.png", parent)
{
    drawing = false;
}

void FillInstrument::mousePressEvent(QMouseEvent *event, FrameEditor& editor, const QPoint& pt)
{
    if(event->button() == Qt::LeftButton)
    {
        drawing = true;
        toolPreview = editor.getPatternAsImage();

        mStartPoint = mEndPoint = pt;
        paint(editor);
    }
}

void FillInstrument::mouseMoveEvent(QMouseEvent *, FrameEditor&, const QPoint&) {
}

void FillInstrument::mouseReleaseEvent(QMouseEvent*, FrameEditor& editor, const QPoint&)
{
    editor.applyInstrument(toolPreview);
    drawing = false;
}

void FillInstrument::paint(FrameEditor& editor)
{
    // Make a copy of the image
    toolPreview = editor.getPatternAsImage();

    QColor switchColor = editor.getPrimaryColor();
    QRgb pixel(toolPreview.pixel(mStartPoint));

    QColor oldColor(pixel);

    if(switchColor.rgb() != oldColor.rgb()) {
        fill(mStartPoint, switchColor.rgb(), pixel, toolPreview);
    }
}

QList<QPoint> neighbors(const QPoint& pt, const QImage& img) {
    QList<QPoint> res;
    if (pt.x() > 0)                 res << QPoint(pt.x()-1, pt.y()  );
    if (pt.y() > 0)                 res << QPoint(pt.x()  , pt.y()-1);
    if (pt.x() < img.width() - 1)  res << QPoint(pt.x()+1, pt.y()  );
    if (pt.y() < img.height() - 1) res << QPoint(pt.x()  , pt.y()+1);
    return res;
}

void FillInstrument::fill(const QPoint& pt, QRgb newColor, QRgb oldColor, QImage& pattern) {
    if(pt.x() >= pattern.width() || pt.y() >= pattern.height()) {
        return;
    }

    if (pattern.pixel(pt) != oldColor) {
        return;
    }
    pattern.setPixel(pt, newColor);

    foreach(const QPoint& p, neighbors(pt, pattern)) {
        if (pattern.pixel(p) == oldColor) fill(p, newColor, oldColor, pattern);
    }
}
