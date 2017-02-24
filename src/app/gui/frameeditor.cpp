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

#define ZOOM_MIN                1
#define ZOOM_MAX                100
#define ZOOM_STEP               .25

QPoint FrameEditor::frameToImage(const int framePointX, const int framePointY ) const {
        int x = framePointX/scale;
        int y = framePointY/scale;
        return QPoint(x,y);
}

QPoint FrameEditor::imageToFrame(const QPoint &imagePoint) const {
    return imagePoint*scale;
}

FrameEditor::FrameEditor(QWidget *parent) :
    QWidget(parent),
    scale(ZOOM_MIN),
    fitToWindow(true),
    frameIndex(0),
    showPlaybackIndicator(false)
{
    // Turn on mouse tracking so we can draw a preview
    // TODO: DO we need to do this here, or just in the constructor?
    setMouseTracking(true);
}

bool FrameEditor::hasImage() const
{
    return !frameData.isNull();
}

void FrameEditor::zoomIn()
{
    setScale(scale*(1+ZOOM_STEP));
}

void FrameEditor::zoomOut()
{
    setScale(scale*(1-ZOOM_STEP));
}

void FrameEditor::setScale(float newScale)
{
    if (newScale <= ZOOM_MIN)
        newScale = ZOOM_MIN;

    if (newScale >= ZOOM_MAX)
        newScale = ZOOM_MAX;

    scale = newScale;

    if(fitToWindow) {
        fitToWindow = false;
        emit(zoomToFitChanged(fitToWindow));
    }

    updateSize();
    update();
}

void FrameEditor::zoomToFit(bool newFitToWindow)
{
    if(newFitToWindow == fitToWindow)
        return;

    fitToWindow = newFitToWindow;
    emit(zoomToFitChanged(fitToWindow));

    updateSize();
    update();
}

const QImage &FrameEditor::getPatternAsImage() const
{
    return frameData;
}

void FrameEditor::resizeEvent(QResizeEvent *resizeEvent)
{
    qDebug() << "Resize event"
             << "scale:" << scale;

    updateSize();

    QWidget::resizeEvent(resizeEvent);
}

void FrameEditor::updateSize()
{
    // There are three inputs:
    // hasImage(): (bool) true if we have an image to scale
    // fitToWindow: (bool) true if we should dynamically adjust 'scale' to fit
    //    the image height to the container (scroll area) size
    // frameData.size(): base resolution of the image we are editing
    // scale: (float) % to scale the image

    // And x outputs:
    // scale: % the frameData image is scaled by when drawing. Only adjusted
    //    here if fitToWindow is true

    if(!hasImage()) {
        // Degenerate case- set our size to 0 to prevent the widget from drawing
        setBaseSize(QSize());

        // TODO: Do we need to clear anything else here?
    }
    else if(fitToWindow){
        // We want to scale the image to fit the height of the QScrollArea viewport,
        // and force the width based on the aspect ratio.
        scale = float(size().height())/frameData.height();

        // TODO: There's a recursion bug with this, that is triggered when the horizontal size
        // is just wider than the viewport. This code tells the viewport to resize, which then
        // turns on the scroll bars, which then makes the viewport just a little smaller, which then
        // calls this code again, which then resizes so that it's just small enough to not need the scroll
        // bars, and so on.
        // This is a hacky workaround to try and stop this after a single cycle.
        static QSize lastQScrollAreaSize;
        if(lastQScrollAreaSize != parentWidget()->parentWidget()->size()) {
            lastQScrollAreaSize = parentWidget()->parentWidget()->size();

            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            setMinimumSize(frameData.width()*scale, 1);
            setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            setBaseSize(QSize(0,0));
        }
    }
    else {
        // Set the widget to a fixed size based on the frame data size and scale
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setBaseSize(frameData.size()*scale);
        setMinimumSize(baseSize());
        setMaximumSize(baseSize());
    }

    qDebug() << "fitToWindow:" << fitToWindow
             << "baseSize:" << baseSize()
             << "QScrollArea_viewport.size():" << parentWidget()->size()
             << "QScrollArea.size():" << parentWidget()->parentWidget()->size()
             << "scale:" << scale;

    updateGrid();
}

void FrameEditor::updateGrid()
{
    if (!hasImage())
        return;

    if (scale < GRID_MIN_Y_SCALE) {
        gridPattern = QImage();
        return;
    }

    gridPattern = QImage(frameData.width()*scale,
                         frameData.height()*scale,
                         QImage::Format_ARGB32_Premultiplied);
    gridPattern.fill(COLOR_CLEAR);

    QPainter painter(&gridPattern);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Draw vertical lines
    painter.setPen(COLOR_GRID_LINES);
    for (int x = 0; x <= frameData.width(); x++)
        painter.drawLine(x*scale,
                         0,
                         x*scale,
                         gridPattern.height());

    // Draw horizontal lines
    for (int y = 0; y <= frameData.height(); y++)
        painter.drawLine(0,
                         y*scale,
                         gridPattern.width(),
                         y*scale);
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

void FrameEditor::setFrameData(int index, const QImage &data)
{
    // Don't update if we are currently in a draw operation
    if(!instrument.isNull() && instrument->hasPreview())
        return;

    // TODO: Unclear logic
    if (data.isNull()) {
        frameData = data;
        frameIndex = index;
        this->setBaseSize(1, 1);
        update();
        return;
    }

    bool sizeChanged = (!hasImage() || (data.size() != frameData.size()));

    frameData = data;
    frameIndex = index;

    if (sizeChanged)
        updateSize();

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
    painter.drawImage(QRect(0, 0,
                            frameData.width()*scale,
                            frameData.height()*scale),
                      frameData);

    if (!instrument.isNull() && instrument->hasPreview())
        painter.drawImage(QRect(0, 0,
                                frameData.width()*scale,
                                frameData.height()*scale),
                          (instrument->getPreview()));

    if (scale >= GRID_MIN_Y_SCALE)
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

bool FrameEditor::event(QEvent *event)
{
    // If it's a scroll event with the ctrl modifier, capture it
    // In Windows 10, somehow pinch gestures are being captured here too :-/
    // TODO: How does this look on macOS and Linux?
    if(event->type() == QEvent::Wheel) {
        QWheelEvent *e = static_cast<QWheelEvent*>(event);
        if(e->modifiers().testFlag(Qt::ControlModifier)) {
            setScale(scale*(1+(e->delta())/1200.0));
            e->accept();
            return true;
        }
    }

    return QWidget::event(event);
}
