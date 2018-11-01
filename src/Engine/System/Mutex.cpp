#include "Bang/Mutex.h"

using namespace Bang;

Mutex::Mutex()
{
}

Mutex::~Mutex()
{
}

void Mutex::Lock()
{
    if (!IsLocked())
    {
        m_mutex.lock();
        m_isLocked = true;
    }
}

bool Mutex::TryLock()
{
    if (!IsLocked())
    {
        return m_mutex.try_lock();
    }
    return true;
}

void Mutex::UnLock()
{
    if (IsLocked())
    {
        m_mutex.unlock();
        m_isLocked = false;
    }
}

bool Mutex::IsLocked() const
{
    return m_isLocked;
}
