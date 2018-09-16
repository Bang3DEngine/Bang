#pragma once

#include "Bang/DPtr.h"

NAMESPACE_BANG_BEGIN

template <class T>
DPtr<T>::DPtr()
{

}

template <class T>
DPtr<T>::DPtr(T *ptr)
{
    Set(ptr);
}


template <class T>
DPtr<T>::~DPtr()
{

}

template<class T>
void DPtr<T>::Set(T *ptr)
{
    if (ptr != p_ptr)
    {
        if (p_ptr)
        {
            p_ptr->EventEmitter<IEventsDestroy>::UnRegisterListener(this);
        }

        T *oldPtr = p_ptr;
        T *newPtr = ptr;

        p_ptr = ptr;
        if (p_ptr)
        {
            p_ptr->EventEmitter<IEventsDestroy>::RegisterListener(this);
        }

        if (m_changedCallback)
        {
            m_changedCallback(oldPtr, newPtr);
        }
    }
}

template<class T>
void DPtr<T>::SetChangedCallback(const DPtr<T>::ChangedCallback &changedCallback)
{
    m_changedCallback = changedCallback;
}

template<class T>
T *DPtr<T>::Get() const
{
    return p_ptr;
}

template <class T>
DPtr<T>::operator T*() const
{
    return p_ptr;
}
template <class T>
DPtr<T>::operator const T*() const
{
    return p_ptr;
}

template<class T>
template<class OtherT>
DPtr<T>& DPtr<T>::operator=(const DPtr<OtherT> &ptr)
{
    Set(ptr.Get());
    return *this;
}

template<class T>
template<class OtherT>
DPtr<T>& DPtr<T>::operator=(OtherT *ptr)
{
    Set(ptr);
    return *this;
}

template <class T>
T& DPtr<T>::operator*()
{
    ASSERT(p_ptr);
    return *p_ptr;
}
template <class T>
const T& DPtr<T>::operator*() const
{
    ASSERT(p_ptr);
    return *p_ptr;
}

template <class T>
T* DPtr<T>::operator->()
{
    return p_ptr;
}
template <class T>
const T* DPtr<T>::operator->() const
{
    return p_ptr;
}

template<class T>
void DPtr<T>::OnDestroyed(EventEmitter<IEventsDestroy> *)
{
    ASSERT(p_ptr);
    Set(nullptr);
}

template <class T>
DPtr<T>::operator bool() const
{
    return (p_ptr != nullptr);
}

NAMESPACE_BANG_END

