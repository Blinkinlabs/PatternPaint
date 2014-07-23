#ifndef ANIMATIONEDITOR_H
#define ANIMATIONEDITOR_H

#include <QWidget>

class AnimationEditor : public QWidget
{
    Q_OBJECT
public:
    explicit AnimationEditor(QWidget *parent = 0);

    /// Re-initialze the animation editor as a blank image with the given size
    /// @param frameCount Number of frames in this animation
    /// @param stripLength Number of LEDs in this strip
    void init(int frameCount, int stripLength);

    /// Initialize the animation editor using a QImage as the new animation
    /// @param newPattern New animation to load
    /// @param scaled If true, scale the image to match the height of the previous animation
    bool init(QImage newPattern, bool scaled = true);

    QImage getPattern() { return pattern; }

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

    void leaveEvent(QEvent * event);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage pattern;        // The actual image
    QImage gridPattern;    // Holds the pre-rendered grid overlay
    QImage toolPreview;    // Holds a preview of the current tool
    int xScale;
    int yScale;

    QColor toolColor;
    int toolSize;

    int playbackRow;

signals:

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);
    void setPlaybackRow(int row);
};

#endif // ANIMATIONEDITOR_H
