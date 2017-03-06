#ifndef PATTERNDITOR_H
#define PATTERNDITOR_H

#include "fixture.h"
#include "pattern.h"
#include "intervalfilter.h"

#include <QWidget>
#include <QPointer>
#include <QGesture>

class QUndoStack;
class PatternFrameUndoCommand;
class AbstractInstrument;

class FrameEditor : public QWidget
{
    Q_OBJECT
public:
    explicit FrameEditor(QWidget *parent = 0);

    /// Re-size the display grid and selector based on new widget geometery
    void resizeEvent(QResizeEvent *resizeEvent);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    // Pass messages to the selected instrument
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    enum ScaleMode {
        FIXED_SIZE,
        FIT_TO_HEIGHT,
        FIT_TO_WIDTH,
        FIT_TO_SCREEN
    };

public:

    /// Instrument interface

    /// Update the pattern with the given changes.
    /// @param update RGBA QImage to draw on top of the current
    void applyInstrument(QImage &newImage);

private:
    float scale;            ///< User-controlled scale factor
    ScaleMode scaleMode;
    bool fitToHeight;

    QImage frameData;       ///< Frame image
    int frameIndex;         ///< Frame index

    QImage gridPattern;    ///< Holds the pre-rendered grid overlay

    IntervalFilter lazyUpdateIntervalFilter;
    IntervalFilter mouseMoveIntervalFilter;
    QPoint lastMousePoint;

    QPointer<AbstractInstrument> instrument;

    bool showPlaybackIndicator;

    QPointer<Fixture> fixture;

    /// Convert a coordinte from frame coordinate to image coordinate
    QPoint frameToImage(const int framePointX, const int framePointY) const;
    QPoint imageToFrame(const QPoint &imagePoint) const;

    /// Redraw the gridPattern to fit the current widget size.
    void updateGrid();

    /// Update the screen, but only if we haven't done so in a while
    void lazyUpdate();

    /// True if the editor has an image to edit
    bool hasImage() const;

    void updateSize();

    void setScale(float newScale);

    void setScaleMode(FrameEditor::ScaleMode newScaleMode);

    void paintEvent(QPaintEvent *event);

    bool event(QEvent *event);

    void pinchTriggered(QPinchGesture *gesture);

signals:
    void dataEdited(int index, const QImage &data);

    void fitToHeightChanged(bool);
    void fitToWidthChanged(bool);
    void fitToScreenChanged(bool);

public slots:
    void setInstrument(AbstractInstrument *);
    void setShowPlaybakIndicator(bool);

    void setFixture(Fixture *);
    void setEditImage(int index, const QImage &data);

    void zoomIn();
    void zoomOut();
    void setFitToHeight(bool selected);
    void setFitToWidth(bool selected);
    void setFitToScreen(bool selected);
};

#endif // PATTERNDITOR_H
