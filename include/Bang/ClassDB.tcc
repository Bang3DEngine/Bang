#pragma once

#include "Bang/ClassDB.h"
#include "Bang/Debug.h"

namespace Bang
{
constexpr inline ClassIdType ClassDB::GetInvalidClassId()
{
    return SCAST<ClassIdType>(-1);
}

constexpr inline bool ClassDB::IsSubClassByIds(ClassIdType baseClassIdBegin,
                                               ClassIdType baseClassIdEnd,
                                               ClassIdType subClassId)
{
    return (subClassId >= baseClassIdBegin && subClassId < baseClassIdEnd);
}

template <class TSubClass>
constexpr inline bool ClassDB::IsSubClass(ClassIdType baseClassIdBegin,
                                          ClassIdType baseClassIdEnd,
                                          const TSubClass *obj)
{
    return ClassDB::IsSubClassByIds(
        baseClassIdBegin, baseClassIdEnd, obj->GetInstanceClassId());
}

template <class TBaseClass, class TSubClass>
constexpr inline bool ClassDB::IsSubClass(const TSubClass *obj)
{
    return ClassDB::IsSubClass(
        TBaseClass::GetClassIdBegin(), TBaseClass::GetClassIdEnd(), obj);
}

template <class T>
T *ClassDB::Create(const String &className)
{
    void *createdObj = ClassDB::Create(className);
    if (!createdObj)
    {
        Debug_Error("Trying to create an object of class '"
                    << className
                    << "' "
                       "which is not registered.");
    }
    return createdObj ? SCAST<T *>(createdObj) : nullptr;
}
}
