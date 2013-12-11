#ifndef ABOUTPATTERNPAINT_H
#define ABOUTPATTERNPAINT_H

#include <QDialog>

namespace Ui {
class AboutPatternPaint;
}

class AboutPatternPaint : public QDialog
{
    Q_OBJECT

public:
    explicit AboutPatternPaint(QWidget *parent = 0);
    ~AboutPatternPaint();

private:
    Ui::AboutPatternPaint *ui;
};

#endif // ABOUTPATTERNPAINT_H
