#ifndef RESIZEPATTERN_H
#define RESIZEPATTERN_H

#include <QDialog>

namespace Ui {
class ResizePattern;
}

/// Dialog to allow the user to specify a new width/height for the animation
class ResizePattern : public QDialog
{
    Q_OBJECT

public:
    explicit ResizePattern(QWidget *parent = 0);
    ~ResizePattern();

    void setLength(int length);
    int length();

    void setLedCount(int ledCount);
    int ledCount();

private:
    Ui::ResizePattern *ui;
};

#endif // RESIZEPATTERN_H
