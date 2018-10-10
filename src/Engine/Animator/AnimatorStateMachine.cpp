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
    Clear();
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
    delete nodeToRemove;
}

AnimatorStateMachineVariable *AnimatorStateMachine::CreateNewVariable()
{
    AnimatorStateMachineVariable *var = new AnimatorStateMachineVariable();

    String varName = "NewVariable";
    varName = Path::GetDuplicateString(varName, GetVariablesNames());
    var->SetName(varName);

    m_variables.PushBack(var);

    return var;
}

AnimatorStateMachineVariable* AnimatorStateMachine::CreateOrGetVariable(
                                                        const String &varName)
{
    AnimatorStateMachineVariable *var = GetVariable(varName);
    if (!var)
    {
        var = CreateNewVariable();
        var->SetName(varName);
    }
    return var;
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
    RemoveVariable( m_variables.IndexOf(var) );
}

void AnimatorStateMachine::RemoveVariable(uint varIdx)
{
    if (varIdx < m_variables.Size())
    {
        delete m_variables[varIdx];
        m_variables.RemoveByIndex(varIdx);
    }
}

float AnimatorStateMachine::GetVariableFloat(const String &varName) const
{
    if (AnimatorStateMachineVariable *var = GetVariable(varName))
    {
        return var->GetValueBool();
    }
    return 0.0f;
}

bool AnimatorStateMachine::GetVariableBool(const String &varName) const
{
    if (AnimatorStateMachineVariable *var = GetVariable(varName))
    {
        return var->GetValueBool();
    }
    return false;
}

void AnimatorStateMachine::Clear()
{
    while (!m_nodes.IsEmpty())
    {
        RemoveNode( m_nodes.Back() );
    }

    while (!m_variables.IsEmpty())
    {
        RemoveVariable( m_variables.Size() - 1 );
    }
}

AnimatorStateMachineVariable *AnimatorStateMachine::GetVariable(
                                                const String &varName) const
{
    for (AnimatorStateMachineVariable *var : m_variables)
    {
        if (var->GetName() == varName)
        {
            return var;
        }
    }
    return nullptr;
}

const Array<AnimatorStateMachineNode*>& AnimatorStateMachine::GetNodes() const
{
    return m_nodes;
}

const Array<AnimatorStateMachineVariable*>& AnimatorStateMachine::GetVariables() const
{
    return m_variables;
}

Array<String> AnimatorStateMachine::GetVariablesNames() const
{
    Array<String> varNames;
    for (AnimatorStateMachineVariable *var : m_variables)
    {
        varNames.PushBack(var->GetName());
    }
    return varNames;
}

void AnimatorStateMachine::Import(const Path &resourceFilepath)
{
    ImportMetaFromFile( MetaFilesManager::GetMetaFilepath(resourceFilepath) );
}

void AnimatorStateMachine::ImportMeta(const MetaNode &metaNode)
{
    Resource::ImportMeta(metaNode);

    if (metaNode.GetChildren("Children").Size() >= 1)
    {
        Clear();

        // First just create the nodes (so that indices work nice)...
        for (uint i = 0; i < metaNode.GetChildren("Children").Size(); ++i)
        {
            CreateAndAddNode();
        }

        // Now import nodes meta
        uint i = 0;
        for (const MetaNode &childMetaNode : metaNode.GetChildren("Children"))
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
        metaNode->AddChild(smNodeMeta, "Children");
    }
}
