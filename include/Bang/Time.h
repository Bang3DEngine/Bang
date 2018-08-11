#ifndef TIME_H
#define TIME_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

class Time
{
public:
    using TimeT = uint64_t;

    static float GetDeltaTime();
    static double GetNow_Seconds();
    static TimeT  GetNow_Millis();
    static TimeT  GetNow_Nanos();
    static double GetEllapsed_Seconds();
    static TimeT  GetEllapsed_Millis();
    static TimeT  GetEllapsed_Nanos();
    static void SetDeltaTime(double seconds);
    static void SetDeltaTimeReferenceToNow();

private:
    double m_time = 0.0;

    TimeT m_ellapsedTimeNanos = 0;
    TimeT m_initialTimeNanos  = 0;

    float m_deltaTime               = 0.0f;
    TimeT m_deltaTimeReferenceNanos = 0;

    Time();
    virtual ~Time() = default;

    static Time *GetInstance();

    friend class Application;
};

NAMESPACE_BANG_END

#endif // TIME_H
