#include "Bang/AnimatorStateMachineNode.h"

#include "Bang/MetaNode.h"
#include "Bang/Resources.h"
#include "Bang/AnimatorStateMachine.h"

USING_NAMESPACE_BANG

AnimatorStateMachineNode::AnimatorStateMachineNode(
                                    AnimatorStateMachine *stateMachine)
{
    p_stateMachine = stateMachine;
    SetName("Node");
}

AnimatorStateMachineNode::~AnimatorStateMachineNode()
{
    EventEmitter<IEventsDestroy>::PropagateToListeners(
                                        &IEventsDestroy::OnDestroyed, this);
    while (!m_connections.IsEmpty())
    {
        RemoveConnection( m_connections.Back() );
    }
}

void AnimatorStateMachineNode::SetName(const String &name)
{
    m_name = name;
}

AnimatorStateMachineConnection*
AnimatorStateMachineNode::CreateConnectionTo(AnimatorStateMachineNode *nodeTo)
{
    auto newConnection = new AnimatorStateMachineConnection(p_stateMachine);
    newConnection->SetNodeFrom(this);
    newConnection->SetNodeTo(nodeTo);
    return AddConnection(newConnection);
}

const AnimatorStateMachineConnection *AnimatorStateMachineNode::GetConnection(
                                                    uint connectionIdx) const
{
    return const_cast<AnimatorStateMachineNode*>(this)->
           GetConnection(connectionIdx);
}

AnimatorStateMachineConnection *AnimatorStateMachineNode::GetConnection(
                                                    uint connectionIdx)
{
    if (connectionIdx < GetConnections().Size())
    {
        return m_connections[connectionIdx];
    }
    return nullptr;
}

void AnimatorStateMachineNode::RemoveConnection(
                            AnimatorStateMachineConnection *connectionToRemove)
{
    if (m_connections.Contains(connectionToRemove))
    {
        m_connections.Remove(connectionToRemove);

        EventEmitter<IEventsAnimatorStateMachineNode>::PropagateToListeners(
                    &IEventsAnimatorStateMachineNode::OnConnectionRemoved,
                    this, connectionToRemove);

        delete connectionToRemove;
    }
}

AnimatorStateMachineConnection *AnimatorStateMachineNode::AddConnection(
                                AnimatorStateMachineConnection *newConnection)
{
    ASSERT(newConnection->GetNodeFrom() == this);
    ASSERT(newConnection->GetNodeTo() != nullptr);
    ASSERT(newConnection->GetNodeTo() != this);

    m_connections.PushBack(newConnection);
    EventEmitter<IEventsAnimatorStateMachineNode>::PropagateToListeners(
                &IEventsAnimatorStateMachineNode::OnConnectionAdded,
                this, newConnection);

    return newConnection;
}

void AnimatorStateMachineNode::SetAnimation(Animation *animation)
{
    p_animation.Set( animation );
}

const String &AnimatorStateMachineNode::GetName() const
{
    return m_name;
}

Animation *AnimatorStateMachineNode::GetAnimation() const
{
    return p_animation.Get();
}

Array<AnimatorStateMachineConnection*>
AnimatorStateMachineNode::GetConnectionsTo(AnimatorStateMachineNode *nodeTo) const
{
    Array<AnimatorStateMachineConnection*> connectionsToNode;
    for (AnimatorStateMachineConnection *connection : GetConnections())
    {
        if (connection->GetNodeTo() == nodeTo)
        {
            connectionsToNode.PushBack(connection);
        }
    }
    return connectionsToNode;
}


const Array<AnimatorStateMachineConnection*>&
AnimatorStateMachineNode::GetConnections() const
{
    return m_connections;
}

void AnimatorStateMachineNode::CloneInto(
                            AnimatorStateMachineNode *nodeToCloneTo) const
{
    nodeToCloneTo->SetName( GetName() );
    nodeToCloneTo->SetAnimation( GetAnimation() );
}

void AnimatorStateMachineNode::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (metaNode.Contains("NodeName"))
    {
        SetName( metaNode.Get<String>("NodeName") );
    }

    if (metaNode.Contains("Animation"))
    {
        SetAnimation( Resources::Load<Animation>(
                            metaNode.Get<GUID>("Animation")).Get() );
    }

    for (const MetaNode &childMetaNode : metaNode.GetChildren("Connections"))
    {
        auto newConnection = new AnimatorStateMachineConnection(p_stateMachine);
        newConnection->ImportMeta(childMetaNode);
        AddConnection(newConnection);
    }
}

void AnimatorStateMachineNode::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("NodeName", GetName());
    metaNode->Set("Animation", GetAnimation() ? GetAnimation()->GetGUID() :
                                                GUID::Empty());
    for (const AnimatorStateMachineConnection *smConn : GetConnections())
    {
        MetaNode smConnMeta = smConn->GetMeta();
        metaNode->AddChild(smConnMeta, "Connections");
    }
}

