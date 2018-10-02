#ifndef TIMESINGLETON_H
#define TIMESINGLETON_H

#include "Bang/Bang.h"
#include "Bang/Time.h"

NAMESPACE_BANG_BEGIN

class TimeSingleton
{
public:
    TimeSingleton();

private:
    Time m_initialTime;

    static Time GetNow();
    static Time GetInit();
    static Time GetEllapsed();

    static void SetInitTime(Time time);

    static TimeSingleton *GetInstance();

    friend class Time;
    friend class Application;
};

NAMESPACE_BANG_END

#endif // TIMESINGLETON_H

