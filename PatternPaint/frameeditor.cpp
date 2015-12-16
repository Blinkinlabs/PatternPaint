#include "frameeditor.h"
#include "abstractinstrument.h"

#include <cmath>
#include <QtWidgets>
#include <QDebug>

#define COLOR_CLEAR             QColor(0, 0, 0, 0)
#define COLOR_GRID_LINES        QColor(30, 30, 30, 200)

#define COLOR_TOOL_DEFAULT      QColor(255, 255, 255)

#define COLOR_PLAYBACK_EDGE     QColor(255, 255, 255, 255)
#define COLOR_PLAYBACK_TOP      QColor(255, 255, 255, 100)

#define MIN_UPDATE_INTERVAL     15  // minimum interval between screen updates, in ms
#define MIN_MOUSE_INTERVAL      5   // minimum interval between mouse inputs, in ms

#define GRID_MIN_Y_SCALE        6   // Minimum scale that the image needs to scale to before the grid is displayed

FrameEditor::FrameEditor(QWidget *parent) :
    QWidget(parent),
    frameIndex(0),
    showPlaybackIndicator(false)
{
    this->setAcceptDrops(true);

    // Turn on mouse tracking so we can draw a preview
    // TODO: DO we need to do this here, or just in the constructor?
    setMouseTracking(true);
}

bool FrameEditor::hasImage()
{
    return !frameData.isNull();
}

void FrameEditor::dragEnterEvent(QDragEnterEvent *event)
{
    if (!hasImage())
        return;

    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void FrameEditor::dropEvent(QDropEvent *event)
{
    Q_UNUSED(event);

// TODO: Attempt to load the file as a single frame image, to replace this frame?

// if(patternData == NULL) {
////    if(patternItem == NULL) {
// return;
// }

//// TODO: Pass this down for someone else to handle?

// QList<QUrl> droppedUrls = event->mimeData()->urls();
// int droppedUrlCnt = droppedUrls.size();
// for(int i = 0; i < droppedUrlCnt; i++) {
// QString localPath = droppedUrls[i].toLocalFile();
// QFileInfo fileInfo(localPath);

//// TODO: OS X Yosemite hack for /.file/id= references

// if(fileInfo.isFile()) {
// patternItem->replace(fileInfo.absoluteFilePath());
//// And stop on the first one we've found.
// return;
// }
// }
}

const QImage &FrameEditor::getPatternAsImage() const
{
    return frameData;
}

void FrameEditor::resizeEvent(QResizeEvent *resizeEvent)
{
    updateGridSize();

    QWidget::resizeEvent(resizeEvent);
}

void FrameEditor::updateGridSize()
{
    if (!hasImage())
        return;

    // Update the widget geometry so that it can be resized correctly
    this->setBaseSize(frameData.size());

    // Base the widget size on the window height
    // cast float to int to save rounded scale
    QSize frameSize = frameData.size();

    // TODO Why?
    pixelScale = static_cast<int>(float(size().height() - 1)/frameSize.height()*10);
    pixelScale /= 10;

    // If the drawing space is large enough, make a grid pattern to superimpose over the image
    if (pixelScale >= GRID_MIN_Y_SCALE) {
        gridPattern = QImage(frameSize.width()*pixelScale  +.5 + 1,
                             frameSize.height()*pixelScale +.5 + 1,
                             QImage::Format_ARGB32_Premultiplied);
        gridPattern.fill(COLOR_CLEAR);

        QPainter painter(&gridPattern);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setRenderHint(QPainter::Antialiasing, false);

        // Draw vertical lines
        painter.setPen(COLOR_GRID_LINES);
        for (int x = 0; x <= frameSize.width(); x++) {
            painter.drawLine(x*pixelScale+.5,
                             0,
                             x*pixelScale+.5,
                             gridPattern.height());
        }

        // Draw horizontal lines
        for (int y = 0; y <= frameSize.height(); y++) {
            painter.drawLine(0,
                             y*pixelScale+.5,
                             gridPattern.width(),
                             y*pixelScale+.5);
        }
    }
}

void FrameEditor::mousePressEvent(QMouseEvent *event)
{
    if (!hasImage() || instrument.isNull())
        return;

    setCursor(instrument->cursor());
    instrument->mousePressEvent(event, *this, event->pos()/pixelScale);
    lazyUpdate();
}

void FrameEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (!hasImage() || instrument.isNull())
        return;

    // Ignore the update request if it came too quickly
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_MOUSE_INTERVAL)
        return;

    lastTime = newTime;

    static QPoint lastPos;

    QPoint pos = event->pos()/pixelScale;

    // If the position hasn't changed, don't do anything.
    // This is to avoid expensive reprocessing of the tool preview window.
    if (pos == lastPos)
        return;

    lastPos = pos;

    instrument->mouseMoveEvent(event, *this, pos);

    lazyUpdate();
}

void FrameEditor::mouseReleaseEvent(QMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);

    if (!hasImage() || instrument.isNull())
        return;

    instrument->mouseReleaseEvent(event, *this, event->pos()/pixelScale);
    lazyUpdate();
}

void FrameEditor::setToolColor(QColor color)
{
    toolColor = color;
}

void FrameEditor::setToolSize(int size)
{
    toolSize = size;
}

void FrameEditor::setInstrument(AbstractInstrument *pi)
{
    instrument = pi;
}

void FrameEditor::setFixture(Fixture *newFixture)
{
    fixture = newFixture;
}

void FrameEditor::setShowPlaybakIndicator(bool newShowPlaybackIndicator)
{
    showPlaybackIndicator = newShowPlaybackIndicator;
}

void FrameEditor::setFrameData(int index, const QImage data)
{
    // Don't update if we are currently in a draw operation
    if(!instrument.isNull() && instrument->hasPreview())
        return;

    if (data.isNull()) {
        frameData = data;
        frameIndex = index;
        this->setBaseSize(1, 1);
        update();
        return;
    }

    // TODO: Unclear logic
    bool updateSize = (!hasImage() || (data.size() != frameData.size()));

    frameData = data;
    frameIndex = index;

    if (updateSize) {
        updateGridSize();
        // Compute a new viewport size, based on the current viewport height
        float scale = float(size().height())/data.size().height();
        this->setMinimumWidth(data.size().width()*scale);
    }

    // and force a screen update
    update();
}

void FrameEditor::lazyUpdate()
{
    // Ignore the update request if it came too quickly
    static qint64 lastTime = 0;
    qint64 newTime = QDateTime::currentMSecsSinceEpoch();
    if (newTime - lastTime < MIN_UPDATE_INTERVAL)
        return;

    lastTime = newTime;

    update();
}

void FrameEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // If we don't have an image, show an empty view
    if (!hasImage()) {
        painter.fillRect(0, 0, this->width(), this->height(), COLOR_CLEAR);
        return;
    }

    // Draw the image and tool preview
    painter.drawImage(QRect(0, 0,
                            frameData.width()*pixelScale+.5,
                            frameData.height()*pixelScale),
                      frameData);

    if (!instrument.isNull() && instrument->hasPreview())
        painter.drawImage(QRect(0, 0, frameData.width()*pixelScale+.5, frameData.height()*pixelScale),
                          (instrument->getPreview()));

    if (pixelScale >= GRID_MIN_Y_SCALE)
        painter.drawImage(0, 0, gridPattern);

    // TODO: How to do this more generically?
// if(deviceModel->showPlaybackIndicator()) {
    if (!fixture.isNull() && showPlaybackIndicator) {
// int playbackRow = deviceModel->getFrameIndex();
// const float outputScale = deviceModel->getDisplaySize().width()*xScale;

        int playbackRow = frameIndex;
        int fixtureWidth = fixture->getSize().width();
        int fixtureHeight = fixture->getSize().height();

        // Draw the playback indicator
        // Note that we need to compute the correct width based on the rounding error of
        // the current cell, otherwise it won't line up correctly with the actual image.
        painter.setPen(COLOR_PLAYBACK_EDGE);
        painter.drawRect(playbackRow*pixelScale +.5,
                         0,
                         int((playbackRow+fixtureWidth)*pixelScale +.5) - int(playbackRow*pixelScale +.5),
                         fixtureHeight*pixelScale);
        painter.fillRect(playbackRow*pixelScale +.5,
                         0,
                         int((playbackRow+fixtureWidth)*pixelScale +.5) - int(playbackRow*pixelScale +.5),
                         fixtureHeight*pixelScale,
                         COLOR_PLAYBACK_TOP);

        painter.drawRect((playbackRow-frameData.width())*pixelScale +.5,
                         0,
                         int((playbackRow+fixtureWidth)*pixelScale +.5) - int(playbackRow*pixelScale +.5),
                         fixtureHeight*pixelScale);
        painter.fillRect((playbackRow-frameData.width())*pixelScale +.5,
                         0,
                         int((playbackRow+fixtureWidth)*pixelScale +.5) - int(playbackRow*pixelScale +.5),
                         fixtureHeight*pixelScale,
                         COLOR_PLAYBACK_TOP);
    }
}

void FrameEditor::applyInstrument(QImage &update)
{
    QPainter painter;
    painter.begin(&frameData);
    painter.drawImage(0, 0, update);
    painter.end();

    emit(dataEdited(frameIndex, frameData));
}


QColor FrameEditor::getPrimaryColor() const
{
    return toolColor;
}

int FrameEditor::getPenSize() const
{
    return toolSize;
}
