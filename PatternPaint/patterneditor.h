#ifndef PATTERNDITOR_H
#define PATTERNDITOR_H

#include <QWidget>

class QUndoStack;
class UndoCommand;
class AbstractInstrument;

class PatternEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PatternEditor(QWidget *parent = 0);

    /// Re-initialze the pattern editor as a blank image with the given size
    /// @param frameCount Number of frames in this pattern
    /// @param stripLength Number of LEDs in this strip
    /// @param initTools - temporary parameter - set tools parameters like size and color to default
    void init(int frameCount, int stripLength, bool initTools = true);

    /// Initialize the pattern editor using a QImage as the new pattern
    /// @param newPattern New pattern to load
    /// @param scaled If true, scale the image to match the height of the previous pattern
    bool init(QImage newPattern, bool scaled = true);

    // TODO - correct this method!
    void resetImage(const QImage& img);

    void setImage(const QImage& img) { pattern = img; }

    inline QUndoStack* getUndoStack() { return m_undoStack; }
    inline qreal getZoomFactor() const { return 1.0; }

    /// Get the image data for the current pattern
    /// @return QImage containing the current pattern
    QImage getPatternAsImage() { return pattern; }
    QImage* getDevice() { return &pattern; }

    // for compatibility with EasyPaint sources
    QColor getPrimaryColor() const { return toolColor; }
    int getPenSize() const { return toolSize; }

    /// Re-size the display grid and selector based on new widget geometery
    void resizeEvent(QResizeEvent * event);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent*);

    void leaveEvent(QEvent * event);

    void pushUndoCommand(UndoCommand *command);
    bool isPaint() const { return m_isPaint; }
    void setPaint(bool paint) { m_isPaint = paint; }

    float scaleX() const { return xScale; }
    float scaleY() const { return yScale; }
protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage pattern;        ///< The actual image
    QImage gridPattern;    ///< Holds the pre-rendered grid overlay
    QImage toolPreview;    ///< Holds a preview of the current tool

    float xScale;          ///< Number of pixels in the grid pattern per pattern pixel.
    float yScale;          ///< Number of pixels in the grid pattern per pattern pixel.

    QColor toolColor;      ///< Color of the current drawing tool (TODO: This should be a pointer to a tool)
    int toolSize;          ///< Size of the current drawing tool (TODO: This should be a pointer to a tool)

    int playbackRow;       ///< Row being played back
    QUndoStack* m_undoStack;
    bool m_isPaint;
    AbstractInstrument* m_pi;

    /// Redraw the gridPattern to fit the current widget size.
    void updateGridSize();

    /// Update the screen, but only if we haven't done so in a while
    void lazyUpdate();
    void applyTool(int x, int y);
    void updateToolPreview(int x, int y);
signals:

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);
    void setPlaybackRow(int row);
    void setInstrument(AbstractInstrument*);
};

#endif // PATTERNDITOR_H
