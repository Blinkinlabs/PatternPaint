#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "blinkycontroller.h"
#include "blinkyuploader.h"
#include "frameeditor.h"
#include "instrumentconfiguration.h"
#include "colorchooser.h"
#include "patterncollection.h"
#include "welcomescreen.h"
#include "fixture.h"
#include "autoupdater.h"
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QProgressDialog>
#include <QMessageBox>
#include <QUndoGroup>
#include <QToolButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QTimer>


#if defined(Q_OS_MACX)
#include "CocoaInitializer.h"
#include "appnap.h"
#include "SparkleAutoUpdater.h"
#elif defined(Q_OS_WIN)
#include "winsparkleautoupdater.h"
#endif


class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_frameDataEdited(int index, QImage update);

    void applyScene(const SceneTemplate &scene);

protected:
    void closeEvent(QCloseEvent *event);

    void showEvent(QShowEvent *event);

private slots:
    void drawTimer_timeout();

    void connectionScannerTimer_timeout();

    void on_actionSave_triggered();
    void on_actionSave_as_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionSystem_Information_triggered();

    void patternSpeed_valueChanged(int value);

    void frameIndex_valueChanged(QString value);

    void on_actionPlay_triggered();

    void on_blinkyConnectionStatusChanged(bool connected);

    void on_uploaderFinished(bool result);

    void on_actionExport_pattern_for_Arduino_triggered();

    void on_actionVisit_the_Blinkinlabs_forum_triggered();

    void on_actionTroubleshooting_tips_triggered();

    void on_actionFlip_Horizontal_triggered();

    void on_actionFlip_Vertical_triggered();

    void on_actionFirmware_Manager_triggered();

    void on_actionRestore_firmware_triggered();

    void on_actionSave_to_Blinky_triggered();

    void on_actionClear_Pattern_triggered();

    void on_instrumentSelected(bool);

    void on_colorPicked(QColor);

    void on_actionClose_triggered();

    void on_actionStepForward_triggered();

    void on_actionStepBackward_triggered();

    void on_actionAddFrame_triggered();

    void on_actionDeleteFrame_triggered();

    void on_ExampleSelected(QAction *);

    void on_patternCollectionCurrentChanged(const QModelIndex &current, const QModelIndex &);

    void on_timelineSelectedChanged(const QModelIndex &current, const QModelIndex &);

    void on_PatternDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                               const QVector<int> &roles);

    void on_actionNew_ScrollingPattern_triggered();

    void on_actionNew_FramePattern_triggered();

    void on_actionConfigure_Scene_triggered();

    void on_actionOpen_Scrolling_Pattern_triggered();

    void on_actionOpen_Frame_based_Pattern_triggered();

    void on_actionPreferences_triggered();

    void on_actionWelcome_triggered();

    void on_actionSave_All_triggered();

    void on_actionClose_All_triggered();

    void on_windowLoaded();

    void on_actionDebug_Log_triggered();

signals:

    /// Signalled when an editable pattern is selected in the editor
    /// @param status true if pattern available, false otherwise.
    void patternStatusChanged(bool status);

    void windowLoaded();

private:
    AutoUpdater *autoUpdater;

    ColorChooser colorChooser;

    QTimer drawTimer;
    QTimer connectionScannerTimer;

    QPointer<BlinkyController> controller;
    QPointer<BlinkyUploader> uploader;
    QPointer<Fixture> fixture;

    PatternCollection patternCollection;

    QLineEdit currentFrame;
    InstrumentConfiguration instrumentConfiguration;

    enum Modes {
        Disconnected, Connected, Uploading
    };
    Modes mode;

    QUndoGroup undoGroup;

    // TODO: Manage these in a seperate class?
    QMetaObject::Connection timelineSelectedChangedConnection;
    QMetaObject::Connection patternDataChangedConnection;

    void populateExamplesMenu(QString directory, QMenu *menu);

    void showError(QString errorMessage);

    void startPlayback();
    void stopPlayback();

    bool promptForSave(Pattern *pattern);
    bool promptForSave(QList<Pattern *> patterns);

    bool savePattern(Pattern *item);
    bool savePatternAs(Pattern *item);

    QProgressDialog* makeProgressDialog();

    bool loadPattern(Pattern::PatternType type, const QString fileName);

    void setPatternName(QString name);

    void editImageChanged(int index, const QImage &data);
    void frameImageChanged(const QImage &data);

    void setPatternModified(bool modified);

    void setNewFrame(int newFrame);

    void updateBlinky(const QImage &frame);

#if defined(Q_OS_MAC)
    /// Object to inhibit app nap when connected to a blinky
    CAppNapInhibitor *appNap;
#endif

    // TODO: These don't belong here.
    int getCurrentFrameIndex();
    int getFrameCount();
    int getCurrentPatternIndex();
    int getPatternCount();
    void openPattern(Pattern::PatternType type);
};

#endif // MAINWINDOW_H
