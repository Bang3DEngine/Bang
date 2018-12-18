#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "Bang/Bang.h"

namespace Bang
{
enum class RenderPass
{
    SCENE = 0,
    SCENE_TRANSPARENT = 6,
    SCENE_POSTPROCESS = 1,
    SCENE_POSTPROCESS_2 = 7,

    CANVAS = 2,
    CANVAS_POSTPROCESS = 3,

    OVERLAY = 4,
    OVERLAY_POSTPROCESS = 5
};
}

#endif  // RENDERPASS_H
