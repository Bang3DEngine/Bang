#ifndef IEVENTLISTENERCOMMON_H
#define IEVENTLISTENERCOMMON_H

#include "Bang/BangDefines.h"

NAMESPACE_BANG_BEGIN

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

NAMESPACE_BANG_END

#endif // IEVENTLISTENERCOMMON_H

