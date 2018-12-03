#ifndef ARRAY_H
#define ARRAY_H

#include <functional>
#include <vector>

#include "Bang/Containers.h"

namespace Bang
{
template <class T>
class Array
{
public:
    using Ref = typename std::vector<T>::reference;
    using ConstRef = typename std::vector<T>::const_reference;
    using Iterator = typename std::vector<T>::iterator;
    using RIterator = typename std::vector<T>::reverse_iterator;
    using Const_Iterator = typename std::vector<T>::const_iterator;
    using Const_RIterator = typename std::vector<T>::const_reverse_iterator;

    static const Array<T> &Empty();

    Array();
    Array(const std::vector<T> &v);
    explicit Array(std::size_t size);
    explicit Array(std::size_t size, const T &initValue);
    Array(std::initializer_list<T> l);
    template <class OtherIterator>
    explicit Array(OtherIterator begin, OtherIterator end);
    template <template <class> class Container>
    explicit Array(const Container<T> &container);

    void Insert(const T &x, int index);

    void PushBack(const T &x);
    void PushFront(const T &x);

    template <class IteratorClass>
    void PushBack(IteratorClass itBegin, IteratorClass itEnd);

    template <template <class OtherT> class Container, class OtherT>
    void PushBack(const Container<OtherT> &container);

    T *Data();
    const T *Data() const;

    Const_Iterator Find(const T &x) const;
    Iterator Find(const T &x);
    Iterator FindLast(const T &x);
    bool Contains(const T &x) const;

    const T &Front() const;
    const T &Back() const;
    T &Front();
    T &Back();

    Iterator Remove(const Iterator &first, const Iterator &last);
    Iterator Remove(Iterator it);
    Iterator Remove(const T &x);
    Iterator RemoveByIndex(std::size_t i);
    void RemoveAll(const T &x);

    void PopBack();
    void PopFront();

    int IndexOf(const T &x) const;

    void Reserve(std::size_t n);
    void Resize(std::size_t n, const T &value = T());
    uint Size() const;
    void Clear();
    bool IsEmpty() const;

    void Reverse();
    void Sort();
    template <class StrictWeakOrdering>
    void Sort(const StrictWeakOrdering &sortClass);

    typename Array<T>::Ref At(std::size_t i);
    typename Array<T>::ConstRef At(std::size_t i) const;
    typename Array<T>::Ref operator[](std::size_t i);
    typename Array<T>::ConstRef operator[](std::size_t i) const;
    bool operator==(const Array<T> &rhs) const;
    bool operator!=(const Array<T> &rhs) const;

    template <template <class> class Container, class OtherT = T>
    Container<OtherT> To() const;

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
    std::vector<T> m_vector;
};
}  // namespace Bang

#include "Array.tcc"

#endif  // ARRAY_H
