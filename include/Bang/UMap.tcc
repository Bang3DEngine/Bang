#pragma once

#include "Bang/UMap.h"

namespace Bang
{
template <class Key, class Value, class Hash, class Pred, class Alloc>
UMap<Key, Value, Hash, Pred, Alloc>::UMap()
{
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
UMap<Key, Value, Hash, Pred, Alloc>::UMap(
    const std::unordered_map<Key, Value, Hash, Pred, Alloc> &m)
    : m_umap(m)
{
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
void UMap<Key, Value, Hash, Pred, Alloc>::Add(const Key &key,
                                              const Value &value)
{
    (*this)[key] = value;
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
void UMap<Key, Value, Hash, Pred, Alloc>::Remove(const Key &key)
{
    m_umap.erase(key);
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Iterator
UMap<Key, Value, Hash, Pred, Alloc>::Remove(
    typename UMap<Key, Value, Hash, Pred, Alloc>::Iterator it)
{
    return m_umap.erase(it);
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
void UMap<Key, Value, Hash, Pred, Alloc>::RemoveValues(const Value &value)
{
    for (auto it = Begin(); it != End();)
    {
        if (it->second == value)
        {
            it = Remove(it);
        }
        else
        {
            ++it;
        }
    }
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
Value &UMap<Key, Value, Hash, Pred, Alloc>::Get(const Key &key)
{
    return (*this)[key];
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
const Value &UMap<Key, Value, Hash, Pred, Alloc>::Get(const Key &key) const
{
    return m_umap.at(key);
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
void UMap<Key, Value, Hash, Pred, Alloc>::Clear()
{
    m_umap.clear();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
int UMap<Key, Value, Hash, Pred, Alloc>::Size() const
{
    return SCAST<int>(m_umap.size());
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
bool UMap<Key, Value, Hash, Pred, Alloc>::IsEmpty() const
{
    return Size() == 0;
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
bool UMap<Key, Value, Hash, Pred, Alloc>::ContainsKey(const Key &key) const
{
    return Find(key) != CEnd();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
bool UMap<Key, Value, Hash, Pred, Alloc>::ContainsValue(
    const Value &value) const
{
    for (auto it = Begin(); it != End(); ++it)
    {
        if (it->second == value)
        {
            return true;
        }
    }
    return false;
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Iterator
UMap<Key, Value, Hash, Pred, Alloc>::Find(const Key &key)
{
    return m_umap.find(key);
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::Find(const Key &key) const
{
    return m_umap.find(key);
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
Array<Key> UMap<Key, Value, Hash, Pred, Alloc>::GetKeysWithValue(
    const Value &v) const
{
    Array<Key> result;
    for (auto it = Begin(); it != End(); ++it)
    {
        if (it->second == v)
        {
            result.PushBack(it->first);
        }
    }
    return result;
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
Array<Key> UMap<Key, Value, Hash, Pred, Alloc>::GetKeys() const
{
    Array<Key> result;
    for (const auto &it : *this)
    {
        result.PushBack(it.first);
    }
    return result;
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
Array<Value> UMap<Key, Value, Hash, Pred, Alloc>::GetValues() const
{
    Array<Value> result;
    for (auto it = Begin(); it != End(); ++it)
    {
        result.PushBack(it->second);
    }
    return result;
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
const Value &UMap<Key, Value, Hash, Pred, Alloc>::operator[](const Key &k) const
{
    return m_umap[k];
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Iterator
UMap<Key, Value, Hash, Pred, Alloc>::Begin()
{
    return m_umap.begin();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::Begin() const
{
    return CBegin();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::CBegin() const
{
    return m_umap.cbegin();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Iterator
UMap<Key, Value, Hash, Pred, Alloc>::End()
{
    return m_umap.end();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::End() const
{
    return CEnd();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::CEnd() const
{
    return m_umap.cend();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Iterator
UMap<Key, Value, Hash, Pred, Alloc>::begin()
{
    return m_umap.begin();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Iterator
UMap<Key, Value, Hash, Pred, Alloc>::end()
{
    return m_umap.end();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::begin() const
{
    return this->CBegin();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::end() const
{
    return this->CEnd();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::cbegin() const
{
    return m_umap.cbegin();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
typename UMap<Key, Value, Hash, Pred, Alloc>::Const_Iterator
UMap<Key, Value, Hash, Pred, Alloc>::cend() const
{
    return m_umap.cend();
}

template <class Key, class Value, class Hash, class Pred, class Alloc>
Value &UMap<Key, Value, Hash, Pred, Alloc>::operator[](const Key &k)
{
    return m_umap[k];
}
}  // namespace Bang
