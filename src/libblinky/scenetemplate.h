#ifndef SCENETEMPLATE_H
#define SCENETEMPLATE_H

#include "colormode.h"

#include <QString>
#include <QSize>

// TODO: Something more flexible
struct SceneTemplate {
    QString name;
    QString photo;
    QString examples;
    QString controllerType;
    QString firmwareName;
    QString fixtureType;
    ColorMode colorMode;
    QSize size;
};

#define SCENE_TEMPLATE_COUNT 6
extern const SceneTemplate sceneTemplates[SCENE_TEMPLATE_COUNT];

#endif // SCENETEMPLATE_H
