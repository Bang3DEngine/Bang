#include "Bang/Time.h"

#include "Bang/Scene.h"
#include "Bang/SceneManager.h"
#include "Bang/TimeSingleton.h"

using namespace Bang;

Time::Time(uint64_t timeNanos)
{
    m_timeNanos = timeNanos;
}

void Time::SetSeconds(double seconds)
{
    m_timeNanos = SCAST<uint64_t>(seconds * 1e9);
}

void Time::SetMillis(uint64_t millis)
{
    m_timeNanos = SCAST<uint64_t>(millis * 1e6);
}

void Time::SetNanos(uint64_t nanos)
{
    m_timeNanos = nanos;
}

void Time::SetInfinity()
{
    *this = Time::Infinity();
}

void Time::SetZero()
{
    *this = Time::Zero();
}

double Time::GetSeconds() const
{
    return GetNanos() / SCAST<double>(1e9);
}

uint64_t Time::GetMillis() const
{
    return SCAST<uint64_t>(GetNanos() / 1e6);
}

uint64_t Time::GetNanos() const
{
    return m_timeNanos;
}

Time Time::Zero()
{
    return Time(0);
}

Time Time::Infinity()
{
    return Time(SCAST<uint64_t>(SCAST<uint>(-1)));
}

Time Time::Seconds(double seconds)
{
    Time time;
    time.SetSeconds(seconds);
    return time;
}

Time Time::Millis(uint64_t millis)
{
    Time time;
    time.SetMillis(millis);
    return time;
}

Time Time::Nanos(uint64_t nanos)
{
    Time time;
    time.SetNanos(nanos);
    return time;
}

Time Time::GetNow()
{
    return TimeSingleton::GetInstance()->GetNow();
}

Time Time::GetInit()
{
    return TimeSingleton::GetInstance()->GetInit();
}

Time Time::GetEllapsed()
{
    return TimeSingleton::GetInstance()->GetEllapsed();
}

Time Time::GetDeltaTime()
{
    if (Scene *scene = SceneManager::GetActiveScene())
    {
        return scene->GetDeltaTime();
    }
    return Time(0);
}

Time Time::GetPassedTimeSince(Time refTime)
{
    Time now = Time::GetNow();
    if (now <= refTime)
    {
        return Time(0);
    }
    return (now - refTime);
}
