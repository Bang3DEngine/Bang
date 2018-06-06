#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

enum class RenderPass
{
    SCENE               = 0,
    SCENE_TRANSPARENT   = 6,
    SCENE_POSTPROCESS   = 1,

    CANVAS              = 2,
    CANVAS_POSTPROCESS  = 3,

    OVERLAY             = 4,
    OVERLAY_POSTPROCESS = 5
};

NAMESPACE_BANG_END

#endif // RENDERPASS_H
