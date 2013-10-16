#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>


/// Color picker implements a color selection widget, which displays an assortment of colors
/// and emits a signal when one is selected.
class ColorPicker : public QWidget
{
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = 0);

    // Initialize the color picker
    // TODO: Do we actually need to do this?
    void init();

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QImage colorImage;
    QColor activeColor;

    void drawRect( int rx, int ry, int rw, int rh, QColor rc );
    void setGradient(int x, int y, float h, QColor c1, QColor c2 );

    void setNewColor(QColor color);
signals:
    // Emitted when the user updates the currently selected color.
    /// @param color Selected color
    void colorChanged(QColor color);
public slots:

};

#endif // COLORPICKER_H
