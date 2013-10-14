#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "blinkytape.h"
#include "patterneditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    BlinkyTape tape;

private slots:
    void on_connectButton_clicked();

    void on_disconnectButton_clicked();

    void drawTimerTimeout();

    void on_animationSpeed_valueChanged(int value);

    void on_animationPlayPause_clicked();

    void on_actionLoad_Image_triggered();

private:
    Ui::MainWindow *ui;

    QTimer *m_drawTimer;
};

#endif // MAINWINDOW_H
