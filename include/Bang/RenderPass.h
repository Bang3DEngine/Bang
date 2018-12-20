#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "Bang/Bang.h"

namespace Bang
{
enum class RenderPass
{
    SCENE_OPAQUE = 0,
    SCENE_DECALS = 8,
    SCENE_TRANSPARENT = 6,
    SCENE_BEFORE_ADDING_LIGHTS = 1,
    SCENE_AFTER_ADDING_LIGHTS = 7,

    CANVAS = 2,
    CANVAS_POSTPROCESS = 3,

    OVERLAY = 4,
    OVERLAY_POSTPROCESS = 5
};
}

#endif  // RENDERPASS_H
