#ifndef MAP_H
#define MAP_H

#include <map>
#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD_T class List;

template <class Key, class Value, class Compare = std::less<Key>>
class Map
{
public:
    using Iterator = typename std::map<Key, Value, Compare>::iterator;
    using RIterator = typename std::map<Key, Value, Compare>::reverse_iterator;
    using Const_Iterator = typename std::map<Key, Value, Compare>::const_iterator;
    using Const_RIterator = typename std::map<Key, Value, Compare>::const_reverse_iterator;

    Map();
    Map(const std::map<Key, Value, Compare> &m);

    void Add(const Key &key, const Value &value = Value());

    void Remove(const Key &key);
    Iterator Remove(Iterator it);
    void RemoveValues(const Value &value);

    Value& Get(const Key &key);
    const Value& Get(const Key &key) const;
    List<Key> GetKeysWithValue(const Value& v) const;
    List<Key> GetKeys() const;
    List<Value> GetValues() const;

    void Clear();
    int Size() const;
    bool IsEmpty() const;

    Iterator Find(const Key &key);
    Const_Iterator Find(const Key &key) const;
    bool ContainsKey(const Key &key) const;
    bool ContainsValue(const Value &value) const;

    Value& operator[](const Key &k);
    const Value& operator[](const Key &k) const;

    Iterator Begin();
    Iterator End();
    Const_Iterator Begin() const;
    Const_Iterator End() const;
    Const_Iterator CBegin() const;
    Const_Iterator CEnd() const;
    RIterator RBegin();
    RIterator REnd();
    Const_RIterator CRBegin() const;
    Const_RIterator CREnd() const;

    // To allow range-based for loops
    Iterator begin();
    Iterator end();
    Const_Iterator begin() const;
    Const_Iterator end() const;
    Const_Iterator cbegin() const;
    Const_Iterator cend() const;

private:
    std::map<Key, Value, Compare> m_map;
};

NAMESPACE_BANG_END

#include "Bang/Map.tcc"

#endif // MAP_H
