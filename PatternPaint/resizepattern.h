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

    void setFrameCount(int getFrameCount);
    int getFrameCount();

    void setOutputSize(QSize size);
    QSize getOutputSize();

private:
    Ui::ResizePattern *ui;
};

#endif // RESIZEPATTERN_H
