#ifndef PATTERNDITOR_H
#define PATTERNDITOR_H

#include <QWidget>

class PatternEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PatternEditor(QWidget *parent = 0);

    /// Re-initialze the pattern editor as a blank image with the given size
    /// @param frameCount Number of frames in this pattern
    /// @param stripLength Number of LEDs in this strip
    void init(int frameCount, int stripLength);

    /// Initialize the pattern editor using a QImage as the new pattern
    /// @param newPattern New pattern to load
    /// @param scaled If true, scale the image to match the height of the previous pattern
    bool init(QImage newPattern, bool scaled = true);

    /// Get the image data for the current pattern
    /// @return QImage containing the current pattern
    QImage getPatternAsImage() { return pattern; }

    /// Re-size the display grid and selector based on new widget geometery
    void resizeEvent(QResizeEvent * event);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

    void leaveEvent(QEvent * event);

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

    /// Redraw the gridPattern to fit the current widget size.
    void updateGridSize();

signals:

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);
    void setPlaybackRow(int row);
};

#endif // PATTERNDITOR_H
