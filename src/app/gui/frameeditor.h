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

    // Pass messages to the selected instrument
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

public:

    /// Instrument interface

    /// Get the image data for the current pattern
    /// @return QImage containing the current pattern
    const QImage &getPatternAsImage() const;

    /// Update the pattern with the given changes.
    /// @param update RGBA QImage to draw on top of the current
    void applyInstrument(QImage &update);

private:
    float scale;            ///< User-controlled scale factor
    bool fitToHeight;

    QImage frameData;       ///< Frame image
    int frameIndex;         ///< Frame index

    QImage gridPattern;    ///< Holds the pre-rendered grid overlay

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

    void paintEvent(QPaintEvent *event);

    bool event(QEvent *event);

    void pinchTriggered(QPinchGesture *gesture);

signals:
    void dataEdited(int index, const QImage &data);
    void zoomToFitChanged(bool enabled);

public slots:
    void setInstrument(AbstractInstrument *);
    void setFixture(Fixture *);
    void setShowPlaybakIndicator(bool);

    void setFrameData(int index, const QImage &data);

    void zoomIn();
    void zoomOut();
    void zoomToFit(bool newFitToHeight);
};

#endif // PATTERNDITOR_H
