#ifndef CHRONO_H
#define CHRONO_H

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/String.h"
#include "Bang/Time.h"

namespace Bang
{
class Chrono
{
public:
    Chrono(const String &chronoName = "");

    void MarkEvent(const String &eventName);
    void Log();

private:
    struct ChronoEvent
    {
        String eventName = "";
        Time time;
        Time timeSinceLastEvent;
    };

    String m_chronoName = "";
    Array<ChronoEvent> m_events = {};
};
}  // namespace Bang

#endif  // CHRONO_H
