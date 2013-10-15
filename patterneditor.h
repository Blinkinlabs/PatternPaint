#ifndef PATTERNEDITOR_H
#define PATTERNEDITOR_H

#include <QWidget>

class PatternEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PatternEditor(QWidget *parent = 0);

    // TODO: Destructor to release pattern

    void init(int width, int height);

    void init(QString filename);

    QImage *getPattern() { return pattern; }

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage *pattern;        // The actual image
    QImage *gridPattern;    // Holds the pre-rendered grid overlay
    QImage *toolPreview;    // Holds a preview of the current tool
    int scale;

    QColor *toolColor;
    int toolSize;


    int playbackRow;

signals:

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);
    void setPlaybackRow(int row);
};

#endif // PATTERNEDITOR_H
