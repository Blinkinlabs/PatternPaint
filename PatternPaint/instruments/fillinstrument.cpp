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
        fill(mStartPoint, switchColor.rgb(), pixel, pe.getPattern());
        /*fillRecurs(mStartPoint.x(), mStartPoint.y(),
                   switchColor.rgb(), pixel,
                   *pe.getPattern());
                   */
    }

    pe.update();
}

QList<QPoint> neighbors(const QPoint& pt, const QImage* img) {
    QList<QPoint> res;
    if (pt.x() > 0) res << QPoint(pt.x()-1, pt.y());
    if (pt.y() > 0) res << QPoint(pt.x(), pt.y()-1);
    if (pt.x() + 1 < img->width()) res << QPoint(pt.x()+1, pt.y());
    if (pt.y() + 1 < img->height()) res << QPoint(pt.x(), pt.y() + 1);
    return res;
}

void FillInstrument::fill(const QPoint& pt, QRgb newColor, QRgb oldColor, QImage* pattern) {
    if (pattern->pixel(pt) != oldColor) return;
    pattern->setPixel(pt, newColor);

    foreach(const QPoint& p, neighbors(pt, pattern)) {
        if (pattern->pixel(p) == oldColor) fill(p, newColor, oldColor, pattern);
    }
}
