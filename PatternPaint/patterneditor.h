#ifndef PATTERNDITOR_H
#define PATTERNDITOR_H

#include <QWidget>
#include "patternitem.h"

class QUndoStack;
class UndoCommand;
class AbstractInstrument;

class PatternEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PatternEditor(QWidget *parent = 0);

    /// Set the patternItem to edit
    /// @param newPatternItem Pattern
    void setPatternItem(PatternItem* newPatternItem);

    /// Get the image data for the current pattern
    /// @return QImage containing the current pattern
    const QImage& getPatternAsImage() const;

    /// Instrument interface

    QColor getPrimaryColor() const { return toolColor; }
    int getPenSize() const { return toolSize; }

    /// Update the pattern with the given changes. Pushes the undo state first.
    /// @param update RGBA QImage to draw on top of the current
    void applyInstrument(QImage& update);

    /// Re-size the display grid and selector based on new widget geometery
    void resizeEvent(QResizeEvent * event);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent*);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
protected:
    void paintEvent(QPaintEvent *event);

private:
    PatternItem* patternItem;    ///< Pattern item we are interacting with

    QImage gridPattern;    ///< Holds the pre-rendered grid overlay

    float xScale;          ///< Number of pixels in the grid pattern per pattern pixel.
    float yScale;          ///< Number of pixels in the grid pattern per pattern pixel.

    QColor toolColor;      ///< Color of the current drawing tool (TODO: This should be a pointer to a tool)
    int toolSize;          ///< Size of the current drawing tool (TODO: This should be a pointer to a tool)

    int playbackRow;       ///< Row being played back (for display purposes only)

    AbstractInstrument* instrument;

    /// Redraw the gridPattern to fit the current widget size.
    void updateGridSize();

    /// Update the screen, but only if we haven't done so in a while
    void lazyUpdate();

signals:
    void forcePatternEditorRedraw();

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);
    void setPlaybackRow(int row);
    void setInstrument(AbstractInstrument*);
};

#endif // PATTERNDITOR_H
