#include "Bang/AnimatorStateMachine.h"

#include <sys/types.h>

#include "Bang/AnimatorStateMachineConnection.h"
#include "Bang/AnimatorStateMachineConnectionTransitionCondition.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineVariable.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Path.h"

using namespace Bang;

AnimatorStateMachine::AnimatorStateMachine()
{
    AnimatorStateMachineNode *entryNode = CreateAndAddNode();
    entryNode->SetName("Entry");

    EventEmitter<IEventsAnimatorStateMachine>::RegisterListener(this);
}

AnimatorStateMachine::~AnimatorStateMachine()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
        &IEventsDestroy::OnDestroyed, this);
    Clear();
}

AnimatorStateMachineNode *AnimatorStateMachine::CreateAndAddNode()
{
    AnimatorStateMachineNode *newSMNode = new AnimatorStateMachineNode(this);
    m_nodes.PushBack(newSMNode);

    if(!GetEntryNode())
    {
        SetEntryNode(newSMNode);
    }

    EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
        &IEventsAnimatorStateMachine::OnNodeCreated, this, m_nodes.Size() - 1,
        newSMNode);

    return newSMNode;
}

const AnimatorStateMachineNode *AnimatorStateMachine::GetNode(
    uint nodeIdx) const
{
    return const_cast<AnimatorStateMachine *>(this)->GetNode(nodeIdx);
}

AnimatorStateMachineNode *AnimatorStateMachine::GetNode(uint nodeIdx)
{
    if(nodeIdx < m_nodes.Size())
    {
        return m_nodes[nodeIdx];
    }
    return nullptr;
}

void AnimatorStateMachine::RemoveNode(AnimatorStateMachineNode *nodeToRemove)
{
    for(AnimatorStateMachineNode *node : m_nodes)
    {
        for(uint i = 0; i < node->GetConnections().Size();)
        {
            AnimatorStateMachineConnection *conn = node->GetConnection(i);
            if(conn->GetNodeTo() == nodeToRemove ||
               conn->GetNodeFrom() == nodeToRemove)
            {
                node->RemoveConnection(conn);
            }
            else
            {
                ++i;
            }
        }
    }

    const uint idxToRemove = GetNodes().IndexOf(nodeToRemove);

    EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
        &IEventsAnimatorStateMachine::OnNodeRemoved, this, idxToRemove,
        nodeToRemove);

    m_nodes.Remove(nodeToRemove);
    delete nodeToRemove;

    if(idxToRemove == GetEntryNodeIdx())
    {
        SetEntryNodeIdx(0);
    }
}

void AnimatorStateMachine::SetEntryNode(AnimatorStateMachineNode *entryNode)
{
    SetEntryNodeIdx(GetNodes().IndexOf(entryNode));
}

void AnimatorStateMachine::SetEntryNodeIdx(uint entryNodeIdx)
{
    if(entryNodeIdx < GetNodes().Size())
    {
        m_entryNodeIdx = entryNodeIdx;
    }
    else
    {
        m_entryNodeIdx = -1u;
    }
}

AnimatorStateMachineVariable *AnimatorStateMachine::CreateNewVariable()
{
    AnimatorStateMachineVariable *var = new AnimatorStateMachineVariable();
    var->p_animatorSM = this;

    String varName = "NewVariable";
    varName = Path::GetDuplicateString(varName, GetVariablesNames());
    var->SetName(varName);

    m_variables.PushBack(var);

    return var;
}

AnimatorStateMachineVariable *AnimatorStateMachine::CreateOrGetVariable(
    const String &varName)
{
    AnimatorStateMachineVariable *var = GetVariable(varName);
    if(!var)
    {
        var = CreateNewVariable();
        var->SetName(varName);
    }
    return var;
}

void AnimatorStateMachine::OnVariableNameChanged(
    AnimatorStateMachineVariable *variable,
    const String &prevVariableName,
    const String &nextVariableName)
{
    BANG_UNUSED(variable);
    for(AnimatorStateMachineNode *node : GetNodes())
    {
        for(AnimatorStateMachineConnection *conn : node->GetConnections())
        {
            for(auto transCond : conn->GetTransitionConditions())
            {
                if(transCond->GetVariableName() == prevVariableName)
                {
                    transCond->SetVariableName(nextVariableName);
                }
            }
        }
    }
}

void AnimatorStateMachine::SetVariableFloat(const String &varName,
                                            const float value)
{
    AnimatorStateMachineVariable *var = CreateOrGetVariable(varName);
    var->SetType(AnimatorStateMachineVariable::Type::FLOAT);
    var->SetValueFloat(value);
}

void AnimatorStateMachine::SetVariableBool(const String &varName,
                                           const bool value)
{
    AnimatorStateMachineVariable *var = CreateOrGetVariable(varName);
    var->SetType(AnimatorStateMachineVariable::Type::BOOL);
    var->SetValueBool(value);
}

void AnimatorStateMachine::RemoveVariable(AnimatorStateMachineVariable *var)
{
    RemoveVariable(m_variables.IndexOf(var));
}

void AnimatorStateMachine::RemoveVariable(uint varIdx)
{
    if(varIdx < m_variables.Size())
    {
        delete m_variables[varIdx];
        m_variables.RemoveByIndex(varIdx);
    }
}

float AnimatorStateMachine::GetVariableFloat(const String &varName) const
{
    if(AnimatorStateMachineVariable *var = GetVariable(varName))
    {
        return var->GetValueBool();
    }
    return 0.0f;
}

bool AnimatorStateMachine::GetVariableBool(const String &varName) const
{
    if(AnimatorStateMachineVariable *var = GetVariable(varName))
    {
        return var->GetValueBool();
    }
    return false;
}

AnimatorStateMachineNode *AnimatorStateMachine::GetEntryNode() const
{
    if(GetEntryNodeIdx() < GetNodes().Size())
    {
        return GetNodes()[GetEntryNodeIdx()];
    }
    return nullptr;
}

AnimatorStateMachineNode *AnimatorStateMachine::GetEntryNodeOrFirstFound() const
{
    if(AnimatorStateMachineNode *entryNode = GetEntryNode())
    {
        return entryNode;
    }
    return ((m_nodes.Size() >= 1) ? m_nodes.Front() : nullptr);
}

uint AnimatorStateMachine::GetEntryNodeIdx() const
{
    return m_entryNodeIdx;
}

void AnimatorStateMachine::Clear()
{
    while(!m_nodes.IsEmpty())
    {
        RemoveNode(m_nodes.Back());
    }

    while(!m_variables.IsEmpty())
    {
        RemoveVariable(m_variables.Size() - 1);
    }
}

AnimatorStateMachineVariable *AnimatorStateMachine::GetVariable(
    const String &varName) const
{
    for(AnimatorStateMachineVariable *var : m_variables)
    {
        if(var->GetName() == varName)
        {
            return var;
        }
    }
    return nullptr;
}

const Array<AnimatorStateMachineNode *> &AnimatorStateMachine::GetNodes() const
{
    return m_nodes;
}

const Array<AnimatorStateMachineVariable *>
    &AnimatorStateMachine::GetVariables() const
{
    return m_variables;
}

Array<String> AnimatorStateMachine::GetVariablesNames() const
{
    Array<String> varNames;
    for(AnimatorStateMachineVariable *var : m_variables)
    {
        varNames.PushBack(var->GetName());
    }
    return varNames;
}

void AnimatorStateMachine::Import(const Path &resourceFilepath)
{
    ImportMetaFromFile(MetaFilesManager::GetMetaFilepath(resourceFilepath));
}

void AnimatorStateMachine::ImportMeta(const MetaNode &metaNode)
{
    Resource::ImportMeta(metaNode);

    if(metaNode.Contains("EntryNodeIdx"))
    {
        SetEntryNodeIdx(metaNode.Get<uint>("EntryNodeIdx"));
    }

    if(metaNode.GetChildren("Nodes").Size() >= 1)
    {
        Clear();

        // First just create the nodes (so that indices work nice)...
        {
            const auto &childrenMetaNodes = metaNode.GetChildren("Nodes");
            for(uint i = 0; i < childrenMetaNodes.Size(); ++i)
            {
                CreateAndAddNode();
            }

            // Now import nodes meta
            uint i = 0;
            for(const MetaNode &childMetaNode : childrenMetaNodes)
            {
                AnimatorStateMachineNode *node = GetNode(i);
                node->ImportMeta(childMetaNode);
                ++i;
            }
        }

        const auto &varsMetaNodes = metaNode.GetChildren("Variables");
        for(const MetaNode &varMetaNode : varsMetaNodes)
        {
            AnimatorStateMachineVariable *var = CreateNewVariable();
            var->ImportMeta(varMetaNode);
        }
    }
}

void AnimatorStateMachine::ExportMeta(MetaNode *metaNode) const
{
    Resource::ExportMeta(metaNode);

    metaNode->Set("EntryNodeIdx", GetEntryNodeIdx());

    for(const AnimatorStateMachineNode *smNode : GetNodes())
    {
        MetaNode smNodeMeta = smNode->GetMeta();
        metaNode->AddChild(smNodeMeta, "Nodes");
    }

    for(const AnimatorStateMachineVariable *var : GetVariables())
    {
        MetaNode varMeta = var->GetMeta();
        metaNode->AddChild(varMeta, "Variables");
    }
}
