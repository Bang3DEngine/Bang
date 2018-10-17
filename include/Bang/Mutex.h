#ifndef MUTEX_H
#define MUTEX_H

#include <mutex>

#include "Bang/BangDefines.h"

namespace Bang
{
class Mutex
{
public:
    Mutex();
    ~Mutex();

    void Lock();
    bool TryLock();
    void UnLock();

private:
    std::mutex m_mutex;
};
}

#endif  // MUTEX_H
