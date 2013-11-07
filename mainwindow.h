#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "blinkytape.h"
//#include "blinkytapethreaded.h"
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
//    BlinkyTapeThreaded tape;

private slots:
    void on_tapeConnectDisconnect_clicked();

    void drawTimerTimeout();

    void on_actionOpen_Animation_triggered();

    void on_actionSave_Animation_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_animationSpeed_valueChanged(int value);

    void on_animationPlayPause_clicked();



    void on_uploadButton_clicked();

    void on_tapeConnectionStatusChanged(bool status);

private:
    Ui::MainWindow *ui;

    QTimer *m_drawTimer;
};

#endif // MAINWINDOW_H
