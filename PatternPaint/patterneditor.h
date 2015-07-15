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

    void setPatternItem(PatternItem* newPatternItem);

    void setImage(const QImage& img);

    /// Get the image data for the current pattern
    /// @return QImage containing the current pattern
    const QImage& getPatternAsImage() const;
    QImage* getPattern();

    // for compatibility with EasyPaint sources
    QColor getPrimaryColor() const { return toolColor; }
    int getPenSize() const { return toolSize; }

    /// Re-size the display grid and selector based on new widget geometery
    void resizeEvent(QResizeEvent * event);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent*);

    void leaveEvent(QEvent * event);

    void pushUndoState();

    bool isPaint() const { return m_isPaint; }
    void setPaint(bool paint) { m_isPaint = paint; }
protected:
    void paintEvent(QPaintEvent *event);

private:
    PatternItem* patternItem;    ///< Pattern item we are interacting with
    QImage* pattern;        ///< The actual image, stored in the patternItem (TODO: Should we be referenceing this indirectly instead?

    QImage gridPattern;    ///< Holds the pre-rendered grid overlay
    QImage toolPreview;    ///< Holds a preview of the current tool

    float xScale;          ///< Number of pixels in the grid pattern per pattern pixel.
    float yScale;          ///< Number of pixels in the grid pattern per pattern pixel.

    QColor toolColor;      ///< Color of the current drawing tool (TODO: This should be a pointer to a tool)
    int toolSize;          ///< Size of the current drawing tool (TODO: This should be a pointer to a tool)

    int playbackRow;       ///< Row being played back (for display purposes only)

    bool m_isPaint;
    AbstractInstrument* m_pi;

    /// Redraw the gridPattern to fit the current widget size.
    void updateGridSize();

    /// Update the screen, but only if we haven't done so in a while
    void lazyUpdate();

signals:
    void resized();

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);
    void setPlaybackRow(int row);
    void setInstrument(AbstractInstrument*);
};

#endif // PATTERNDITOR_H
