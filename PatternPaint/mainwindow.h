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

#include "ui_mainwindow.h"

class QUndoGroup;
class ColorChooser;
class QToolButton;
class QSpinBox;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void drawTimer_timeout();

    void connectionScannerTimer_timeout();

    void on_actionLoad_File_triggered();

    void on_actionSave_File_triggered();
    void on_actionSave_File_as_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionSystem_Information_triggered();

    void on_patternSpeed_valueChanged(int value);

    void on_actionPlay_triggered();

    void on_tapeConnectionStatusChanged(bool connected);

    void on_uploaderMaxProgressChanged(int progressDialog);

    void on_uploaderProgressChanged(int progressDialog);

    void on_uploaderFinished(bool result);

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

    void on_actionConnect_triggered();

    void on_instrumentAction(bool);

    void on_colorPicked(QColor);

    void on_patternChanged(bool);

    void on_patternResized();

    void on_imageChanged(bool changed);

    void on_patternFilenameChanged(QFileInfo fileinfo);

private:
    ColorChooser* m_colorChooser;

    QTimer* drawTimer;

    QPointer<BlinkyTape> tape;
    QPointer<PatternUploader> uploader;

    QTimer *connectionScannerTimer;

    QProgressDialog* progressDialog;
    QMessageBox* errorMessageDialog;

    QSpinBox* pSpeed;

    enum Modes { Disconnected, Connected, Uploading };
    Modes mode;

    QUndoGroup *m_undoStackGroup;
    QAction* m_undoAction;
    QAction* m_redoAction;
    QFileInfo  m_lastFileInfo;

    void writeSettings();
    void readSettings();
    bool saveFile(const QFileInfo fileinfo);

    int promptForSave();
};

#endif // MAINWINDOW_H
