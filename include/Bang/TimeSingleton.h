#ifndef TIMESINGLETON_H
#define TIMESINGLETON_H

#include "Bang/BangDefines.h"
#include "Bang/Time.h"

namespace Bang
{
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
}

#endif  // TIMESINGLETON_H
