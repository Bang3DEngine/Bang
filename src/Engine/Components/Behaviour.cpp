#include "Bang/Behaviour.h"

#include "Bang/File.h"
#include "Bang/Time.h"
#include "Bang/Extensions.h"
#include "Bang/Application.h"
#include "Bang/BehaviourManager.h"
#include "Bang/BPReflectedStruct.h"

USING_NAMESPACE_BANG

Behaviour::Behaviour()
{
    CONSTRUCT_CLASS_ID(Behaviour)
    BehaviourManager::GetActive()->RegisterBehaviour(this);
}

Behaviour::~Behaviour()
{
}

const BPReflectedStruct& Behaviour::GetReflectionInfo() const
{
    return m_reflectionInfo;
}
