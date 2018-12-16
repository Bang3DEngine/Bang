#ifndef RENDERFLAGS_H
#define RENDERFLAGS_H

#include "Bang/Flags.h"

namespace Bang
{
enum class RenderFlag : uint64_t
{
    NONE = 0,
    CLEAR_COLOR = (1 << 0),
    CLEAR_NORMALS = (1 << 1),
    CLEAR_ALBEDO = (1 << 2),
    CLEAR_LIGHT = (1 << 3),
    CLEAR_MISC = (1 << 4),
    CLEAR_DEPTH_STENCIL = (1 << 5),
    RENDER_SHADOW_MAPS = (1 << 6),
    RENDER_REFLECTION_PROBES = (1 << 7),
    DEFAULT = SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_COLOR) |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_NORMALS) |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_ALBEDO) |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_LIGHT) |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_MISC) |
              SCAST<FlagsPrimitiveType>(RenderFlag::CLEAR_DEPTH_STENCIL) |
              SCAST<FlagsPrimitiveType>(RenderFlag::RENDER_SHADOW_MAPS) |
              SCAST<FlagsPrimitiveType>(RenderFlag::RENDER_REFLECTION_PROBES)
};

CREATE_FLAGS(RenderFlags, RenderFlag);
}

#endif  // RENDERFLAGS_H
