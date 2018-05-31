#ifndef IEVENTEMITTER_H
#define IEVENTEMITTER_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD template<class T> class EventEmitter;

class IEventEmitter
{
public:
    void SetEmitEvents(bool emitEvents);
    bool IsEmittingEvents() const;

private:
    bool m_emitEvents = true;

    IEventEmitter() = default;
    virtual ~IEventEmitter() = default;

    template<class> friend class EventEmitter;
};

NAMESPACE_BANG_END

#endif // IEVENTEMITTER_H
