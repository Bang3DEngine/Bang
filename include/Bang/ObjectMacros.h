#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#include "Bang/ClassDB.h"

namespace Bang
{
#define OBJECT(CLASS) SET_CLASS_ID(CLASS) OBJECT_ABSTRACT(CLASS)
#define OBJECT_ABSTRACT(CLASS) friend class Bang::ClassDB;
}

#endif  // OBJECTMACROS_H
