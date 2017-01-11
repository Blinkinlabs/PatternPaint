#ifndef SCENECONFIGURATION_H
#define SCENECONFIGURATION_H

#include "scenetemplate.h"
#include "patternwriter.h"

#include <QDialog>

namespace Ui {
class SceneConfiguration;
}

/// Dialog to allow the user to specify a new width/height for the animation
class SceneConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit SceneConfiguration(QWidget *parent = 0);
    ~SceneConfiguration();

    void setSceneTemplate(SceneTemplate sceneTemplate);

    SceneTemplate getSceneTemplate();

private slots:
    void on_sceneTemplate_activated(const QString &arg1);

    void on_fixtureHeight_textEdited(const QString &arg1);

    void on_fixtureWidth_textEdited(const QString &arg1);

    void on_FixtureType_currentIndexChanged(int index);

    void on_ColorType_currentIndexChanged(int index);

    void on_controllerType_currentIndexChanged(int index);

private:
    Ui::SceneConfiguration *ui;

    void setControllerType(QString type);
    void setFixtureType(QString type);
    void setFixtureSize(QSize size);
    void setColorMode(ColorMode mode);

    void sceneCustomized();
};

#endif // SCENECONFIGURATION_H
