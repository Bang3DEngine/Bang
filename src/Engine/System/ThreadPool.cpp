#include "Bang/ThreadPool.h"

#include <list>

#include "Bang/List.tcc"
#include "Bang/Thread.h"

using namespace Bang;

ThreadPool::ThreadPool()
{
}

ThreadPool::~ThreadPool()
{
}

bool ThreadPool::TryStart(ThreadRunnable *runnable)
{
    auto it = m_threadList.Begin();
    while (it != m_threadList.End())
    {
        Thread *thread = *it;
        if (thread->HasFinished())
        {
            it = m_threadList.Remove(it);
        }
        else
        {
            ++it;
        }
    }

    if (m_threadList.Size() >= m_maxThreadCount)
    {
        return false;
    }

    String threadName = m_threadsName + String::ToString(m_threadList.Size());
    Thread *thread = new Thread(runnable, threadName);
    thread->Start();
    thread->Detach();
    m_threadList.PushBack(thread);
    return true;
}

void ThreadPool::SetName(const String &name)
{
    if (name != GetName())
    {
        m_threadsName = name;
    }
}

void ThreadPool::SetMaxThreadCount(int maxThreadCount)
{
    m_maxThreadCount = maxThreadCount;
}

const String &ThreadPool::GetName() const
{
    return m_threadsName;
}

int ThreadPool::GetMaxThreadCount() const
{
    return m_maxThreadCount;
}
