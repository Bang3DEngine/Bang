#ifndef FAST_DYNAMIC_CAST_H
#define FAST_DYNAMIC_CAST_H

#include "Bang/Assert.h"
#include "Bang/Bang.h"

#include <cmath>
#include <limits>

namespace Bang
{
using ClassIdType = uint;
using CID = ClassIdType;

constexpr inline CID GetInvalidClassId()
{
    return SCAST<CID>(-1);
}

// Functions to know whether a FastDynamicCastable can be cast by looking at
// its id
constexpr inline bool CanFastDynamicCast(ClassIdType objClassId)
{
    return (objClassId != GetInvalidClassId());
}
template <class T>
constexpr inline bool CanFastDynamicCast(T *obj)
{
    return CanFastDynamicCast(obj->GetClassId()) && T::CanFastDynamicCast();
}

// SFINAE helper to know whether T is FastDynamicCastable
template <class T, class = void>
struct IsFastDynamicCastable
{
    static constexpr bool value = false;
};
template <class T>
struct IsFastDynamicCastable<T, decltype(T::GetClassIdBegin(), void())>
{
    static constexpr bool value = true;
};

// FastDynamicCast function if the two classes can FastDynamicCast. Otherwise
// fallback to the usual dynamic_cast
template <class TCastTo, class TCastFrom>
inline TCastTo FastDynamicCast(
    TCastFrom obj,
    typename std::enable_if<
        IsFastDynamicCastable<
            typename std::remove_pointer<TCastTo>::type>::value &&
        IsFastDynamicCastable<
            typename std::remove_pointer<TCastFrom>::type>::value>::type * = 0)
{
    ClassIdType objClassId = obj->GetClassId();
    if (CanFastDynamicCast(objClassId))
    {
        // Is the object class id in the range of the class ids of the class
        // we want to cast to?
        if (objClassId >=
                std::remove_pointer<TCastTo>::type::GetClassIdBegin() &&
            objClassId < std::remove_pointer<TCastTo>::type::GetClassIdEnd())
        {
            return RCAST<TCastTo>(obj);
        }
    }
    else
    {
        return DCAST<TCastTo>(obj);
    }
    return nullptr;
}
template <class TCastTo, class TCastFrom>
inline TCastTo FastDynamicCast(
    TCastFrom obj,
    typename std::enable_if<
        !IsFastDynamicCastable<
            typename std::remove_pointer<TCastTo>::type>::value ||
        !IsFastDynamicCastable<
            typename std::remove_pointer<TCastFrom>::type>::value>::type * = 0)
{
    return DCAST<TCastTo>(obj);
}

// This macro sets a class ID by adding the needed members to the class
#define SET_CLASS_ID(CLASS, ID_BEGIN, ID_END)     \
public:                                           \
    constexpr static inline CID GetClassIdBegin() \
    {                                             \
        return (ID_BEGIN);                        \
    }                                             \
    constexpr static inline CID GetClassIdEnd()   \
    {                                             \
        return (ID_END);                          \
    }                                             \
    constexpr static bool CanFastDynamicCast()    \
    {                                             \
        return true;                              \
    }                                             \
                                                  \
private:

#define SET_CLASS_ID_AS_ROOT()                    \
protected:                                        \
    ClassIdType m_classId = GetInvalidClassId();  \
                                                  \
public:                                           \
    ClassIdType GetClassId()                      \
    {                                             \
        return m_classId;                         \
    }                                             \
    constexpr static inline CID GetClassIdBegin() \
    {                                             \
        return GetInvalidClassId();               \
    }                                             \
    constexpr static inline CID GetClassIdEnd()   \
    {                                             \
        return GetInvalidClassId();               \
    }                                             \
                                                  \
public:                                           \
    constexpr static bool CanFastDynamicCast()    \
    {                                             \
        return false;                             \
    }                                             \
                                                  \
private:

#define CONSTRUCT_CLASS_ID(CLASS) m_classId = CLASS::GetClassIdBegin();
}

#endif  // FAST_DYNAMIC_CAST_H
