#ifndef HIDEFLAGS_H
#define HIDEFLAGS_H

#include "Bang/Flags.h"

namespace Bang
{
enum class HideFlag
{
    NONE = 0,
    DONT_SERIALIZE = 1,
    DONT_CLONE = 2,
    DEFAULT = HideFlag::NONE
};

CREATE_FLAGS(HideFlags, HideFlag);
}

#endif  // HIDEFLAGS_H
