#include "Bang/Behaviour.h"

#include "Bang/BehaviourManager.h"
#include "Bang/EventListener.tcc"
#include "Bang/FastDynamicCast.h"
#include "Bang/IEventsGameObjectPhysics.h"

using namespace Bang;

Behaviour::Behaviour()
{
    CONSTRUCT_CLASS_ID(Behaviour)
    BehaviourManager::GetActive()->RegisterBehaviour(this);
}

Behaviour::~Behaviour()
{
}
