#ifndef RENDERFLAGS_H
#define RENDERFLAGS_H

#include "Bang/Flags.h"

NAMESPACE_BANG_BEGIN

enum class RenderFlag
{
    NONE = 0,
    CLEAR_COLOR              = 0b00000000001,
    CLEAR_NORMALS            = 0b00000000010,
    CLEAR_ALBEDO             = 0b00000000100,
    CLEAR_MISC               = 0b00000001000,
    CLEAR_DEPTH_STENCIL      = 0b00000010000,
    RENDER_SHADOW_MAPS       = 0b00000100000,
    RENDER_REFLECTION_PROBES = 0b00001000000,
    DEFAULT = SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_COLOR)         |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_NORMALS)       |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_ALBEDO)        |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_MISC)          |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_DEPTH_STENCIL) |
              SCAST<FlagsPrimitiveType>(RenderFlag::RENDER_SHADOW_MAPS)  |
              SCAST<FlagsPrimitiveType>(RenderFlag::RENDER_REFLECTION_PROBES)
};

CREATE_FLAGS(RenderFlags, RenderFlag);

NAMESPACE_BANG_END

#endif // RENDERFLAGS_H
