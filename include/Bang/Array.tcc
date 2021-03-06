#pragma once

#include "Bang/Array.h"
#include "Bang/Assert.h"
#include "Bang/Containers.h"

namespace Bang
{
template <class T>
Array<T>::Array()
{
}

template <class T>
Array<T>::Array(const std::vector<T> &v) : m_vector(v)
{
}

template <class T>
Array<T>::Array(std::size_t size) : m_vector(size)
{
}

template <class T>
Array<T>::Array(std::size_t size, const T &initValue)
    : m_vector(size, initValue)
{
}

template <class T>
Array<T>::Array(std::initializer_list<T> l) : m_vector(l)
{
}

template <class T>
void Array<T>::Insert(const T &x, int index)
{
    ASSERT(index >= 0 && index <= SCAST<int>(Size()));
    Resize(Size() + 1);

    for (int i = Size() - 1; i > index; --i)
    {
        this->At(i) = this->At(i - 1);
    }
    this->At(index) = x;
}

template <class T>
template <class OtherIterator>
Array<T>::Array(OtherIterator begin, OtherIterator end) : m_vector(begin, end)
{
}

template <class T>
template <template <class> class Container>
Array<T>::Array(const Container<T> &container)
    : Array(container.begin(), container.end())
{
}

template <class T>
void Array<T>::PushBack(const T &x)
{
    m_vector.push_back(x);
}

template <class T>
void Array<T>::PushBack(T &&x)
{
    m_vector.push_back(x);
}

template <class T>
void Array<T>::PushFront(const T &x)
{
    Insert(x, 0);
}

template <class T>
template <template <class OtherT> class Container, class OtherT>
void Array<T>::PushBack(const Container<OtherT> &container)
{
    for (auto it = container.Begin(); it != container.End(); ++it)
    {
        PushBack(*it);
    }
}

template <class T>
T *Array<T>::Data()
{
    return m_vector.data();
}

template <class T>
const T *Array<T>::Data() const
{
    return m_vector.data();
}

template <class T>
typename Array<T>::Const_Iterator Array<T>::Find(const T &x) const
{
    return Containers::Find(*this, x);
}

template <class T>
typename Array<T>::Iterator Array<T>::Find(const T &x)
{
    return Containers::Find(*this, x);
}

template <class T>
typename Array<T>::Iterator Array<T>::FindLast(const T &x)
{
    for (auto it = m_vector.RBegin(); it != m_vector.REnd(); ++it)
    {
        if (*it == x)
        {
            Iterator res = it.base();
            std::advance(res, -1);
            return res;
        }
    }
    return m_vector.End();
}

template <class T>
bool Array<T>::Contains(const T &x) const
{
    return Containers::Contains(*this, x);
}

template <class T>
const T &Array<T>::Front() const
{
    return m_vector.front();
}

template <class T>
const T &Array<T>::Back() const
{
    return m_vector.back();
}

template <class T>
T &Array<T>::Front()
{
    return m_vector.front();
}

template <class T>
T &Array<T>::Back()
{
    return m_vector.back();
}

template <class T>
typename Array<T>::Iterator Array<T>::Remove(const Array<T>::Iterator &first,
                                             const Array<T>::Iterator &last)
{
    return m_vector.erase(first, last);
}

template <class T>
typename Array<T>::Iterator Array<T>::Remove(Array<T>::Iterator it)
{
    return m_vector.erase(it);
}

template <class T>
typename Array<T>::Iterator Array<T>::Remove(const T &x)
{
    Iterator it = Find(x);
    if (it != End())
    {
        return Remove(it);
    }
    return End();
}

template <class T>
typename Array<T>::Iterator Array<T>::RemoveByIndex(std::size_t i)
{
    Iterator it = Begin();
    std::advance(it, i);
    if (it != End())
    {
        return Remove(it);
    }
    return End();
}

template <class T>
void Array<T>::RemoveAll(const T &x)
{
    for (Iterator it = Begin(); it != End();)
    {
        if (*it == x)
        {
            it = Remove(it);
        }
        else
        {
            ++it;
        }
    }
}

template <class T>
void Array<T>::PopBack()
{
    m_vector.pop_back();
}

template <class T>
void Array<T>::PopFront()
{
    RemoveByIndex(0);
}

template <class T>
int Array<T>::IndexOf(const T &x) const
{
    int i = 0;
    for (const T &y : *this)
    {
        if (x == y)
        {
            return i;
        }
        ++i;
    }
    return -1;
}

template <class T>
void Array<T>::Reserve(std::size_t n)
{
    m_vector.reserve(n);
}

template <class T>
void Array<T>::Resize(std::size_t n, const T &value)
{
    m_vector.resize(n, value);
}

template <class T>
uint Array<T>::Size() const
{
    return SCAST<uint>(m_vector.size());
}

template <class T>
void Array<T>::Clear()
{
    m_vector.clear();
}

template <class T>
bool Array<T>::IsEmpty() const
{
    return Size() == 0;
}

template <class T>
void Array<T>::Reverse()
{
    if (Size() >= 1)
    {
        std::reverse(Begin(), End());
    }
}

template <class T>
void Array<T>::Sort()
{
    if (!IsEmpty())
    {
        Containers::Sort(Begin(), End());
    }
}

template <class T>
template <class StrictWeakOrdering>
void Array<T>::Sort(const StrictWeakOrdering &sortClass)
{
    if (!IsEmpty())
    {
        Containers::Sort(Begin(), End(), sortClass);
    }
}

template <class T>
typename Array<T>::Ref Array<T>::At(std::size_t i)
{
    return m_vector.at(i);
}

template <class T>
typename Array<T>::ConstRef Array<T>::At(std::size_t i) const
{
    return m_vector.at(i);
}

template <class T>
typename Array<T>::Ref Array<T>::operator[](std::size_t i)
{
    return m_vector[i];
}

template <class T>
typename Array<T>::ConstRef Array<T>::operator[](std::size_t i) const
{
    return m_vector[i];
}

template <class T>
bool Array<T>::operator==(const Array<T> &rhs) const
{
    if (Size() != rhs.Size())
    {
        return false;
    }

    for (uint i = 0; i < Size(); ++i)
    {
        if (At(i) != rhs.At(i))
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool Array<T>::operator!=(const Array<T> &rhs) const
{
    return !(*this == rhs);
}

template <class T>
template <template <class> class Container, class OtherT>
Container<OtherT> Array<T>::To() const
{
    Container<OtherT> cont;
    for (const T &x : *this)
    {
        cont.PushBack(OtherT(x));
    }
    return cont;
}

template <class T>
typename Array<T>::Iterator Array<T>::Begin()
{
    return m_vector.begin();
}

template <class T>
typename Array<T>::Iterator Array<T>::End()
{
    return m_vector.end();
}

template <class T>
typename Array<T>::Const_Iterator Array<T>::Begin() const
{
    return m_vector.cbegin();
}

template <class T>
typename Array<T>::Const_Iterator Array<T>::End() const
{
    return m_vector.cend();
}

template <class T>
typename Array<T>::RIterator Array<T>::RBegin()
{
    return m_vector.rbegin();
}

template <class T>
typename Array<T>::RIterator Array<T>::REnd()
{
    return m_vector.rend();
}

template <class T>
typename Array<T>::Const_RIterator Array<T>::RBegin() const
{
    return m_vector.crbegin();
}

template <class T>
typename Array<T>::Const_RIterator Array<T>::REnd() const
{
    return m_vector.crend();
}

template <class T>
typename Array<T>::Iterator Array<T>::begin()
{
    return m_vector.begin();
}

template <class T>
typename Array<T>::Iterator Array<T>::end()
{
    return m_vector.end();
}

template <class T>
typename Array<T>::Const_Iterator Array<T>::begin() const
{
    return m_vector.cbegin();
}

template <class T>
typename Array<T>::Const_Iterator Array<T>::end() const
{
    return m_vector.cend();
}

template <class T>
const std::vector<T>& Array<T>::GetVector() const
{
    return m_vector;
}

template <class T>
template <class IteratorClass>
void Array<T>::PushBack(IteratorClass itBegin, IteratorClass itEnd)
{
    IteratorClass it = itBegin;
    while (it != itEnd)
    {
        PushBack(*it);
        ++it;
    }
}

template <class T>
const Array<T> &Array<T>::Empty()
{
    static Array<T> empty;
    return empty;
}
}  // namespace Bang
