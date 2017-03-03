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
    scaleMode(FIT_TO_SCREEN),
    frameIndex(0),
    mouseMoveIntervalFilter(MIN_MOUSE_INTERVAL),
    showPlaybackIndicator(false)
{
    setMouseTracking(true);

    // Grab pinch gestures
    grabGesture(Qt::PinchGesture);
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

    if(scaleMode != FIXED_SIZE) {
        setScaleMode(FIXED_SIZE);
    }
    else {
        updateSize();
        update();
    }
}

void FrameEditor::setScaleMode(FrameEditor::ScaleMode newScaleMode)
{
    if(scaleMode == newScaleMode)
        return;

    qDebug() << "newScaleMode:" << newScaleMode;
    scaleMode = newScaleMode;

    emit(fitToHeightChanged(scaleMode == FIT_TO_HEIGHT));
    emit(fitToWidthChanged(scaleMode == FIT_TO_WIDTH));
    emit(fitToScreenChanged(scaleMode == FIT_TO_SCREEN));

    updateSize();
    update();
}

void FrameEditor::setFitToHeight(bool selected)
{
    if(selected)
        setScaleMode(FIT_TO_HEIGHT);
    else
        setScaleMode(FIXED_SIZE);
}

void FrameEditor::setFitToWidth(bool selected)
{
    if(selected)
        setScaleMode(FIT_TO_WIDTH);
    else
        setScaleMode(FIXED_SIZE);
}

void FrameEditor::setFitToScreen(bool selected)
{
    if(selected)
        setScaleMode(FIT_TO_SCREEN);
    else
        setScaleMode(FIXED_SIZE);
}

void FrameEditor::resizeEvent(QResizeEvent *resizeEvent)
{
    updateSize();

    QWidget::resizeEvent(resizeEvent);
}

void FrameEditor::updateSize()
{
    if(!hasImage()) {
        // Degenerate case- set our size to 0 to prevent the widget from drawing
        setMinimumSize(QSize(0,0));
        setMaximumSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));

        // TODO: Do we need to clear anything else here?
    }
    else if(scaleMode == FIT_TO_HEIGHT){
        // In fit-to-height mode, the image height should be maximized to fill the
        // QScrollArea container. Unfortunately the available height of the QScrollArea
        // depends on whether the scroll bar is visible or not, which depends on the
        // scaled width of the frameData. The strategy then is to determine if the
        // scroll bar should be drawn or not, based on the aspect ratio of the image.

        scale = float(size().height())/frameData.height();

        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        setMinimumSize(frameData.width()*scale, 1);
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

        (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // Finally, force the horizontal scroll bar on or off depending on the parent widget size. If the scroll
        // bar state changes, it will cause the size function to be called again.
        QSize scrollAreaSize = (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->size();

        float imageAspectRatio = float(frameData.width())/frameData.height();
        float parentAspectRatio = float(scrollAreaSize.width())/scrollAreaSize.height();

        if(imageAspectRatio > parentAspectRatio)
            (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        else
            (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else if(scaleMode == FIT_TO_WIDTH){
        // In fit-to-height mode, the image height should be maximized to fill the
        // QScrollArea container. Unfortunately the available height of the QScrollArea
        // depends on whether the scroll bar is visible or not, which depends on the
        // scaled width of the frameData. The strategy then is to determine if the
        // scroll bar should be drawn or not, based on the aspect ratio of the image.

        scale = float(size().width())/frameData.width();

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setMinimumSize(1, frameData.height()*scale);
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

        (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // Finally, force the horizontal scroll bar on or off depending on the parent widget size. If the scroll
        // bar state changes, it will cause the size function to be called again.
        QSize scrollAreaSize = (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->size();

        float imageAspectRatio = float(frameData.width())/frameData.height();
        float parentAspectRatio = float(scrollAreaSize.width())/scrollAreaSize.height();

        if(imageAspectRatio < parentAspectRatio)
            (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        else
            (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else if(scaleMode == FIT_TO_SCREEN){
        // In fit-to-height mode, the image height should be maximized to fill the
        // QScrollArea container. Unfortunately the available height of the QScrollArea
        // depends on whether the scroll bar is visible or not, which depends on the
        // scaled width of the frameData. The strategy then is to determine if the
        // scroll bar should be drawn or not, based on the aspect ratio of the image.

        // Finally, force the horizontal scroll bar on or off depending on the parent widget size. If the
        // scroll bar state changes, it will cause the size function to be called again.
        QSize scrollAreaSize = (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->size();

        float imageAspectRatio = float(frameData.width())/frameData.height();
        float parentAspectRatio = float(scrollAreaSize.width())/scrollAreaSize.height();

        // NOTE: Setting a fixed policy here for one direction seems to be necessicary to keep the image
        // centered, however it introduces a bug in the resize behavior (at least on Windows): if the
        // resize happens only in the direction that the object is fixed, then a resizeEvent won't be
        // passed to the widget.
        if(imageAspectRatio < parentAspectRatio) {
            scale = float(size().height())/frameData.height();
            setMinimumSize(frameData.width()*scale, 1);
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        }
        else {
            scale = float(size().width())/frameData.width();
            setMinimumSize(1, frameData.height()*scale);
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }

        setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));

        (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else {
        // Set the widget to a fixed size based on the frame data size and scale
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setMinimumSize(frameData.size()*scale);
        setMaximumSize(frameData.size()*scale);

        (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        (static_cast<QScrollArea *>(parentWidget()->parentWidget()))->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

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

void FrameEditor::enterEvent(QEvent *event)
{
    // Force a screen update here, to draw the instrument preview
    update();
    QWidget::enterEvent(event);
}

void FrameEditor::leaveEvent(QEvent *event)
{
    // Force a screen update here, to hide the instrument preview
    update();
    QWidget::leaveEvent(event);
}

void FrameEditor::mousePressEvent(QMouseEvent *event)
{
    if (!hasImage() || instrument.isNull())
        return;

    instrument->mousePressEvent(event, frameData, frameToImage(event->x(),event->y()));
    lazyUpdate();
}

void FrameEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (!hasImage() || instrument.isNull())
        return;

    if(!mouseMoveIntervalFilter.check())
        return;

    QPoint mousePoint = frameToImage(event->x(),event->y());

    // Filter the move event if it didn't result in a move to a new image pixel
    if (mousePoint == lastMousePoint)
        return;

    lastMousePoint = mousePoint;

    instrument->mouseMoveEvent(event, frameData, frameToImage(event->x(),event->y()));
    lazyUpdate();
}

void FrameEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (!hasImage() || instrument.isNull())
        return;

    instrument->mouseReleaseEvent(event, *this, frameData, frameToImage(event->x(),event->y()));
    lazyUpdate();
}

void FrameEditor::setInstrument(AbstractInstrument *pi)
{
    instrument = pi;
    setCursor(instrument->getCursor());
}

void FrameEditor::setFixture(Fixture *newFixture)
{
    fixture = newFixture;
}

void FrameEditor::setShowPlaybakIndicator(bool newShowPlaybackIndicator)
{
    showPlaybackIndicator = newShowPlaybackIndicator;
}

void FrameEditor::setEditImage(int index, const QImage &data)
{
    // Don't update if we are currently in a draw operation
    if(!instrument.isNull() && instrument->isDrawing())
        return;

    // TODO: Unclear logic
    if (data.isNull()) {
        frameData = data;
        frameIndex = index;

        updateSize();
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
    static IntervalFilter rateLimiter(MIN_UPDATE_INTERVAL);

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

    if (!instrument.isNull() && underMouse()) {
        painter.drawImage(QRect(0, 0,
                                frameData.width()*scale,
                                frameData.height()*scale),
                          (instrument->getPreview()));
    }

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



void FrameEditor::applyInstrument(QImage &newImage)
{
    QPainter painter;
    painter.begin(&frameData);
    painter.drawImage(0, 0, newImage);
    painter.end();

    emit(dataEdited(frameIndex, frameData));
}

void FrameEditor::pinchTriggered(QPinchGesture *gesture)
{
    if(gesture->changeFlags() & QPinchGesture::ScaleFactorChanged) {
        setScale(scale * gesture->scaleFactor());
    }
}

bool FrameEditor::event(QEvent *event)
{
    // If it's a scroll event with the ctrl modifier, capture it
    // In Windows 10, this also captures pinch events
    if(event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if(wheelEvent->modifiers().testFlag(Qt::ControlModifier)) {
            setScale(scale*(1+(wheelEvent->delta())/1200.0));

            wheelEvent->accept();
            return true;
        }
    }

    // On macOS, this captures pinch events
    else if(event->type() == QEvent::Gesture) {
        QGestureEvent *gestureEvent = static_cast<QGestureEvent*>(event);

        if(QGesture *pinch = gestureEvent->gesture(Qt::PinchGesture)) {
            pinchTriggered(static_cast<QPinchGesture *>(pinch));

            gestureEvent->accept();
            return true;
        }
    }

    return QWidget::event(event);
}
