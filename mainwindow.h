#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>

#include "blinkytape.h"
#include "animationuploader.h"
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

private slots:
    void on_tapeConnectDisconnect_clicked();

    void drawTimer_timeout();

    void on_actionOpen_Animation_triggered();

    void on_actionSave_Animation_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionSystem_Information_triggered();

    void on_animationSpeed_valueChanged(int value);

    void on_animationPlayPause_clicked();

    void on_uploadButton_clicked();

    void on_tapeConnectionStatusChanged(bool connected);

    void on_uploaderProgressChanged(int progress);

    void on_uploaderFinished(bool result);

private:
    Ui::MainWindow *ui;

    QTimer *drawTimer;
    QProgressDialog* progress;
    BlinkyTape* tape;
    AnimationUploader* uploader;
};

#endif // MAINWINDOW_H
