#ifndef RESIZEPATTERN_H
#define RESIZEPATTERN_H

#include <QDialog>

namespace Ui {
class ResizeDialog;
}

/// Dialog to allow the user to specify a new width/height for the animation
class ResizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResizeDialog(QWidget *parent = 0);
    ~ResizeDialog();

    void setOutputSize(QSize size);
    QSize getOutputSize();

private:
    Ui::ResizeDialog *ui;
};

#endif // RESIZEPATTERN_H
