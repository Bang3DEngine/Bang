#ifndef MUTEXLOCKER_H
#define MUTEXLOCKER_H

#include "Bang/BangDefines.h"

NAMESPACE_BANG_BEGIN

FORWARD class Mutex;

class MutexLocker
{
public:
    MutexLocker(Mutex *mutex);
    ~MutexLocker();

private:
    Mutex *p_mutex;
};

NAMESPACE_BANG_END

#endif // MUTEXLOCKER_H
