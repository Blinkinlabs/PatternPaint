#include "patterneditor.h"
#include "abstractinstrument.h"
#include "timelinedisplay.h"

#include <cmath>
#include <QtWidgets>
#include <QDebug>

#define COLOR_CLEAR             QColor(0,0,0,0)
#define COLOR_CANVAS_DEFAULT    QColor(0,0,0,0)
#define COLOR_GRID_LINES        QColor(30,30,30,200)

#define COLOR_TOOL_DEFAULT      QColor(255,255,255)

#define COLOR_PLAYBACK_EDGE     QColor(255,255,255,255)
#define COLOR_PLAYBACK_TOP      QColor(255,255,255,100)

#define MIN_UPDATE_INTERVAL     15  // minimum interval between screen updates, in ms
#define MIN_MOUSE_INTERVAL      5   // minimum interval between mouse inputs, in ms

#define GRID_MIN_Y_SCALE 6      // Minimum scale that the image needs to scale to before the grid is displayed


PatternEditor::PatternEditor(QWidget *parent) :
    QWidget(parent)
{
    this->setAcceptDrops(true);
    deviceModel = new TimelineDisplay();
}


void PatternEditor::setDisplayModel(DisplayModel *newDisplayModel)
{
    deviceModel = newDisplayModel;
    setPatternItem(patternItem);
}

void PatternEditor::dragEnterEvent(QDragEnterEvent *event)
{
    if(patternItem == NULL) {
        return;
    }

    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void PatternEditor::dropEvent(QDropEvent *event)
{
    if(patternItem == NULL) {
        return;
    }

    // TODO: Pass this down for someone else to handle?

    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int droppedUrlCnt = droppedUrls.size();
    for(int i = 0; i < droppedUrlCnt; i++) {
        QString localPath = droppedUrls[i].toLocalFile();
        QFileInfo fileInfo(localPath);

        // TODO: OS X Yosemite hack for /.file/id= references

        if(fileInfo.isFile()) {
            patternItem->replace(fileInfo.absoluteFilePath());
            // And stop on the first one we've found.
            return;
        }
    }
}

const QImage &PatternEditor::getPatternAsImage() const {
    if(!deviceModel->hasPatternItem()) {
        // TODO: Refactor so we don't have to do this?
        static QImage nullimage(1,1,QImage::Format_RGB32);
        return nullimage;
    }

    return deviceModel->getFrameData();
}

void PatternEditor::resizeEvent(QResizeEvent * )
{
    updateGridSize();
}

void PatternEditor::updateGridSize() {
    if(!deviceModel->hasPatternItem()) {
        return;
    }

    // Base the widget size on the window height
    // cast float to int to save rounded scale
    QSize frameSize = deviceModel->getFrameData().size();

    float scale = static_cast<int>(float(size().height() - 1)/frameSize.height()*10);
    scale /= 10;

    // Use a square aspect to display the grid
    xScale = scale;
    yScale = scale;

    // If the drawing space is large enough, make a grid pattern to superimpose over the image
    if(yScale >= GRID_MIN_Y_SCALE) {
        gridPattern = QImage(frameSize.width()*xScale  +.5 + 1,
                             frameSize.height()*yScale +.5 + 1,
                             QImage::Format_ARGB32_Premultiplied);
        gridPattern.fill(COLOR_CLEAR);


        QPainter painter(&gridPattern);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setRenderHint(QPainter::Antialiasing, false);

        // Draw vertical lines
        painter.setPen(COLOR_GRID_LINES);
        for(int x = 0; x <= frameSize.width(); x++) {
            painter.drawLine(x*xScale+.5,
                             0,
                             x*xScale+.5,
                             gridPattern.height());
        }

        // Draw horizontal lines
        for(int y = 0; y <= frameSize.height(); y++) {
            painter.drawLine(0,
                             y*yScale+.5,
                             gridPattern.width(),
                             y*yScale+.5);
        }
    }
}


void PatternEditor::mousePressEvent(QMouseEvent *event) {
    if (!deviceModel->hasPatternItem() || instrument.isNull()) {
        return;
    }

    setCursor(instrument->cursor());
    instrument->mousePressEvent(event, *this, QPoint(event->x()/xScale, event->y()/yScale));
    lazyUpdate();
}

void PatternEditor::mouseMoveEvent(QMouseEvent *event){
    if (!deviceModel->hasPatternItem() || instrument.isNull()) {
        return;
    }

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

    instrument->mouseMoveEvent(event, *this, QPoint(x, y));

    lazyUpdate();
}

void PatternEditor::mouseReleaseEvent(QMouseEvent* event) {
    setCursor(Qt::ArrowCursor);

    if (!deviceModel->hasPatternItem() || instrument.isNull()) {
        return;
    }

    instrument->mouseReleaseEvent(event, *this, QPoint(event->x()/xScale, event->y()/yScale));
    lazyUpdate();
}

void PatternEditor::setPatternItem(PatternItem* newPatternItem) {
    patternItem = newPatternItem;
    deviceModel->setSource(newPatternItem);

    if(!deviceModel->hasPatternItem()) {
        // TODO: Reset size to some default?
        return;
    }

    this->setBaseSize(deviceModel->getFrameData().size());

    // Turn on mouse tracking so we can draw a preview
    setMouseTracking(true);

    updateGridSize();

    // and force a screen update
    update();
}

void PatternEditor::setToolColor(QColor color) {
    toolColor = color;
}

void PatternEditor::setToolSize(int size) {
    toolSize = size;
}

void PatternEditor::setPlaybackRow(int row) {
    deviceModel->setFrameIndex(row);
    lazyUpdate();
}

void PatternEditor::setInstrument(AbstractInstrument* pi) {
    instrument = pi;
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
    if(!deviceModel->hasPatternItem()) {
        painter.fillRect(0,0,this->width(),this->height(), QColor(0,0,0));
        return;
    }

    QImage frameData = deviceModel->getFrameData();

    // Draw the image and tool preview
    painter.drawImage(QRect(0,0,
                            frameData.width()*xScale+.5,
                            frameData.height()*yScale),
                            frameData);

    if (!instrument.isNull() && instrument->showPreview()) {
        painter.drawImage(QRect(0,0,frameData.width()*xScale+.5,frameData.height()*yScale), (instrument->getPreview()));
    }

    if(yScale >= GRID_MIN_Y_SCALE) {
        painter.drawImage(0,0,gridPattern);
    }

    // TODO: How to do this more generically?
    if(deviceModel->showPlaybackIndicator()) {
        int playbackRow = deviceModel->getFrameIndex();

        // Draw the playback indicator
        // Note that we need to compute the correct width based on the rounding error of
        // the current cell, otherwise it won't line up correctly with the actual image.
        painter.setPen(COLOR_PLAYBACK_EDGE);
        painter.drawRect(playbackRow*xScale +.5,
                         0,
                         int((playbackRow+1)*xScale +.5) - int(playbackRow*xScale +.5),
                         frameData.height()*yScale);
        painter.fillRect(playbackRow*xScale +.5,
                         0,
                         int((playbackRow+1)*xScale +.5) - int(playbackRow*xScale +.5),
                         frameData.height()*yScale,
                         COLOR_PLAYBACK_TOP);
    }

}

void PatternEditor::applyInstrument(QImage& update)
{
    if(!deviceModel->hasPatternItem()) {
        return;
    }

    deviceModel->applyInstrument(update);
}
