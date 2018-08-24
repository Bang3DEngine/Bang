#ifndef ARRAY_H
#define ARRAY_H

#include <vector>
#include <functional>

#include "Bang/TypeTraits.h"
#include "Bang/Containers.h"

NAMESPACE_BANG_BEGIN

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

    static const Array<T>& Empty();

    Array();
    Array(const std::vector<T> &v);
    Array(int size);
    Array(int size, const T& initValue);
    Array(std::initializer_list<T> l);
    template <class OtherIterator>
    explicit Array(OtherIterator begin, OtherIterator end);
    template < template<class> class Container>
    explicit Array(const Container<T> &container);

    void Insert(const T& x, int index);

    void PushBack(const T& x);

    template<class IteratorClass>
    void PushBack(IteratorClass itBegin, IteratorClass itEnd);

    template <template <class OtherT> class Container, class OtherT>
    void PushBack(const Container<OtherT>& container);

    T* Data();
    const T* Data() const;

    Const_Iterator Find(const T& x) const;
    Iterator Find(const T& x);
    Iterator FindLast(const T& x);
    bool Contains(const T &x) const;

    const T& Front() const;
    const T& Back() const;
    T& Front();
    T& Back();

    Iterator Remove(const Iterator &first, const Iterator &last);
    Iterator Remove(Iterator it);
    Iterator Remove(const T& x);
    void RemoveAll(const T& x);

    T& PopBack();

    int IndexOf(const T& x) const;

    void Resize(int n);
    uint Size() const;
    void Clear();
    bool IsEmpty() const;

    void Sort();
    template<class StrictWeakOrdering>
    void Sort(const StrictWeakOrdering &sortClass);

    typename Array<T>::Ref At(std::size_t i);
    typename Array<T>::ConstRef At(std::size_t i) const;
    typename Array<T>::Ref operator[](std::size_t i);
    typename Array<T>::ConstRef operator[](std::size_t i) const;
    bool operator==(const Array<T> &rhs) const;

    template< template <class> class Container, class OtherT = T>
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

template<class T, class... Args>
Array<const T&> MakeArray(const T& x, Args&&... args)
{
    if (sizeof...(args) == 0) { return Array<const T&>(); }

    Array<const T&> res;
    res.PushBack( Cast<const T&>(x) );
    for (auto arg : {args...}) { res.PushBack( Cast<const T&>(arg) ); }
    return res;
}

NAMESPACE_BANG_END

#include "Array.tcc"

#endif // ARRAY_H
