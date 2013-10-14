#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>

class ColorPicker : public QWidget
{
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = 0);

    void init();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage *colorImage;
    QColor *activeColor;

    void drawRect( int rx, int ry, int rw, int rh, QColor rc );
    void setGradient(int x, int y, float w, float h, QColor c1, QColor c2 );

    void setNewColor(QColor color);
signals:
    void colorChanged(QColor color);
public slots:

};

#endif // COLORPICKER_H
