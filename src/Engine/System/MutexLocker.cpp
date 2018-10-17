#include "Bang/MutexLocker.h"

#include "Bang/Mutex.h"

using namespace Bang;

MutexLocker::MutexLocker(Mutex *mutex)
{
    p_mutex = mutex;
    p_mutex->Lock();
}

MutexLocker::~MutexLocker()
{
    p_mutex->UnLock();
}
