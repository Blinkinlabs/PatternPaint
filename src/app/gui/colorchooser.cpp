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
#include <QMouseEvent>
#include <QColorDialog>
#include <QDebug>
#include <QPainter>
#include <QFrame>

#define ICON_SIZE 32

ColorChooser::ColorChooser(QWidget *parent) :
    QWidget(parent),
    colorDialog(this)
{
    this->setMinimumSize(ICON_SIZE,ICON_SIZE);

    colorDialog.setOptions(QColorDialog::NoButtons);

    connect(&colorDialog, QColorDialog::currentColorChanged,
            this, ColorChooser::on_currentColorChanged);
}

void ColorChooser::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    int offsetY = 0;
    if(this->height()>ICON_SIZE)
        offsetY = (this->height()-32)/2;

    painter.fillRect(0, offsetY, ICON_SIZE, ICON_SIZE, currentColor);

    painter.setPen(QPen(Qt::black));
    painter.drawRect(0, offsetY, ICON_SIZE-1, ICON_SIZE-1);
    painter.drawRect(1, offsetY+1, ICON_SIZE-3, ICON_SIZE-3);
}


void ColorChooser::setColor(const QColor &color)
{
    if (!color.isValid())
        return;

    currentColor = color;
    update();

    colorDialog.setCurrentColor(currentColor);
}

void ColorChooser::on_currentColorChanged(const QColor &color)
{
    if (!color.isValid())
        return;

    currentColor = color;
    update();

    emit(sendColor(color));
}

void ColorChooser::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // TODO: If the user closed the dialog, this won't toggle for some reason?
        colorDialog.setVisible(!colorDialog.isVisible());
    }
}
