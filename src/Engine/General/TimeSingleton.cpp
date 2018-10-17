#include "Bang/TimeSingleton.h"

#include <chrono>
#include <ratio>

#include "Bang/Application.h"

using namespace Bang;

TimeSingleton::TimeSingleton()
{
}

Time TimeSingleton::GetNow()
{
    Time now;
    now.SetNanos(std::chrono::system_clock::now().time_since_epoch() /
                 std::chrono::nanoseconds(1));
    return now;
}

Time TimeSingleton::GetInit()
{
    return TimeSingleton::GetInstance()->m_initialTime;
}

Time TimeSingleton::GetEllapsed()
{
    return TimeSingleton::GetNow() - TimeSingleton::GetInit();
}

void TimeSingleton::SetInitTime(Time time)
{
    TimeSingleton::GetInstance()->m_initialTime = time;
}

TimeSingleton *TimeSingleton::GetInstance()
{
    return Application::GetInstance()->GetTime();
}
