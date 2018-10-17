#ifndef IEVENTLISTENERCOMMON_H
#define IEVENTLISTENERCOMMON_H

#include "Bang/BangDefines.h"

namespace Bang
{
class IEventListenerCommon
{
protected:
    IEventListenerCommon() = default;
    virtual ~IEventListenerCommon() = default;

public:
    void SetReceiveEventsCommon(bool receiveEvents);
    bool IsReceivingEventsCommon() const;

private:
    bool m_isReceivingEventsCommon = true;
};
}

#endif  // IEVENTLISTENERCOMMON_H
