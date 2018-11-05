#pragma once

#include "Bang/USet.h"

namespace Bang
{
template <class Key, class Hash, class Pred, class Alloc>
USet<Key, Hash, Pred, Alloc>::USet(
    const std::unordered_set<Key, Hash, Pred, Alloc> &s)
    : m_uset(s)
{
}

template <class Key, class Hash, class Pred, class Alloc>
void USet<Key, Hash, Pred, Alloc>::Add(const Key &key)
{
    m_uset.insert(key);
}

template <class Key, class Hash, class Pred, class Alloc>
template <class OtherIterator>
void USet<Key, Hash, Pred, Alloc>::Add(OtherIterator itBegin,
                                       OtherIterator itEnd)
{
    m_uset.insert(itBegin, itEnd);
}

template <class Key, class Hash, class Pred, class Alloc>
void USet<Key, Hash, Pred, Alloc>::Remove(const Key &key)
{
    m_uset.erase(key);
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Iterator
USet<Key, Hash, Pred, Alloc>::Remove(Iterator it)
{
    return m_uset.erase(it);
}

template <class Key, class Hash, class Pred, class Alloc>
void USet<Key, Hash, Pred, Alloc>::Clear()
{
    m_uset.clear();
}

template <class Key, class Hash, class Pred, class Alloc>
int USet<Key, Hash, Pred, Alloc>::Size() const
{
    return m_uset.size();
}

template <class Key, class Hash, class Pred, class Alloc>
bool USet<Key, Hash, Pred, Alloc>::IsEmpty() const
{
    return m_uset.empty();
}

template <class Key, class Hash, class Pred, class Alloc>
bool USet<Key, Hash, Pred, Alloc>::Contains(const Key &key) const
{
    return m_uset.find(key) != m_uset.end();
}

template <class Key, class Hash, class Pred, class Alloc>
Array<Key> USet<Key, Hash, Pred, Alloc>::GetKeys() const
{
    Array<Key> result;
    for (Const_Iterator cit = this->Begin(); cit != this->End(); ++cit)
    {
        result.PushBack(*cit);
    }
    return result;
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Iterator
USet<Key, Hash, Pred, Alloc>::Begin()
{
    return m_uset.begin();
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Iterator
USet<Key, Hash, Pred, Alloc>::End()
{
    return m_uset.end();
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Const_Iterator
USet<Key, Hash, Pred, Alloc>::Begin() const
{
    return m_uset.cbegin();
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Const_Iterator
USet<Key, Hash, Pred, Alloc>::End() const
{
    return m_uset.cend();
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Iterator
USet<Key, Hash, Pred, Alloc>::begin()
{
    return m_uset.begin();
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Iterator
USet<Key, Hash, Pred, Alloc>::end()
{
    return m_uset.end();
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Const_Iterator
USet<Key, Hash, Pred, Alloc>::begin() const
{
    return m_uset.begin();
}

template <class Key, class Hash, class Pred, class Alloc>
typename USet<Key, Hash, Pred, Alloc>::Const_Iterator
USet<Key, Hash, Pred, Alloc>::end() const
{
    return m_uset.end();
}
}
