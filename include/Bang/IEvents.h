#ifndef IEVENTS_H
#define IEVENTS_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

#define _IEVENTS_COMMON(ClassName) \
    private: \
        ClassName() = default; \
        virtual ~ClassName() = default; \

#define IEVENTS(ClassName) \
    _IEVENTS_COMMON(ClassName) \
    template<class> friend class EventListener;

#define IEVENTS_NS(ClassName) \
    _IEVENTS_COMMON(ClassName) \
    template<class> friend class Bang::EventListener;

FORWARD template<class> class EventEmitter;

NAMESPACE_BANG_END

#endif // IEVENTS_H
