#ifndef USET_H
#define USET_H

#include <unordered_set>

#include "Bang/Bang.h"

namespace Bang
{
template <class Key,
          class Hash = std::hash<Key>,
          class Pred = std::equal_to<Key>,
          class Alloc = std::allocator<Key>>
class USet
{
public:
    using Iterator =
        typename std::unordered_set<Key, Hash, Pred, Alloc>::iterator;
    using Const_Iterator =
        typename std::unordered_set<Key, Hash, Pred, Alloc>::const_iterator;

    USet() = default;
    USet(const std::unordered_set<Key, Hash, Pred, Alloc> &s);
    ~USet() = default;

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

    // To allow range-based for loops
    Iterator begin();
    Iterator end();
    Const_Iterator begin() const;
    Const_Iterator end() const;

private:
    std::unordered_set<Key, Hash, Pred, Alloc> m_uset;
};
}

#include "Bang/USet.tcc"

#endif  // USET_H
