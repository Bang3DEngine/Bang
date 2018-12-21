#include "Bang/Behaviour.h"

#include "Bang/BehaviourManager.h"
#include "Bang/ClassDB.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsGameObjectPhysics.h"

using namespace Bang;

Behaviour::Behaviour()
{
    SET_INSTANCE_CLASS_ID(Behaviour)
    BehaviourManager::GetActive()->RegisterBehaviour(this);
}

Behaviour::~Behaviour()
{
}
