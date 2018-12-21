#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#include "Bang/ClassDB.h"

namespace Bang
{
#define OBJECT(CLASS) SET_CLASS_ID(CLASS) OBJECT_WITHOUT_CLASS_ID(CLASS)
#define OBJECT_WITHOUT_CLASS_ID(CLASS) friend class Bang::ClassDB;
}

#endif  // OBJECTMACROS_H
