#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#include "Bang/ClassDB.h"

namespace Bang
{
#define OBJECT_WITH_FAST_DYNAMIC_CAST_EXPLICIT(CLASS, CIDBegin, CIDEnd) \
    SET_CLASS_ID(CLASS, CIDBegin, CIDEnd)

#define OBJECT_WITH_FAST_DYNAMIC_CAST(CLASS) \
    OBJECT_WITH_FAST_DYNAMIC_CAST_EXPLICIT(  \
        CLASS, ClassDB::CLASS##CIDBegin, ClassDB::CLASS##CIDEnd)
}

#endif  // OBJECTMACROS_H
