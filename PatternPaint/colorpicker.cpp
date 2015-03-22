// Based on:
// http://www.julapy.com/processing/ColorPicker.pde

#include "colorpicker.h"

#include <QtWidgets>
#include <cmath>

ColorPicker::ColorPicker(QWidget *parent) :
    QWidget(parent)
{
}

void ColorPicker::init() {
    colorImage = QImage(width(),height(),QImage::Format_ARGB32);
    colorImage.fill(QColor(0,0,0,0));

    int w = width();
    int h = height() - 30;  // leave some space for the active color

    // draw color.
    int cw = w - 10;
    for( int i=0; i<cw; i++ )
    {
      float nColorPercent = i / (float)cw;
      float rad = (-360 * nColorPercent) * (M_PI / 180);
      int nR = (int)(cos(rad               ) * 127 + 128);
      int nG = (int)(cos(rad + 2 * M_PI / 3) * 127 + 128);
      int nB = (int)(cos(rad + 4 * M_PI / 3) * 127 + 128);
      QColor nColor(nR, nG, nB);

      setGradient( i, 0, h/2, QColor(0xFFFFFF), nColor );
      setGradient( i, (h/2), h/2, nColor, QColor(0,0,0) );
    }

    // draw black/white.
    drawRect( cw, 0,   5, h/2, QColor(0xFFFFFF) );
    drawRect( cw, h/2, 5, h/2-1, QColor(0,0,0) );

    // draw grey scale.
    for( int j=0; j<h-2; j++ )
    {
        int g = 255 - (int)(j/(float)(h-1) * 255 );
        drawRect( w-5, j, 5, 1, QColor( g,g,g) );
    }

    // Choose white by default
    setNewColor(QColor(255,255,255));

    update();
}

void ColorPicker::setGradient(int x, int y, float h, QColor c1, QColor c2 )
{
    float deltaR = c2.red() - c1.red();
    float deltaG = c2.green() - c1.green();
    float deltaB = c2.blue() - c1.blue();

    QPainter painter(&colorImage);
    for (int j = y; j<(y+h); j++)
    {
        QColor c = QColor( c1.red()+(j-y)*(deltaR/h), c1.green()+(j-y)*(deltaG/h), c1.blue()+(j-y)*(deltaB/h) );
        painter.setPen(c);
        painter.drawPoint(x,j);
    }
}

void ColorPicker::drawRect( int rx, int ry, int rw, int rh, QColor rc )
{
    QPainter painter(&colorImage);
    painter.setPen(rc);

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(rc);
    painter.setBrush(brush);

    painter.drawRect(rx, ry, rw, rh);

//    for(int i=rx; i<rx+rw; i++)
//    {
//        for(int j=ry; j<ry+rh; j++)
//        {
//            painter.drawPoint(i,j);
//        }
//    }
}

void ColorPicker::setNewColor(QColor color) {
    activeColor=color;
    drawRect(0,height()-20,width(),height(),activeColor);
    update();

    emit colorChanged(color);
}

void ColorPicker::mousePressEvent(QMouseEvent *event){
    // Get the color under the event, and set it as our active color
    setNewColor(colorImage.pixel(event->x(),event->y()));
}

void ColorPicker::mouseMoveEvent(QMouseEvent *event){
    // Get the color under the event, and set it as our active color
    setNewColor(colorImage.pixel(event->x(),event->y()));
}

void ColorPicker::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.drawImage(0,0,colorImage);
}
