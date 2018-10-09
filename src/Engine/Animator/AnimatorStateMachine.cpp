#include "Bang/AnimatorStateMachine.h"

#include "Bang/MetaNode.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineConnection.h"

USING_NAMESPACE_BANG

AnimatorStateMachine::AnimatorStateMachine()
{
    AnimatorStateMachineNode *entryNode = CreateAndAddNode();
    entryNode->SetName("Entry");
}

AnimatorStateMachine::~AnimatorStateMachine()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
                                        &IEventsDestroy::OnDestroyed, this);

    while (!m_nodes.IsEmpty())
    {
        delete m_nodes.Back();
        m_nodes.PopBack();
    }
}

AnimatorStateMachineNode *AnimatorStateMachine::CreateAndAddNode()
{
    AnimatorStateMachineNode *newSMNode = new AnimatorStateMachineNode(this);
    m_nodes.PushBack(newSMNode);

    EventEmitter<IEventsAnimatorStateMachine>::PropagateToListeners(
                &IEventsAnimatorStateMachine::OnNodeCreated,
                this,
                m_nodes.Size()-1,
                newSMNode);

    return newSMNode;
}

const AnimatorStateMachineNode *AnimatorStateMachine::GetNode(uint nodeIdx) const
{
    return const_cast<AnimatorStateMachine*>(this)->GetNode(nodeIdx);
}

AnimatorStateMachineNode *AnimatorStateMachine::GetNode(uint nodeIdx)
{
    if (nodeIdx < m_nodes.Size())
    {
        return m_nodes[nodeIdx];
    }
    return nullptr;
}

void AnimatorStateMachine::RemoveNode(AnimatorStateMachineNode *nodeToRemove)
{
    for (AnimatorStateMachineNode *node : m_nodes)
    {
        for (uint i = 0; i < node->GetConnections().Size();)
        {
            AnimatorStateMachineConnection *conn = node->GetConnection(i);
            if (conn->GetNodeTo()   == nodeToRemove ||
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
                &IEventsAnimatorStateMachine::OnNodeRemoved,
                this, idxToRemove, nodeToRemove);
    m_nodes.Remove(nodeToRemove);
}

AnimatorStateMachineVariable* AnimatorStateMachine::AddOrGetVariable(
                                                        const String &varName)
{
    AnimatorStateMachineVariable *var = nullptr;
    if (m_nameToVariable.ContainsKey(varName))
    {
        var = m_nameToVariable.Get(varName);
    }
    else
    {
        AnimatorStateMachineVariable *var = new AnimatorStateMachineVariable();
        m_nameToVariable.Add(varName, var);
    }
    return var;
}

void AnimatorStateMachine::SetVariableFloat(const String &varName,
                                            const float value)
{
    AnimatorStateMachineVariable *var = AddOrGetVariable(varName);
    var->SetType(AnimatorStateMachineVariable::Type::FLOAT);
    var->SetValueFloat(value);
}

void AnimatorStateMachine::SetVariableBool(const String &varName,
                                           const bool value)
{
    AnimatorStateMachineVariable *var = AddOrGetVariable(varName);
    var->SetType(AnimatorStateMachineVariable::Type::BOOL);
    var->SetValueBool(value);
}

float AnimatorStateMachine::GetVariableFloat(const String &varName) const
{
    if (m_nameToVariable.ContainsKey(varName))
    {
        return m_nameToVariable.Get(varName)->GetValueFloat();
    }
    return 0.0f;
}

bool AnimatorStateMachine::GetVariableBool(const String &varName) const
{
    if (m_nameToVariable.ContainsKey(varName))
    {
        return m_nameToVariable.Get(varName)->GetValueBool();
    }
    return false;
}

void AnimatorStateMachine::Clear()
{
    while (!m_nodes.IsEmpty())
    {
        RemoveNode( m_nodes.Back() );
    }
}

AnimatorStateMachineVariable *AnimatorStateMachine::GetVariable(
                                                const String &varName) const
{
    if (m_nameToVariable.ContainsKey(varName))
    {
        return m_nameToVariable.Get(varName);
    }
    return nullptr;
}

const Array<AnimatorStateMachineNode*>& AnimatorStateMachine::GetNodes() const
{
    return m_nodes;
}

Array<AnimatorStateMachineVariable*> AnimatorStateMachine::GetVariables() const
{
    return m_nameToVariable.GetValues();
}

const Map<String, AnimatorStateMachineVariable*>&
      AnimatorStateMachine::GetNameToVariables() const
{
    return m_nameToVariable;
}

void AnimatorStateMachine::Import(const Path &resourceFilepath)
{
    ImportMetaFromFile( MetaFilesManager::GetMetaFilepath(resourceFilepath) );
}

void AnimatorStateMachine::ImportMeta(const MetaNode &metaNode)
{
    Resource::ImportMeta(metaNode);

    if (metaNode.GetChildren().Size() >= 1)
    {
        Clear();

        // First just create the nodes (so that indices work nice)...
        for (uint i = 0; i < metaNode.GetChildren().Size(); ++i)
        {
            CreateAndAddNode();
        }

        // Now import nodes meta
        uint i = 0;
        for (const MetaNode &childMetaNode : metaNode.GetChildren())
        {
            AnimatorStateMachineNode *node = GetNode(i);
            node->ImportMeta(childMetaNode);
            ++i;
        }
    }
}

void AnimatorStateMachine::ExportMeta(MetaNode *metaNode) const
{
    Resource::ExportMeta(metaNode);

    for (const AnimatorStateMachineNode *smNode : GetNodes())
    {
        MetaNode smNodeMeta;
        smNode->ExportMeta(&smNodeMeta);
        metaNode->AddChild(smNodeMeta);
    }
}
