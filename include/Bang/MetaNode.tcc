#pragma once

#include "Bang/MetaNode.h"

namespace Bang
{
template <class T>
void MetaNode::Set(const String &attributeName, const T &value)
{
    MetaAttribute attr;
    attr.Set<T>(attributeName, value);
    Set(attr);
}

template <class T>
void MetaNode::SetArray(const String &name, const Array<T> &array)
{
    for (uint i = 0; i < array.Size(); ++i)
    {
        const T &x = array[i];
        Set(name + "_" + String::ToString(i), x);
    }
}

template <class T>
T MetaNode::Get(const String &attributeName, const T &defaultValue) const
{
    MetaAttribute *attr = GetAttribute(attributeName);
    return attr ? attr->Get<T>() : defaultValue;
}

template <class T>
Array<T> MetaNode::GetArray(const String &attributeName) const
{
    int i = 0;
    Array<T> result;
    while (true)
    {
        const String attrNamei = attributeName + "_" + String::ToString(i);
        if (!Contains(attrNamei))
        {
            break;
        }
        result.PushBack(Get<T>(attrNamei));
        ++i;
    }
    return result;
}

template <class T>
T MetaNode::Get(const String &attributeName) const
{
    return Get<T>(attributeName, T());
}
}  // namespace Bang
