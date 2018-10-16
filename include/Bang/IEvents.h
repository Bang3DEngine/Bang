#ifndef IEVENTS_H
#define IEVENTS_H

#include "Bang/Bang.h"

FORWARD NAMESPACE_BANG_BEGIN
FORWARD_T class EventEmitter;
FORWARD_T class EventListener;
FORWARD NAMESPACE_BANG_END

#define IEVENTS_COMMON_(ClassName) \
    private: \
        ClassName() = default; \
        virtual ~ClassName() = default; \

#define IEVENTS(ClassName) \
    IEVENTS_COMMON_(ClassName) \
    template<class> friend class EventListener

#define IEVENTS_NS(ClassName) \
    IEVENTS_COMMON_(ClassName) \
    template<class> friend class Bang::EventListener

#endif // IEVENTS_H
