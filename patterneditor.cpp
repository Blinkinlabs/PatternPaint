#include "patterneditor.h"

#include <iostream>
#include <cmath>
#include <QtWidgets>

PatternEditor::PatternEditor(QWidget *parent) :
    QWidget(parent)
{
}

void PatternEditor::init(int width, int height) {

    scale = 6;  // How big we want to make it

    delete pattern;
    delete gridPattern;
    delete activeColor;

    // Make a color image as default
    pattern = new QImage(width,height,QImage::Format_RGB32);
    float phase = 0;
    for(int x = 0; x < pattern->width();x++) {
        for(int y = 0; y < pattern->height(); y++) {

            int color = (((int)((std::sin(phase        + y/12.0) + 1)*127)) << 16)
                      + (((int)((std::sin(phase + 2.09 + y/12.0) + 1)*127)) << 8)
                      + (((int)((std::sin(phase + 4.18 + y/12.0) + 1)*127))     );
            pattern->setPixel(x,y,color);
        }
        phase += .105;
    }

    // And make a grid pattern to superimpose over the image
    gridPattern = new QImage(width*scale+1,height*scale+1,QImage::Format_ARGB32);
    gridPattern->fill(QColor(0,0,0,0));

    QPainter painter(gridPattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QColor(80,80,80));  //TODO: Global color pallate?
    for(int x = 0; x <= pattern->width(); x++) {
        painter.drawLine(x*scale,0,x*scale,pattern->height()*scale);
    }
    for(int y = 0; y <= pattern->height(); y++) {
        painter.drawLine(0,y*scale,pattern->width()*scale,y*scale);
    }

    activeColor = new QColor(0,0,0);
    size = 1;

    update();
}

void PatternEditor::init(QString filename) {
    // TODO: How to handle stuff that's not the right size?
    // just wedge it in?
    QImage newPattern(filename);

    // TODO: Stop if we couldn't open the file
    // TODO: Implement 'save' check?

    init(newPattern.width(),60);
    pattern->fill(QColor(0,0,0));

    QPainter painter(pattern);
    painter.drawImage(0,0,newPattern);

    update();
}

void PatternEditor::mousePressEvent(QMouseEvent *event){
    int x = event->x()/scale;
    int y = event->y()/scale;

    std::cout << "pressing in view, " << event->x() << "," << event->y() << std::endl;
    std::cout << "pressing in view, " << x << "," << y << std::endl;

    QPainter painter(pattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(*activeColor);
    painter.drawPoint(x,y);
    painter.drawEllipse ( QPoint(x,y), size, size );

    update();
}

void PatternEditor::mouseMoveEvent(QMouseEvent *event){
    int x = event->x()/scale;
    int y = event->y()/scale;

    std::cout << "pressing in view, " << event->x() << "," << event->y() << std::endl;
    std::cout << "pressing in view, " << x << "," << y << std::endl;

    QPainter painter(pattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(*activeColor);
    painter.drawPoint(x,y);
    painter.drawEllipse ( QPoint(x,y), size, size );

    update();
}

void PatternEditor::setToolColor(QColor color) {
    activeColor->operator =(color);
}

void PatternEditor::setToolSize(int s) {
    size = s;
}

void PatternEditor::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(QRect(0,0,pattern->width()*scale, pattern->height()*scale),*pattern);
    painter.drawImage(0,0,*gridPattern);
}
