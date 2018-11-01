#pragma once

#include "Bang/Map.h"

namespace Bang
{
template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Map()
{
}

template <class Key, class Value, class Compare>
Map<Key, Value, Compare>::Map(const std::map<Key, Value, Compare> &m) : m_map(m)
{
}

template <class Key, class Value, class Compare>
void Map<Key, Value, Compare>::Add(const Key &key, const Value &value)
{
    (*this)[key] = value;
}

template <class Key, class Value, class Compare>
template <typename... Args>
void Map<Key, Value, Compare>::Emplace(Args &&... args)
{
    m_map.emplace(args...);
}

template <class Key, class Value, class Compare>
void Map<Key, Value, Compare>::Remove(const Key &key)
{
    m_map.erase(key);
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::Remove(
    typename Map<Key, Value, Compare>::Iterator it)
{
    return m_map.erase(it);
}

template <class Key, class Value, class Compare>
void Map<Key, Value, Compare>::RemoveValues(const Value &value)
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

template <class Key, class Value, class Compare>
Value &Map<Key, Value, Compare>::Get(const Key &key)
{
    return (*this)[key];
}

template <class Key, class Value, class Compare>
const Value &Map<Key, Value, Compare>::Get(const Key &key) const
{
    return m_map.at(key);
}

template <class Key, class Value, class Compare>
void Map<Key, Value, Compare>::Clear()
{
    m_map.clear();
}

template <class Key, class Value, class Compare>
int Map<Key, Value, Compare>::Size() const
{
    return SCAST<int>(m_map.size());
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::IsEmpty() const
{
    return Size() == 0;
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::ContainsKey(const Key &key) const
{
    return Find(key) != CEnd();
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::ContainsValue(const Value &value) const
{
    for (auto it = Begin(); it != End(); ++it)
    {
        if (it->second == value)
            return true;
    }
    return false;
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::Find(
    const Key &key)
{
    return m_map.find(key);
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::Find(const Key &key) const
{
    return m_map.find(key);
}

template <class Key, class Value, class Compare>
Array<Key> Map<Key, Value, Compare>::GetKeysWithValue(const Value &v) const
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

template <class Key, class Value, class Compare>
Array<Key> Map<Key, Value, Compare>::GetKeys() const
{
    Array<Key> result;
    for (const auto &it : *this)
    {
        result.PushBack(it.first);
    }
    return result;
}

template <class Key, class Value, class Compare>
Array<Value> Map<Key, Value, Compare>::GetValues() const
{
    Array<Value> result;
    for (auto it = Begin(); it != End(); ++it)
    {
        result.PushBack(it->second);
    }
    return result;
}

template <class Key, class Value, class Compare>
const Value &Map<Key, Value, Compare>::operator[](const Key &k) const
{
    return m_map[k];
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::Begin()
{
    return m_map.begin();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::Begin() const
{
    return CBegin();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::CBegin() const
{
    return m_map.cbegin();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::End()
{
    return m_map.end();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::End() const
{
    return CEnd();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::CEnd() const
{
    return m_map.cend();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::RIterator Map<Key, Value, Compare>::RBegin()
{
    return m_map.rbegin();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::RIterator Map<Key, Value, Compare>::REnd()
{
    return m_map.rend();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_RIterator
Map<Key, Value, Compare>::CRBegin() const
{
    return m_map.crbegin();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_RIterator
Map<Key, Value, Compare>::CREnd() const
{
    return m_map.crend();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::begin()
{
    return m_map.begin();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Iterator Map<Key, Value, Compare>::end()
{
    return m_map.end();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::begin() const
{
    return this->CBegin();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::end() const
{
    return this->CEnd();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::cbegin() const
{
    return m_map.cbegin();
}

template <class Key, class Value, class Compare>
typename Map<Key, Value, Compare>::Const_Iterator
Map<Key, Value, Compare>::cend() const
{
    return m_map.cend();
}

template <class Key, class Value, class Compare>
Value &Map<Key, Value, Compare>::operator[](const Key &k)
{
    return m_map[k];
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::operator==(
    const Map<Key, Value, Compare> &rhs) const
{
    return m_map == rhs.m_map;
}

template <class Key, class Value, class Compare>
bool Map<Key, Value, Compare>::operator!=(
    const Map<Key, Value, Compare> &rhs) const
{
    return !(*this == rhs);
}
}
