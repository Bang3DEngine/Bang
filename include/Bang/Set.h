#ifndef SET_H
#define SET_H

#include <set>
#include "Bang/Bang.h"

namespace Bang
{
template <class>
class Array;

template <class Key, class Compare = std::less<Key>>
class Set
{
public:
    using Iterator = typename std::set<Key, Compare>::iterator;
    using RIterator = typename std::set<Key, Compare>::reverse_iterator;
    using Const_Iterator = typename std::set<Key, Compare>::const_iterator;
    using Const_RIterator =
        typename std::set<Key, Compare>::const_reverse_iterator;

    Set();
    Set(const std::set<Key, Compare> &s);

    template <class OtherIterator>
    Set(OtherIterator itBegin, OtherIterator itEnd);

    void Add(const Key &key);

    template <class OtherIterator>
    void Add(OtherIterator itBegin, OtherIterator itEnd);

    void Remove(const Key &key);
    Iterator Remove(Iterator it);

    void Clear();
    int Size() const;
    bool IsEmpty() const;
    bool Contains(const Key &key) const;

    Array<Key> GetKeys() const;

    Iterator Begin();
    Iterator End();
    Const_Iterator Begin() const;
    Const_Iterator End() const;
    RIterator RBegin();
    RIterator REnd();
    Const_RIterator RBegin() const;
    Const_RIterator REnd() const;

    // To allow range-based for loops
    Iterator begin();
    Iterator end();
    Const_Iterator begin() const;
    Const_Iterator end() const;

private:
    std::set<Key, Compare> m_set;
};
}

#include "Set.tcc"

#endif  // SET_H
