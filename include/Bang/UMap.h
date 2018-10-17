#ifndef UMAP_H
#define UMAP_H

#include <unordered_map>

#include "Bang/Bang.h"

namespace Bang
{
template <class>
class Array;

template <class Key,
          class Value,
          class Hash = std::hash<Key>,
          class Pred = std::equal_to<Key>,
          class Alloc = std::allocator<std::pair<const Key, Value>>>
class UMap
{
public:
    using Iterator =
        typename std::unordered_map<Key, Value, Hash, Pred, Alloc>::iterator;
    using Const_Iterator = typename std::
        unordered_map<Key, Value, Hash, Pred, Alloc>::const_iterator;

    UMap();
    UMap(const std::unordered_map<Key, Value, Hash, Pred, Alloc> &m);

    void Add(const Key &key, const Value &value = Value());

    void Remove(const Key &key);
    Iterator Remove(Iterator it);
    void RemoveValues(const Value &value);

    Value &Get(const Key &key);
    const Value &Get(const Key &key) const;
    Array<Key> GetKeysWithValue(const Value &v) const;
    Array<Key> GetKeys() const;
    Array<Value> GetValues() const;

    void Clear();
    int Size() const;
    bool IsEmpty() const;

    Iterator Find(const Key &key);
    Const_Iterator Find(const Key &key) const;
    bool ContainsKey(const Key &key) const;
    bool ContainsValue(const Value &value) const;

    Value &operator[](const Key &k);
    const Value &operator[](const Key &k) const;

    Iterator Begin();
    Iterator End();
    Const_Iterator Begin() const;
    Const_Iterator End() const;
    Const_Iterator CBegin() const;
    Const_Iterator CEnd() const;

    // To allow range-based for loops
    Iterator begin();
    Iterator end();
    Const_Iterator begin() const;
    Const_Iterator end() const;
    Const_Iterator cbegin() const;
    Const_Iterator cend() const;

private:
    std::unordered_map<Key, Value, Hash, Pred, Alloc> m_umap;
};
}

#include "Bang/UMap.tcc"

#endif  // UMAP_H
