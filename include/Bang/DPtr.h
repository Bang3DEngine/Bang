#ifndef DPTR_H
#define DPTR_H

#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsDestroy.h"

namespace Bang
{
template <class T>
class DPtr : public EventListener<IEventsDestroy>
{
public:
    using ChangedCallback = std::function<void(T *oldPtr, T *newPtr)>;

    DPtr();
    DPtr(T *ptr);

    ~DPtr();

    void Set(T *ptr);
    void SetChangedCallback(const ChangedCallback &changedCallback);

    T *Get() const;

    operator T *() const;
    operator const T *() const;

    template <class OtherT>
    DPtr<T> &operator=(const DPtr<OtherT> &ptr);

    template <class OtherT>
    DPtr<T> &operator=(OtherT *ptr);

    T &operator*();
    const T &operator*() const;

    T *operator->();
    const T *operator->() const;
    operator bool() const;

private:
    T *p_ptr = nullptr;
    ChangedCallback m_changedCallback;

    // IEvenntsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;
};

template <class LHST, class AHST>
inline bool operator==(const LHST *lhs, const DPtr<AHST> &rhs)
{
    return (SCAST<AHST>(lhs) == rhs.Get());
}
template <class LHST, class AHST>
inline bool operator==(const DPtr<LHST> &lhs, const AHST *rhs)
{
    return (lhs.Get() == SCAST<LHST>(rhs));
}
template <class LHST, class AHST>
inline bool operator==(const DPtr<LHST> &lhs, const DPtr<AHST> &rhs)
{
    return (lhs.Get() == rhs.Get());
}

template <class LHST, class AHST>
inline bool operator!=(const LHST *lhs, const DPtr<AHST> &rhs)
{
    return (SCAST<AHST>(lhs) != rhs.Get());
}
template <class LHST, class AHST>
inline bool operator!=(const DPtr<LHST> &lhs, const AHST *rhs)
{
    return (lhs.Get() != SCAST<LHST>(rhs));
}
template <class LHST, class AHST>
inline bool operator!=(const DPtr<LHST> &lhs, const DPtr<AHST> &rhs)
{
    return (lhs.Get() != rhs.Get());
}
}

#include "Bang/DPtr.tcc"

#endif  // DPTR_H
