#include "Bang/MutexLocker.h"

#include "Bang/Mutex.h"

USING_NAMESPACE_BANG

MutexLocker::MutexLocker(Mutex *mutex)
{
    p_mutex = mutex;
    p_mutex->Lock();
}

MutexLocker::~MutexLocker()
{
    p_mutex->UnLock();
}
