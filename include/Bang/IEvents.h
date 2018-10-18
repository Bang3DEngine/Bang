#ifndef IEVENTS_H
#define IEVENTS_H

#include "Bang/Bang.h"

namespace Bang
{
template <class>
class EventEmitter;
template <class>
class EventListener;
}

#define IEVENTS_COMMON_(ClassName) \
private:                           \
    ClassName() = default;         \
    virtual ~ClassName() = default;

#define IEVENTS(ClassName)     \
    IEVENTS_COMMON_(ClassName) \
    template <class>           \
    friend class Bang::EventListener

#define IEVENTS_NS(ClassName)  \
    IEVENTS_COMMON_(ClassName) \
    template <class>           \
    friend class Bang::EventListener

#endif  // IEVENTS_H
