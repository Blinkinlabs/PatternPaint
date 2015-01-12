#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QMessageBox>

#include "blinkytape.h"
#include "patternuploader.h"
#include "avrpatternuploader.h"
#include "patterneditor.h"
#include "addressprogrammer.h"

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

    void connectionScannerTimer_timeout();

    void on_actionLoad_File_triggered();

    void on_actionSave_File_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionSystem_Information_triggered();

    void on_patternSpeed_valueChanged(int value);

    void on_patternPlayPause_clicked();

    void on_tapeConnectionStatusChanged(bool connected);

    void on_uploaderMaxProgressChanged(int progressDialog);

    void on_uploaderProgressChanged(int progressDialog);

    void on_uploaderFinished(bool result);

    void on_saveToTape_clicked();

    void on_saveFile_clicked();

    void on_loadFile_clicked();

    void on_actionExport_pattern_for_Arduino_triggered();

    void on_actionVisit_the_BlinkyTape_forum_triggered();

    void on_actionTroubleshooting_tips_triggered();

    void on_actionFlip_Horizontal_triggered();

    void on_actionFlip_Vertical_triggered();

    void on_actionLoad_rainbow_sketch_triggered();

    void on_actionSave_to_Tape_triggered();

    void on_actionClear_Pattern_triggered();

    void on_actionResize_Pattern_triggered();

    void on_actionAddress_programmer_triggered();

private:
    Ui::MainWindow *ui;

    QTimer* drawTimer;

    QPointer<BlinkyTape> tape;
    QPointer<PatternUploader> uploader;

    QTimer *connectionScannerTimer;

    QProgressDialog* progressDialog;
    QMessageBox* errorMessageDialog;
};

#endif // MAINWINDOW_H
