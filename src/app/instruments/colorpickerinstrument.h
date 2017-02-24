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

#ifndef COLORPICKERINSTRUMENT_H
#define COLORPICKERINSTRUMENT_H

#include "abstractinstrument.h"

#include <QObject>

/**
 * @brief Colorpicker instrument class.
 *
 */
class ColorpickerInstrument : public AbstractInstrument
{
    Q_OBJECT

public:
    explicit ColorpickerInstrument(InstrumentConfiguration *instrumentConfiguration,
                                   QObject *parent = 0);

    void mousePressEvent(QMouseEvent *event, const QImage &frameData, const QPoint &);
    void mouseMoveEvent(QMouseEvent *event, const QImage &frameData, const QPoint &);
    void mouseReleaseEvent(QMouseEvent *event, FrameEditor &, const QImage &, const QPoint &);
    bool hasPreview() const
    {
        return false;
    }

private:
    void paint(const QImage &frameData, const QPoint &point);

signals:
    void pickedColor(QColor);
};

#endif // COLORPICKERINSTRUMENT_H
