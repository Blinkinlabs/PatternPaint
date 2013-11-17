#include "patterneditor.h"

#include <iostream>
#include <cmath>
#include <QtWidgets>

// TODO: Change this when we connect to a tape, etc?
#define BLINKYTAPE_STRIP_HEIGHT 60

PatternEditor::PatternEditor(QWidget *parent) :
    QWidget(parent)
{
}

void PatternEditor::init(int width, int height)
{
    xScale = 7;  // How big we want to make it
    yScale = 6;  // How big we want to make it

    // Make a color image as default
    pattern = QImage(width,height,QImage::Format_RGB32);
    pattern.fill(0);
//    float phase = 0;
//    for(int x = 0; x < pattern.width();x++) {
//        for(int y = 0; y < pattern.height(); y++) {

//            int color = (((int)((std::sin(phase        + y/12.0) + 1)*127)) << 16)
//                      + (((int)((std::sin(phase + 2.09 + y/12.0) + 1)*127)) << 8)
//                      + (((int)((std::sin(phase + 4.18 + y/12.0) + 1)*127))     );
//            pattern.setPixel(x,y,color);
//        }
//        phase += .105;
//    }

    // And make a grid pattern to superimpose over the image
    gridPattern = QImage(width*xScale+1,height*yScale+1,QImage::Format_ARGB32);
    gridPattern.fill(QColor(0,0,0,0));

    QPainter painter(&gridPattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.setPen(QColor(40,40,40));
    for(int x = 0; x <= pattern.width(); x++) {
        painter.drawLine(x*xScale,0,x*xScale,pattern.height()*yScale-1);
        painter.drawLine(x*xScale-1,0,x*xScale-1,pattern.height()*yScale-1);
    }

    toolPreview = QImage(width,height,QImage::Format_ARGB32);
    toolPreview.fill(QColor(0,0,0,0));

    toolColor = QColor(255,255,255);
    toolSize = 2;

    // Turn on mouse tracking so we can draw a preview
    setMouseTracking(true);

    // Set the widget size
    setFixedSize(pattern.width()*xScale,pattern.height()*yScale);
    updateGeometry();

    update();
}

bool PatternEditor::init(QString fileName) {
    // TODO: How to handle stuff that's not the right size?
    // Right now we just take the top part, could resize instead.
    QImage newPattern;

    if(!newPattern.load(fileName)) {
        return false;
    }

    // If the pattern doesn't fit, scale it.
    // TODO: Display an import dialog to let the user decide what to do?
    if(newPattern.height() > BLINKYTAPE_STRIP_HEIGHT) {
        newPattern = newPattern.scaledToHeight(BLINKYTAPE_STRIP_HEIGHT);
    }

    // TODO: Implement 'save' check?

    init(newPattern.width(),BLINKYTAPE_STRIP_HEIGHT);
    pattern.fill(QColor(0,0,0));

    QPainter painter(&pattern);
    painter.drawImage(0,0,newPattern);

    update();
    return true;
}

void PatternEditor::mousePressEvent(QMouseEvent *event){
    int x = event->x()/xScale;
    int y = event->y()/yScale;

    QPainter painter(&pattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(toolColor);

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(toolColor);
    painter.setBrush(brush);

    painter.drawPoint(x,y);
    painter.drawEllipse ( QPoint(x,y), toolSize/2, toolSize/2 );

    update();
}

void PatternEditor::leaveEvent(QEvent * event) {
    QPainter painter(&toolPreview);
    toolPreview.fill(QColor(0,0,0,0));

    update();
}

void PatternEditor::mouseMoveEvent(QMouseEvent *event){
    int x = event->x()/xScale;
    int y = event->y()/yScale;

    // If we aren't pressed down, just draw a preview
    QPainter painter(&toolPreview);
    toolPreview.fill(QColor(0,0,0,0));

    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(toolColor);

    QBrush brush = painter.brush();
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(toolColor);
    painter.setBrush(brush);

    painter.drawPoint(x,y);
    painter.drawEllipse ( QPoint(x,y), toolSize/2, toolSize/2 );

    if( event->buttons() &  Qt::LeftButton ) {
        QPainter painter(&pattern);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen(toolColor);


        QBrush brush = painter.brush();
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(toolColor);
        painter.setBrush(brush);

        painter.drawPoint(x,y);
        painter.drawEllipse ( QPoint(x,y), toolSize/2, toolSize/2 );
    }

    update();
}

void PatternEditor::setToolColor(QColor color) {
    toolColor = color;
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

    painter.drawRect(0,0,width()-1, height()-1);

    // Draw the image
    painter.drawImage(QRect(0,0,pattern.width()*xScale, pattern.height()*yScale),pattern);
    painter.drawImage(QRect(0,0,pattern.width()*xScale, pattern.height()*yScale),toolPreview);
    painter.drawImage(0,0,gridPattern);

    // Draw the playback indicator
    painter.setPen(QColor(255,255,255));
    painter.drawRect(playbackRow*xScale,0,xScale-1, pattern.height()*yScale-1);
    painter.fillRect(playbackRow*xScale,0,xScale-1, pattern.height()*yScale,QColor(255,255,255,100));
}
