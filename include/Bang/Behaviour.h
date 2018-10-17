#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BPReflectedStruct.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsGameObjectPhysics.h"
#include "Bang/String.h"

using namespace Bang;
namespace Bang
{
class IEventsGameObjectPhysics;

class Behaviour : public Component,
                  public EventListener<IEventsGameObjectPhysics>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(Behaviour)

public:
    Behaviour();
    virtual ~Behaviour() override;

    virtual const BPReflectedStruct &GetReflectionInfo() const;

public:
    mutable BPReflectedStruct m_reflectionInfo;
};

// DEFINES
#define BANG_BEHAVIOUR_CLASS(CLASS_NAME)                  \
    extern "C" Behaviour *CreateDynamically_##CLASS_NAME( \
        Application *mainBinaryApplication);              \
    extern "C" void DeleteDynamically_##CLASS_NAME(Behaviour *b);

#define BANG_BEHAVIOUR_CLASS_IMPL(CLASS_NAME)                        \
    extern "C" Behaviour *CreateDynamically_##CLASS_NAME(            \
        Application *mainBinaryApplication)                          \
    {                                                                \
        /* This line links the Application in the main binary        \
            to the Application in the behaviour loaded library. */   \
        Application::SetApplicationSingleton(mainBinaryApplication); \
        return Component::Create<CLASS_NAME>();                      \
    }                                                                \
                                                                     \
    extern "C" void DeleteDynamically_##CLASS_NAME(Behaviour *b)     \
    {                                                                \
        delete b;                                                    \
    }
}

#endif
