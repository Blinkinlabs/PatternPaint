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
        60,
        1
    },
    {
        "BlinkyTile",
        ":/scenetemplates/images/scenetemplates/blinkytile.jpg",
        ":/examples/blinkytile",
        "Light Buddy (BlinkyTile)",
        "Default",
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
        "Default",
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
        "Default",
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
        "Default",
        "Linear",
        RGB,
        10,
        1
    },
};
