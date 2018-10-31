#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/String.h"

namespace Bang
{
class Thread;
class ThreadRunnable;

class ThreadPool
{
public:
    ThreadPool();
    virtual ~ThreadPool();

    bool TryStart(ThreadRunnable *runnable);

    void SetName(const String &name);
    void SetMaxThreadCount(int maxThreadCount);

    const String &GetName() const;
    int GetMaxThreadCount() const;

private:
    String m_threadsName = "BangPooledThread";
    List<Thread *> m_threadList;

    uint m_maxThreadCount = 32;
};
}  // namespace Bang

#endif  // THREADPOOL_H
