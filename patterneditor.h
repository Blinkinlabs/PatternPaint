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
    QImage *pattern;
    QImage *gridPattern;

    QColor *activeColor;
    int size;

    int scale;

signals:

public slots:
    void setToolColor(QColor color);
    void setToolSize(int size);
};

#endif // PATTERNEDITOR_H
