#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QMessageBox>
#include <QUndoGroup>
#include <QToolButton>
#include <QSpinBox>
#include <QLineEdit>

#include "blinkycontroller.h"
#include "patternuploader.h"
#include "frameeditor.h"
#include "addressprogrammer.h"
#include "colorchooser.h"
#include "patterncollection.h"

#include "ui_mainwindow.h"

#if defined Q_OS_MAC
#include "appnap.h"
#endif

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void on_patternSizeUpdated();

    void on_frameDataEdited(int index, QImage update);

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

    void on_actionNew_triggered();

    void on_actionClose_triggered();

    void on_actionStepForward_triggered();

    void on_actionStepBackward_triggered();

    void on_actionTimeline_triggered();

    void on_actionMatrix_triggered();

    void on_actionAddFrame_triggered();

    void on_actionDeleteFrame_triggered();

    void on_ExampleSelected(QAction*);

    void on_patternCollectionCurrentChanged(const QModelIndex &current, const QModelIndex &);

    void on_timelineSelectedChanged(const QModelIndex &current, const QModelIndex &);

    void on_timelineDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

    void on_actionNew_ScrollingPattern_triggered();

    void on_actionNew_FramePattern_triggered();

signals:

    /// Signalled when an editable pattern is selected in the editor
    /// @param status true if pattern available, false otherwise.
    void patternStatusChanged(bool status);

private:
    ColorChooser colorChooser;

    QTimer drawTimer;
    QTimer connectionScannerTimer;

    QPointer<BlinkyController> controller;
    QPointer<PatternUploader> uploader;

    QProgressDialog progressDialog;

    PatternCollection patternCollection;

    QSpinBox pSpeed;
    QLineEdit pFrame;

    enum Modes { Disconnected, Connected, Uploading };
    Modes mode;

    QUndoGroup undoGroup;

    PatternWriter::ColorMode colorMode;

    void populateExamplesMenu(QString directory, QMenu* menu);

    void showError(QString errorMessage);

    void startPlayback();
    void stopPlayback();

    bool promptForSave(Pattern *item);
    bool promptForSave(std::vector<Pattern*> item);

    bool savePattern(Pattern *item);
    bool savePatternAs(Pattern *item);

    void connectController();
    void connectUploader();

    bool loadPattern(Pattern::PatternType type, const QString fileName);

    void setPatternName(QString name);

    void setPatternData(int index, QImage data);

    void setPatternModified(bool modified);

    void setNewFrame(int newFrame);

    void updateBlinky();

#if defined(Q_OS_MAC)
    /// Object to inhibit app nap when connected to a blinky
    CAppNapInhibitor* appNap;
#endif

    // TODO: These don't belong here.
    int getCurrentFrameIndex();
    int getFrameCount();
    int getCurrentPatternIndex();
    int getPatternCount();
};

#endif // MAINWINDOW_H
