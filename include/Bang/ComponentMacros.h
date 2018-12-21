#ifndef COMPONENTMACROS_H
#define COMPONENTMACROS_H

#include "Bang/ClassDB.h"
#include "Bang/ClassDB.h"
#include "Bang/EventEmitter.h"
#include "Bang/IEventsComponentChangeGameObject.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IToString.h"
#include "Bang/Object.h"
#include "Bang/RenderPass.h"
#include "Bang/Serializable.h"

namespace Bang
{
#define REQUIRE_COMPONENT(gameObject, CLASS) \
    ASSERT(gameObject->HasComponent<CLASS>())

#define COMPONENT_ABSTRACT(CLASS)  \
    OBJECT_WITHOUT_CLASS_ID(CLASS) \
    SERIALIZABLE(CLASS)            \
    friend class Bang::Component;  \
    friend class Bang::GameObject;

#define COMPONENT(CLASS) \
    ICLONEABLE(CLASS)    \
    SET_CLASS_ID(CLASS)  \
    COMPONENT_ABSTRACT(CLASS)

#define COMPONENT_WITHOUT_CLASS_ID(CLASS) \
    ICLONEABLE(CLASS)                     \
    COMPONENT_ABSTRACT(CLASS)
}

#endif  // COMPONENTMACROS_H
