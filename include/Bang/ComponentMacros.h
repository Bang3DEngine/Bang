#ifndef COMPONENTMACROS_H
#define COMPONENTMACROS_H

#include "Bang/ComponentFactory.h"
#include "Bang/EventEmitter.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/IEventsComponentChangeGameObject.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IToString.h"
#include "Bang/Object.h"
#include "Bang/ObjectClassIds.h"
#include "Bang/RenderPass.h"
#include "Bang/Serializable.h"

namespace Bang
{
#define REQUIRE_COMPONENT(gameObject, CLASS) \
    ASSERT(gameObject->HasComponent<CLASS>())

#define CLONEABLE_COMPONENT(CLASS)        \
public:                                   \
    virtual CLASS *Clone() const override \
    {                                     \
        CLASS *clone = new CLASS();       \
        CloneInto(clone);                 \
        return clone;                     \
    }

#define COMPONENT_ABSTRACT(CLASS)        \
    SERIALIZABLE(CLASS)                  \
    friend class Bang::Component;        \
    friend class Bang::ComponentFactory; \
    friend class Bang::GameObject;

#define COMPONENT(CLASS)       \
    CLONEABLE_COMPONENT(CLASS) \
    COMPONENT_ABSTRACT(CLASS)  \
    SET_CLASS_NOT_FAST_DYNAMIC_CASTABLE(CLASS)

#define COMPONENT_WITH_FAST_DYNAMIC_CAST_ABSTRACT(CLASS) \
    COMPONENT_ABSTRACT(CLASS)                            \
    OBJECT_WITH_FAST_DYNAMIC_CAST(CLASS)

#define COMPONENT_WITH_FAST_DYNAMIC_CAST(CLASS) \
    CLONEABLE_COMPONENT(CLASS)                  \
    COMPONENT_WITH_FAST_DYNAMIC_CAST_ABSTRACT(CLASS)
}

#endif  // COMPONENTMACROS_H
