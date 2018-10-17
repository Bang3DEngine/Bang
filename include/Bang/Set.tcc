#pragma once

#include "Bang/Set.h"

namespace Bang
{
template <class Key, class Compare>
Set<Key, Compare>::Set()
{
}

template <class Key, class Compare>
Set<Key, Compare>::Set(const std::set<Key, Compare> &s) : m_set(s)
{
}

template <class Key, class Compare>
template <class OtherIterator>
Set<Key, Compare>::Set(OtherIterator itBegin, OtherIterator itEnd)
{
    Set(itBegin, itEnd);
}

template <class Key, class Compare>
void Set<Key, Compare>::Add(const Key &key)
{
    m_set.insert(key);
}

template <class Key, class Compare>
template <class OtherIterator>
void Set<Key, Compare>::Add(OtherIterator itBegin, OtherIterator itEnd)
{
    m_set.insert(itBegin, itEnd);
}

template <class Key, class Compare>
void Set<Key, Compare>::Remove(const Key &key)
{
    m_set.erase(key);
}

template <class Key, class Compare>
typename Set<Key, Compare>::Iterator Set<Key, Compare>::Remove(
    typename Set<Key, Compare>::Iterator it)
{
    return m_set.erase(it);
}

template <class Key, class Compare>
void Set<Key, Compare>::Clear()
{
    m_set.clear();
}

template <class Key, class Compare>
int Set<Key, Compare>::Size() const
{
    return m_set.size();
}

template <class Key, class Compare>
bool Set<Key, Compare>::IsEmpty() const
{
    return m_set.empty();
}

template <class Key, class Compare>
bool Set<Key, Compare>::Contains(const Key &key) const
{
    return m_set.find(key) != m_set.end();
}

template <class Key, class Compare>
Array<Key> Set<Key, Compare>::GetKeys() const
{
    Array<Key> result;
    for (Const_Iterator cit = this->Begin(); cit != this->End(); ++cit)
    {
        result.PushBack(*cit);
    }
    return result;
}

template <class Key, class Compare>
typename Set<Key, Compare>::Iterator Set<Key, Compare>::Begin()
{
    return m_set.begin();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Iterator Set<Key, Compare>::End()
{
    return m_set.end();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Const_Iterator Set<Key, Compare>::Begin() const
{
    return m_set.cbegin();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Const_Iterator Set<Key, Compare>::End() const
{
    return m_set.cend();
}

template <class Key, class Compare>
typename Set<Key, Compare>::RIterator Set<Key, Compare>::RBegin()
{
    return m_set.rbegin();
}

template <class Key, class Compare>
typename Set<Key, Compare>::RIterator Set<Key, Compare>::REnd()
{
    return m_set.rend();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Const_RIterator Set<Key, Compare>::RBegin() const
{
    return m_set.crbegin();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Const_RIterator Set<Key, Compare>::REnd() const
{
    return m_set.crend();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Iterator Set<Key, Compare>::begin()
{
    return m_set.begin();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Iterator Set<Key, Compare>::end()
{
    return m_set.end();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Const_Iterator Set<Key, Compare>::begin() const
{
    return m_set.cbegin();
}

template <class Key, class Compare>
typename Set<Key, Compare>::Const_Iterator Set<Key, Compare>::end() const
{
    return m_set.cend();
}
}
