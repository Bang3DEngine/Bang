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

#define COMPONENT_ABSTRACT_(CLASS) \
    OBJECT_ABSTRACT(CLASS)         \
    friend class Bang::GameObject;

#define COMPONENT_ABSTRACT(CLASS) \
    OBJECT_ABSTRACT(CLASS)        \
    friend class Bang::Component; \
    friend class Bang::GameObject;

#define COMPONENT_NO_FRIEND(CLASS) \
    OBJECT(CLASS)                  \
    friend class Bang::GameObject;

#define COMPONENT(CLASS)       \
    COMPONENT_NO_FRIEND(CLASS) \
    friend class Bang::Component;

#define COMPONENT_WITHOUT_CLASS_ID(CLASS) \
    OBJECT_WITHOUT_CLASS_ID(CLASS)        \
    friend class Bang::Component;         \
    friend class Bang::GameObject;
}

#endif  // COMPONENTMACROS_H
