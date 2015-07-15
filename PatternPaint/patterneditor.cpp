#include "patterneditor.h"
#include "abstractinstrument.h"

#include <iostream>
#include <cmath>
#include <QtWidgets>
#include <QDebug>

#define COLOR_CLEAR             QColor(0,0,0,0)
#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,0)
#define COLOR_GRID_LINES        QColor(30,30,30,200)
#define COLOR_GRID_EDGES        QColor(30,30,30, 60)

#define COLOR_TOOL_DEFAULT      QColor(255,255,255)

#define COLOR_PLAYBACK_EDGE     QColor(255,255,255,255)
#define COLOR_PLAYBACK_TOP      QColor(255,255,255,100)

#define MIN_UPDATE_INTERVAL     15  // minimum interval between screen updates, in ms
#define MIN_MOUSE_INTERVAL      5   // minimum interval between mouse inputs, in ms

PatternEditor::PatternEditor(QWidget *parent) :
    QWidget(parent)
{
    m_isPaint = false;
    m_pi = NULL;
}

void PatternEditor::setImage(const QImage& img) {
    if(patternItem == NULL) {
        return;
    }

    patternItem->setImage(img);
}

const QImage &PatternEditor::getPatternAsImage() const {
    if(patternItem == NULL) {
        return QImage(1,1,QImage::Format_RGB32);
    }

    return QImage(patternItem->getImage());
}

QImage* PatternEditor::getPattern() {
    if(patternItem == NULL) {
        return NULL;
    }

    return patternItem->getImagePointer();
}

void PatternEditor::resizeEvent(QResizeEvent * event)
{
    Q_UNUSED(event);

    updateGridSize();
}

//void PatternEditor::init(int frameCount, int stripLength)
//{
//    // Store the width and height, so that letterboxscrollarea can resize this widget properly
//    this->setBaseSize(frameCount, stripLength);

//    // Initialize the pattern to a blank canvass
//    pattern = QImage(frameCount,
//                     stripLength,
//                     QImage::Format_ARGB32_Premultiplied);
//    pattern.fill(COLOR_CANVAS_DEFAULT);

//    toolPreview = QImage(frameCount,
//                         stripLength,
//                         QImage::Format_ARGB32_Premultiplied);
//    toolPreview.fill(COLOR_CLEAR);

//    // Turn on mouse tracking so we can draw a preview
//    setMouseTracking(true);

//    updateGridSize();

//    update();
//}

//bool PatternEditor::init(QImage newPattern, bool scaled) {
//    // TODO: Implement 'save' check before overwriting?

//    // If the pattern doesn't fit, scale it.
//    // TODO: Display an import dialog to let the user decide what to do?
//    if(scaled && newPattern.height() != pattern.height()) {
//        newPattern = newPattern.scaledToHeight(pattern.height());
//    }

//    // Re-init the display using the new geometry
//    init(newPattern.width(), newPattern.height());

//    // Draw the new pattern to the display
//    QPainter painter(&pattern);
//    painter.drawImage(0,0,newPattern);

//    // and force a screen update
//    update();

//    emit resized();
//    return true;
//}

void PatternEditor::updateGridSize() {
    if(patternItem == NULL) {
        return;
    }

    // Base the widget size on the window height
    // cast float to int to save rounded scale
    float scale = static_cast<int>(float(size().height() - 1)/pattern->height()*10);
    scale /= 10;

    // Use a square aspect to display the grid
    xScale = scale;
    yScale = scale;


    // And make a grid pattern to superimpose over the image
    gridPattern = QImage(pattern->width()*xScale  +.5 + 1,
                         pattern->height()*yScale +.5 + 1,
                         QImage::Format_ARGB32_Premultiplied);
    gridPattern.fill(COLOR_CLEAR);

    QPainter painter(&gridPattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Draw vertical lines
    painter.setPen(COLOR_GRID_LINES);
    for(int x = 0; x <= pattern->width(); x++) {
        painter.drawLine(x*xScale+.5,
                         0,
                         x*xScale+.5,
                         gridPattern.height());
    }

    // Draw horizontal lines
    for(int y = 0; y <= pattern->height(); y++) {
        painter.drawLine(0,
                         y*yScale+.5,
                         gridPattern.width(),
                         y*yScale+.5);
    }

    // Draw corners
    painter.setPen(COLOR_GRID_EDGES);
    for(int x = 0; x <= pattern->width(); x++) {
        for(int y = 0; y <= pattern->height(); y++) {
            painter.drawPoint(QPoint(x*xScale     +.5 +1,    y*yScale     +.5 +1));
            painter.drawPoint(QPoint((x+1)*xScale +.5 -1,    y*yScale     +.5 +1));
            painter.drawPoint(QPoint(x*xScale     +.5 +1,    (y+1)*yScale +.5 -1));
            painter.drawPoint(QPoint((x+1)*xScale +.5 -1,    (y+1)*yScale +.5 -1));
        }
    }
}


void PatternEditor::mousePressEvent(QMouseEvent *event) {
    if (m_pi) {
        m_pi->mousePressEvent(event, *this, QPoint(event->x()/xScale, event->y()/yScale));
        lazyUpdate();
    }
}

void PatternEditor::leaveEvent(QEvent * event) {
    Q_UNUSED(event);

    toolPreview.fill(COLOR_CLEAR);

    update();
}

void PatternEditor::mouseMoveEvent(QMouseEvent *event){

    // Ignore the update request if it came too quickly
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_MOUSE_INTERVAL) {
        return;
    }

    lastTime = newTime;

    static int oldX = -1;
    static int oldY = -1;

    Q_UNUSED(oldX);
    Q_UNUSED(oldY);

    int x = event->x()/xScale;
    int y = event->y()/yScale;

    // If the position hasn't changed, don't do anything.
    // This is to avoid expensive reprocessing of the tool preview window.
    if(x == oldX && y == oldY) {
        return;
    }

    oldX = x;
    oldY = y;

    if (m_pi) {
        setCursor(m_pi->cursor());
        m_pi->mouseMoveEvent(event, *this, QPoint(x, y));
    } else {
        setCursor(Qt::ArrowCursor);
    }

    lazyUpdate();
}

void PatternEditor::mouseReleaseEvent(QMouseEvent* event) {
    if (m_pi) {
        m_pi->mouseReleaseEvent(event, *this, QPoint(event->x()/xScale, event->y()/yScale));
        lazyUpdate();
    }
}

void PatternEditor::setPatternItem(PatternItem* newPatternItem) {
    if(newPatternItem == NULL) {
        return;
    }

    patternItem = newPatternItem;

    pattern = patternItem->getImagePointer();

    // Store the width and height, so that letterboxscrollarea can resize this widget properly
    this->setBaseSize(pattern->width(), pattern->height());

    toolPreview = QImage(pattern->width(),
                         pattern->height(),
                         QImage::Format_ARGB32_Premultiplied);
    toolPreview.fill(COLOR_CLEAR);

    // Turn on mouse tracking so we can draw a preview
    setMouseTracking(true);

    updateGridSize();

    // and force a screen update
    update();

    emit resized();
}

void PatternEditor::setToolColor(QColor color) {
    toolColor = color;
}

void PatternEditor::setToolSize(int size) {
    toolSize = size;
}

void PatternEditor::setPlaybackRow(int row) {
    // TODO: Sanitize this?
    playbackRow = row;
    lazyUpdate();
}

void PatternEditor::setInstrument(AbstractInstrument* pi) {
    m_pi = pi;
}

void PatternEditor::lazyUpdate() {
    // Ignore the update request if it came too quickly
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_UPDATE_INTERVAL) {
        return;
    }

    lastTime = newTime;

    update();
}

void PatternEditor::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // If we don't have a pattern item, show an empty view
    if(patternItem == NULL) {
        painter.fillRect(0,0,this->width(),this->height(), QColor(0,0,0));
        return;
    }

    // Draw the image and tool preview
    painter.drawImage(QRect(0,0,pattern->width()*xScale+.5,pattern->height()*yScale), *pattern);

    if (m_pi && m_pi->showPreview())
        painter.drawImage(QRect(0,0,pattern->width()*xScale+.5,pattern->height()*yScale), toolPreview);

    painter.drawImage(0,0,gridPattern);

    // Draw the playback indicator
    // Note that we need to compute the correct width based on the rounding error of
    // the current cell, otherwise it won't line up correctly with the actual image.
    painter.setPen(COLOR_PLAYBACK_EDGE);
    painter.drawRect(playbackRow*xScale +.5,
                     0,
                     int((playbackRow+1)*xScale +.5) - int(playbackRow*xScale +.5),
                     pattern->height()*yScale);
    painter.fillRect(playbackRow*xScale +.5,
                     0,
                     int((playbackRow+1)*xScale +.5) - int(playbackRow*xScale +.5),
                     pattern->height()*yScale,
                     COLOR_PLAYBACK_TOP);

}

void PatternEditor::pushUndoState()
{
    patternItem->pushUndoState();
}
