#ifndef COMPONENTMACROS_H
#define COMPONENTMACROS_H

#include "Bang/Object.h"
#include "Bang/IToString.h"
#include "Bang/RenderPass.h"
#include "Bang/Serializable.h"
#include "Bang/EventEmitter.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/ComponentFactory.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/IEventsComponentChangeGameObject.h"

NAMESPACE_BANG_BEGIN

#define REQUIRE_COMPONENT(gameObject, CLASS) \
    ASSERT(gameObject->HasComponent<CLASS>())

#define CLONEABLE_COMPONENT(CLASS) \
    public: \
    virtual CLASS *Clone() const override \
    { \
        CLASS *clone = Component::Create<CLASS>(); \
        CloneInto(clone); \
        return clone; \
    } \

#define COMPONENT_ABSTRACT(CLASS) \
    SERIALIZABLE(CLASS) \
    friend class Component;

#define COMPONENT(CLASS) \
    CLONEABLE_COMPONENT(CLASS) \
    COMPONENT_ABSTRACT(CLASS)

#define COMPONENT_WITH_FAST_DYNAMIC_CAST_EXPLICIT_ABSTRACT(CLASS, CIDBegin, CIDEnd) \
    COMPONENT_ABSTRACT(CLASS) \
    SET_CLASS_ID(CLASS, CIDBegin, CIDEnd)

#define COMPONENT_WITH_FAST_DYNAMIC_CAST_EXPLICIT(CLASS, CIDBegin, CIDEnd) \
    COMPONENT(CLASS) \
    SET_CLASS_ID(CLASS, CIDBegin, CIDEnd)


#define COMPONENT_WITH_FAST_DYNAMIC_CAST_ABSTRACT(CLASS) \
    COMPONENT_WITH_FAST_DYNAMIC_CAST_EXPLICIT_ABSTRACT(CLASS, CLASS##CIDBegin, CLASS##CIDEnd)

#define COMPONENT_WITH_FAST_DYNAMIC_CAST(CLASS) \
    COMPONENT_WITH_FAST_DYNAMIC_CAST_EXPLICIT(CLASS, CLASS##CIDBegin, CLASS##CIDEnd)

NAMESPACE_BANG_END

#endif // COMPONENTMACROS_H
