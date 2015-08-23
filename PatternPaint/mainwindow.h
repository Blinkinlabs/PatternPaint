#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QMessageBox>

#include "blinkycontroller.h"
#include "patternuploader.h"
#include "patterneditor.h"
#include "addressprogrammer.h"

#include "ui_mainwindow.h"

#include "leddisplay.h"

#if defined Q_OS_MAC
#include "appnap.h"
#endif

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

    void setDisplayMode(LedDisplay::Mode newDisplayMode);
public slots:
    void setPatternItem(QListWidgetItem *, QListWidgetItem *);

    void on_patternDataUpdated();
    void on_patternSizeUpdated();

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

    void patternSpeed_valueChanged(int value);

    void frameIndex_valueChanged(QString value);

    void on_actionPlay_triggered();

    void on_blinkyConnectionStatusChanged(bool connected);

    void on_uploaderMaxProgressChanged(int progressDialog);

    void on_uploaderProgressChanged(int progressDialog);

    void on_uploaderFinished(bool result);

    void on_actionExport_pattern_for_Arduino_triggered();

    void on_actionVisit_the_Blinkinlabs_forum_triggered();

    void on_actionTroubleshooting_tips_triggered();

    void on_actionFlip_Horizontal_triggered();

    void on_actionFlip_Vertical_triggered();

    void on_actionLoad_rainbow_sketch_triggered();

    void on_actionSave_to_Blinky_triggered();

    void on_actionClear_Pattern_triggered();

    void on_actionResize_Pattern_triggered();

    void on_actionAddress_programmer_triggered();

    void on_actionConnect_triggered();

    void on_instrumentSelected(bool);

    void on_colorPicked(QColor);

    void on_patternNameUpdated();

    void on_actionGRB_triggered();

    void on_actionRGB_triggered();

    void on_actionNew_triggered();

    void on_actionClose_triggered();

    void on_actionStepForward_triggered();

    void on_actionStepBackward_triggered();

    void on_actionTimeline_triggered();

    void on_actionMatrix_triggered();

    void on_actionAddFrame_triggered();

    void on_actionDeleteFrame_triggered();

    void on_patternModifiedChanged();

private:
    ColorChooser* colorChooser;

    QTimer* drawTimer;

    QPointer<BlinkyController> controller;
    QPointer<PatternUploader> uploader;

    QTimer *connectionScannerTimer;

    QProgressDialog* progressDialog;

    PatternUpdateNotifier patternUpdateNotifier;

    QSpinBox* pSpeed;
    QLineEdit* pFrame;

    enum Modes { Disconnected, Connected, Uploading };
    Modes mode;

    QUndoGroup *undoGroup;
    QAction* undoAction;
    QAction* redoAction;

    void showError(QString errorMessage);

    void writeSettings();
    void readSettings();

    void startPlayback();
    void stopPlayback();

    bool promptForSave(PatternItem* item);
    bool promptForSave(std::vector<PatternItem*> item);

    bool savePattern(PatternItem *item);
    bool savePatternAs(PatternItem *item);

    void connectController();
    void connectUploader();

    void setColorMode(Pattern::ColorMode newColorOrder);

    Pattern::ColorMode colorMode;

    LedDisplay* ledDisplay;

    int frame;      // Current frame we are drawing
    void setNewFrame(int newFrame);

    void updateBlinky();

#if defined(Q_OS_MAC)
    /// Object to inhibit app nap when connected to a blinky
    CAppNapInhibitor* appNap;
#endif
};

#endif // MAINWINDOW_H
