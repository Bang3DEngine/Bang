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

    bool IsLocked() const;

private:
    bool m_isLocked = false;
    std::mutex m_mutex;
};
}  // namespace Bang

#endif  // MUTEX_H
