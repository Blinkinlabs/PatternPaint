#include "frameeditor.h"
#include "abstractinstrument.h"
#include "eventratelimiter.h"

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


QPoint FrameEditor::frameToImage(const int &framePointX, const int &framePointY ) const {
        int x = framePointX/pixelScale;
        int y = framePointY/pixelScale;
        return QPoint(x,y);
}

QPoint FrameEditor::imageToFrame(const QPoint &imagePoint) const {
    return imagePoint*pixelScale;
}

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

bool FrameEditor::hasImage() const
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

    pixelScale = float(size().height())/frameSize.height();

    // If the drawing space is large enough, make a grid pattern to superimpose over the image
    if (pixelScale >= GRID_MIN_Y_SCALE) {
        gridPattern = QImage(frameSize.width()*pixelScale,
                             frameSize.height()*pixelScale,
                             QImage::Format_ARGB32_Premultiplied);
        gridPattern.fill(COLOR_CLEAR);

        QPainter painter(&gridPattern);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
        painter.setRenderHint(QPainter::Antialiasing, false);

        // Draw vertical lines
        painter.setPen(COLOR_GRID_LINES);
        for (int x = 0; x <= frameSize.width(); x++) {
            painter.drawLine(std::round(x*pixelScale),
                             0,
                             std::round(x*pixelScale),
                             gridPattern.height());
        }

        // Draw horizontal lines
        for (int y = 0; y <= frameSize.height(); y++) {
            painter.drawLine(0,
                             std::round(y*pixelScale),
                             gridPattern.width(),
                             std::round(y*pixelScale));
        }
    }
}

void FrameEditor::mousePressEvent(QMouseEvent *event)
{
    if (!hasImage() || instrument.isNull())
        return;

    instrument->mousePressEvent(event, *this, frameToImage(event->x(),event->y()));
    lazyUpdate();
}

void FrameEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (!hasImage() || instrument.isNull())
        return;

    // Filter the move event if it came too quickly
    static intervalFilter rateLimiter(MIN_MOUSE_INTERVAL);

    if(!rateLimiter.check())
        return;

    QPoint mousePoint = frameToImage(event->x(),event->y());

    // Filter the move event if it didn't result in a move to a new image pixel
    if (mousePoint == lastMousePoint)
        return;

    lastMousePoint = mousePoint;

    instrument->mouseMoveEvent(event, *this, mousePoint);

    lazyUpdate();
}

void FrameEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (!hasImage() || instrument.isNull())
        return;

    instrument->mouseReleaseEvent(event, *this, frameToImage(event->x(),event->y()));
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
    setCursor(instrument->cursor());
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
    static intervalFilter rateLimiter(MIN_UPDATE_INTERVAL);

    if(!rateLimiter.check())
        return;

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
    // TODO: Why +1 here?
    painter.drawImage(QRect(0, 0,
                            frameData.width()*pixelScale + 1,
                            frameData.height()*pixelScale + 1),
                      frameData);

    if (!instrument.isNull() && instrument->hasPreview())
        painter.drawImage(QRect(0, 0,
                                frameData.width()*pixelScale + 1,
                                frameData.height()*pixelScale + 1),
                          (instrument->getPreview()));

    if (pixelScale >= GRID_MIN_Y_SCALE)
        painter.drawImage(0, 0, gridPattern);

    // TODO: How to do this more generically?
    if (!fixture.isNull() && showPlaybackIndicator) {

        int playbackRow = frameIndex;
        int fixtureWidth = fixture->getExtents().width();
        int fixtureHeight = fixture->getExtents().height();

        // Draw the playback indicator
        painter.setPen(COLOR_PLAYBACK_EDGE);

        QPoint topLeft = imageToFrame(QPoint(playbackRow, 0));
        QPoint bottomRight = imageToFrame(QPoint(playbackRow + fixtureWidth, fixtureHeight)) - QPoint(1,0);

        painter.drawRect(QRect(topLeft, bottomRight));
        painter.fillRect(QRect(topLeft, bottomRight), COLOR_PLAYBACK_TOP);

        // In the case that the indicator is split, draw the other half
        topLeft = imageToFrame(QPoint(playbackRow-frameData.width(),0));
        bottomRight = imageToFrame(QPoint(playbackRow - frameData.width() + fixtureWidth, fixtureHeight)) - QPoint(1,0);

        painter.drawRect(QRect(topLeft, bottomRight));
        painter.fillRect(QRect(topLeft, bottomRight), COLOR_PLAYBACK_TOP);
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
