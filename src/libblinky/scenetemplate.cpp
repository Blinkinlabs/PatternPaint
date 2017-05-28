#include "scenetemplate.h"

// TODO: Put these in JSON, allow them to be saved/loaded externally

const SceneTemplate sceneTemplates[SCENE_TEMPLATE_COUNT] = {
    {
        "BlinkyTape",
        ":/scenetemplates/images/scenetemplates/blinkytape.jpg",
        ":/examples/blinkytape",
        "BlinkyTape",
        "default",
        "Linear",
        RGB,
        QSize(1,60)
    },
    {
        "BlinkyTile",
        ":/scenetemplates/images/scenetemplates/blinkytile.jpg",
        ":/examples/blinkytile",
        "Light Buddy (BlinkyTile)",
        "default",
        "Linear",
        RGB,
        QSize(1,56)
    },
    {
        "Blinky Pixels",
        ":/scenetemplates/images/scenetemplates/pixels.jpg",
        ":/examples/blinkytape",
        "BlinkyTape",
        "default",
        "Linear",
        GRB,
        QSize(1,50)
    },
    {
        "BlinkyMatrix",
        ":/scenetemplates/images/scenetemplates/matrix.jpg",
        ":/examples/matrix",
        "BlinkyTape",
        "default",
        "Matrix-Zigzag",
        RGB,
        QSize(8,8)
    },
    {
        "BlinkyPendant",
        ":/scenetemplates/images/scenetemplates/pendant.jpg",
        ":/examples/pendant",
        "BlinkyPendant",
        "default",
        "Linear",
        RGB,
        QSize(1,10)
    },
    {
        "EightByEight",
        ":/scenetemplates/images/scenetemplates/eightbyeight.jpg",
        ":/examples/matrix",
        "EightByEight",
        "default",
        "Matrix-Cols",
        RGB,
        QSize(8,8)
    },
};
