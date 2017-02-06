#include "scenetemplate.h"

// TODO: Put these in JSON, allow them to be saved/loaded externally

SceneTemplate sceneTemplates[SCENE_TEMPLATE_COUNT] = {
    {
        "BlinkyTape",
        ":/scenetemplates/images/scenetemplates/blinkytape.jpg",
        ":/examples/blinkytape",
        "BlinkyTape",
        "Default",
        "Linear",
        RGB,
        QSize(1,60)
    },
    {
        "BlinkyTile",
        ":/scenetemplates/images/scenetemplates/blinkytile.jpg",
        ":/examples/blinkytile",
        "Light Buddy (BlinkyTile)",
        "Default",
        "Linear",
        RGB,
        QSize(1,56)
    },
    {
        "Blinky Pixels",
        ":/scenetemplates/images/scenetemplates/pixels.jpg",
        ":/examples/blinkytape",
        "BlinkyTape",
        "Default",
        "Linear",
        GRB,
        QSize(1,50)
    },
    {
        "BlinkyMatrix",
        ":/scenetemplates/images/scenetemplates/matrix.jpg",
        ":/examples/matrix",
        "BlinkyTape",
        "Default",
        "Matrix-Zigzag",
        RGB,
        QSize(8,8)
    },
    {
        "BlinkyPendant",
        ":/scenetemplates/images/scenetemplates/pendant.jpg",
        ":/examples/pendant",
        "BlinkyPendant",
        "Default",
        "Linear",
        RGB,
        QSize(1,10)
    },
    {
        "EightByEight",
        ":/scenetemplates/images/scenetemplates/eightbyeight.jpg",
        ":/examples/matrix",
        "EightByEight",
        "Default",
        "Matrix-Rows",
        RGB,
        QSize(8,8)
    },
};
