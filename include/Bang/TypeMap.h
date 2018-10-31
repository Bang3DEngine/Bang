#ifndef TYPEMAP_H
#define TYPEMAP_H

#include <iostream>
#include <type_traits>
#include <typeinfo>

#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/String.h"
#include "Bang/UMap.h"

namespace Bang
{
using TypeId = std::size_t;
template <class T>
static TypeId GetTypeId()
{
    return typeid(T).hash_code();
}

template <class T>
static TypeId GetTypeId(
    const T &x,
    typename std::enable_if<!std::is_pointer<T>::value, T>::type = 0)
{
    return typeid(x).hash_code();
}

template <class T>
static TypeId GetTypeId(
    const T &x,
    typename std::enable_if<std::is_pointer<T>::value, T>::type = 0)
{
    return x ? typeid(*x).hash_code() : -1;
}

template <class Value>
class TypeMap : public UMap<TypeId, Value>
{
public:
    template <class Class>
    void Add(const Value &value = Value())
    {
        UMap<TypeId, Value>::Add(GetTypeId<Class>(), value);
    }
    void Add(const TypeId &className, const Value &value = Value())
    {
        UMap<TypeId, Value>::Add(className, value);
    }

    template <class Class>
    void Remove()
    {
        UMap<TypeId, Value>::Remove(GetTypeId<Class>());
    }

    template <class Class>
    Value &Get()
    {
        return UMap<TypeId, Value>::Get(GetTypeId<Class>());
    }
    Value &Get(const TypeId &className)
    {
        return UMap<TypeId, Value>::Get(className);
    }

    template <class Class>
    const Value &Get() const
    {
        return UMap<TypeId, Value>::Get(GetTypeId<Class>());
    }
    const Value &Get(const TypeId &className) const
    {
        return UMap<TypeId, Value>::Get(className);
    }

    template <class Class>
    List<Value> GetAllDerived()
    {
        List<Value> values;
        for (const auto &it : *this)
        {
            if (Class *v = DCAST<Class *>(it.second))
            {
                values.PushBack(v);
            }
        }
        return values;
    }

    template <class Class>
    bool ContainsKey() const
    {
        return ContainsKey(GetTypeId<Class>());
    }
    bool ContainsKey(const TypeId &typeId) const
    {
        return UMap<TypeId, Value>::ContainsKey(typeId);
    }
};
}  // namespace Bang

#endif  // TYPEMAP_H
