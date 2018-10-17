#ifndef NEEDEDUNIFORMFLAGS_H
#define NEEDEDUNIFORMFLAGS_H

#include "Bang/Bang.h"
#include "Bang/Flags.h"

namespace Bang
{
enum class NeededUniformFlag : uint32_t
{
    NONE = (0),
    MODEL = (1 << 0),
    MODEL_INV = (1 << 1),
    NORMAL = (1 << 2),
    PVM = (1 << 3),
    PVM_INV = (1 << 4),
    SKYBOXES = (1 << 5),
    MATERIAL_ALBEDO = (1 << 6),
    MATERIAL_PBR = (1 << 7),
    TIME = (1 << 8),
    ALL = MODEL | MODEL_INV | NORMAL | PVM | PVM_INV | SKYBOXES |
          MATERIAL_ALBEDO |
          MATERIAL_PBR |
          TIME,
    DEFAULT = ALL
};
CREATE_FLAGS(NeededUniformFlags, NeededUniformFlag);
}

#endif  // NEEDEDUNIFORMFLAGS_H
