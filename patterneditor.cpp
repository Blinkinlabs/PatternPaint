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

    // TODO: Just leak for now
    //delete pattern;
    //delete gridPattern;
    //delete activeColor;

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

    toolPreview = new QImage(width,height,QImage::Format_ARGB32);
    toolPreview->fill(QColor(0,0,0,0));

    toolColor = new QColor(0,0,0);
    toolSize = 1;

    // Turn on mouse tracking so we can draw a preview
    setMouseTracking(true);

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
    painter.setPen(*toolColor);

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(*toolColor);
    painter.setBrush(brush);

    for(int i = -1; i < 2; i++) {
        painter.drawPoint(x+i*pattern->width(),y);
        painter.drawEllipse ( QPoint(x+i*pattern->width(),y), toolSize/2, toolSize/2 );
    }

    update();
}

void PatternEditor::mouseMoveEvent(QMouseEvent *event){
    int x = event->x()/scale;
    int y = event->y()/scale;

    // If we aren't pressed down, just draw a preview
    QPainter painter(toolPreview);
    toolPreview->fill(QColor(0,0,0,0));

    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(*toolColor);

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(*toolColor);
    painter.setBrush(brush);

    for(int i = -1; i < 2; i++) {
        painter.drawPoint(x+i*pattern->width(),y);
        painter.drawEllipse ( QPoint(x+i*pattern->width(),y), toolSize/2, toolSize/2 );
    }

    if( event->buttons() &  Qt::LeftButton ) {
        QPainter painter(pattern);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(*toolColor);


        QBrush brush = painter.brush();
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(*toolColor);
        painter.setBrush(brush);

        for(int i = -1; i < 2; i++) {
            painter.drawPoint(x+i*pattern->width(),y);
            painter.drawEllipse ( QPoint(x+i*pattern->width(),y), toolSize/2, toolSize/2 );
        }
    }

    update();
}

void PatternEditor::setToolColor(QColor color) {
    toolColor->operator =(color);
}

void PatternEditor::setToolSize(int size) {
    toolSize = size;
}

void PatternEditor::setPlaybackRow(int row) {
    playbackRow = row;
    update();
}

void PatternEditor::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(QRect(0,0,pattern->width()*scale, pattern->height()*scale),*pattern);
    painter.drawImage(0,0,*gridPattern);
    painter.drawImage(QRect(0,0,pattern->width()*scale, pattern->height()*scale),*toolPreview);

    painter.setPen(QColor(255,255,255));
    painter.drawRect(playbackRow*scale,0,scale, height()-1);
    painter.fillRect(playbackRow*scale,0,scale, height(),QColor(255,255,255,100));
}
