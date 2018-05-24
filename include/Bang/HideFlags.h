#ifndef HIDEFLAGS_H
#define HIDEFLAGS_H

#include "Bang/Flags.h"

NAMESPACE_BANG_BEGIN

enum HideFlag
{
    DEFAULT,
    DONT_SERIALIZE,
    DONT_CLONE
};

CREATE_FLAGS(HideFlags, HideFlag);

NAMESPACE_BANG_END

#endif // HIDEFLAGS_H
