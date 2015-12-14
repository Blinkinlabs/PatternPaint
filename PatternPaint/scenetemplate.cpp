#include "scenetemplate.h"

// TODO: Put these in JSON, allow them to be saved/loaded externally

SceneTemplate sceneTemplates[SCENE_TEMPLATE_COUNT] = {
    {
        "BlinkyTape",
        ":/scenetemplates/images/scenetemplates/blinkytape.jpg",
        ":/examples/blinkytape",
        "BlinkyTape",
        "Linear",
        RGB,
        60,
        1
    },
    {
        "BlinkyTile",
        ":/scenetemplates/images/scenetemplates/blinkytile.jpg",
        ":/examples/blinkytile",
        "Light Buddy (BlinkyTile)",
        "Linear",
        RGB,
        56,
        1
    },
    {
        "Blinky Pixels",
        ":/scenetemplates/images/scenetemplates/pixels.jpg",
        ":/examples/blinkytape",
        "BlinkyTape",
        "Linear",
        GRB,
        50,
        1
    },
    {
        "BlinkyMatrix",
        ":/scenetemplates/images/scenetemplates/matrix.jpg",
        ":/examples/matrix",
        "BlinkyTape",
        "Matrix",
        RGB,
        8,
        8
    },
    {
        "BlinkyPendant",
        ":/scenetemplates/images/scenetemplates/pendant.jpg",
        ":/examples/pendant",
        "BlinkyPendant",
        "Linear",
        RGB,
        10,
        1
    },
};
