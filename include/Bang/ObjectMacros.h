#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#include "Bang/ClassDB.h"
#include "Bang/Serializable.h"

namespace Bang
{
#define OBJECT(CLASS)   \
    SET_CLASS_ID(CLASS) \
    SERIALIZABLE(CLASS) \
    friend class Bang::ClassDB;

#define OBJECT_WITHOUT_CLASS_ID(CLASS) \
    SERIALIZABLE(CLASS)                \
    friend class Bang::ClassDB;

#define OBJECT_ABSTRACT_WITHOUT_CLASS_ID(CLASS) \
    SERIALIZABLE_ABSTRACT(CLASS)                \
    friend class Bang::ClassDB;

#define OBJECT_ABSTRACT(CLASS)   \
    SERIALIZABLE_ABSTRACT(CLASS) \
    friend class Bang::ClassDB;
}

#endif  // OBJECTMACROS_H
