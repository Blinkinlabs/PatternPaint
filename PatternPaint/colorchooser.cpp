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

#include "colorchooser.h"

#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>

ColorChooser::ColorChooser(const QColor& color, QWidget *parent) :
    QLabel(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Box);
    pixmap = QPixmap(30, 30);

    setMargin(2);
    setAlignment(Qt::AlignCenter);

    setColor(color);
    setPixmap(pixmap);
}

ColorChooser::~ColorChooser()
{
}

void ColorChooser::setColor(const QColor &color)
{
   currentColor = color;

    pixmap.fill(currentColor);
    setPixmap(pixmap);
}

void ColorChooser::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QColor color = QColorDialog::getColor(currentColor, this);
        if(color.isValid())
        {
            setColor(color);
            emit sendColor(color);
        }
    }
}
