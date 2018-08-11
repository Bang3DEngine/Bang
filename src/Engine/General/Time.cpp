#include "Bang/Time.h"

#include <chrono>

#include "Bang/Application.h"

USING_NAMESPACE_BANG

float Time::GetDeltaTime()
{
    Time *time = Time::GetInstance();
    return (Time::GetNow_Millis() - time->m_deltaTimeReferenceNanos) / 1000.0f;
}

double Time::GetNow_Seconds()
{
    return GetNow_Nanos() / SCAST<double>(1e9);
}

Time::TimeT Time::GetNow_Millis()
{
    return GetNow_Nanos() / 1e6;
}

Time::TimeT Time::GetNow_Nanos()
{
    return std::chrono::system_clock::now().time_since_epoch() /
           std::chrono::nanoseconds(1);
}

double Time::GetEllapsed_Seconds()
{
    return Time::GetEllapsed_Nanos() / SCAST<double>(1e9);
}

Time::TimeT Time::GetEllapsed_Millis()
{
    return Time::GetEllapsed_Nanos() / 1e6;
}

Time::TimeT Time::GetEllapsed_Nanos()
{
    return (std::chrono::system_clock::now().time_since_epoch() /
            std::chrono::nanoseconds(1)) -
           Time::GetInstance()->m_initialTimeNanos;
}

void Time::SetDeltaTime(double seconds)
{
    Time::GetInstance()->m_deltaTimeReferenceNanos = Time::GetNow_Millis() -
                                                     (seconds * 1000);
}

void Time::SetDeltaTimeReferenceToNow()
{
    Time::GetInstance()->m_deltaTimeReferenceNanos = Time::GetNow_Millis();
}

Time::Time()
{
    m_initialTimeNanos = Time::GetNow_Nanos();
}

Time *Time::GetInstance()
{
    return Application::GetInstance()->GetTime();
}
