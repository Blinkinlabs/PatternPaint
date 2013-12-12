#ifndef RESIZEANIMATION_H
#define RESIZEANIMATION_H

#include <QDialog>

namespace Ui {
class ResizeAnimation;
}

class ResizeAnimation : public QDialog
{
    Q_OBJECT

public:
    explicit ResizeAnimation(QWidget *parent = 0);
    ~ResizeAnimation();

    void setLength(int length);
    int length();

private:
    Ui::ResizeAnimation *ui;
};

#endif // RESIZEANIMATION_H
