#include "Bang/TimeSingleton.h"

#include "Bang/Application.h"

USING_NAMESPACE_BANG

TimeSingleton::TimeSingleton()
{
    m_initialTime = GetNow();
}

Time TimeSingleton::GetNow()
{
    Time now;
    now.SetNanos(std::chrono::system_clock::now().time_since_epoch() /
                 std::chrono::nanoseconds(1));
    return now;
}

Time TimeSingleton::GetEllapsed()
{
    return TimeSingleton::GetNow() - TimeSingleton::GetInstance()->m_initialTime;
}

Time TimeSingleton::GetDeltaTime()
{
    return Time::GetNow() - TimeSingleton::GetInstance()->m_deltaTimeReference;
}

void TimeSingleton::SetDeltaTimeReferenceToNow()
{
    TimeSingleton::GetInstance()->m_deltaTimeReference = TimeSingleton::GetNow();
}

TimeSingleton *TimeSingleton::GetInstance()
{
    return Application::GetInstance()->GetTime();
}
