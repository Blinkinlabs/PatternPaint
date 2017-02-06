#ifndef SCENETEMPLATE_H
#define SCENETEMPLATE_H

#include "colormode.h"

#include <QString>

// TODO: Something more flexible
struct SceneTemplate {
    QString name;
    QString photo;
    QString examples;
    QString controllerType;
    QString firmwareName;
    QString fixtureType;
    ColorMode colorMode;
    int height;
    int width;
};

#define SCENE_TEMPLATE_COUNT 6
extern SceneTemplate sceneTemplates[SCENE_TEMPLATE_COUNT];

#endif // SCENETEMPLATE_H
