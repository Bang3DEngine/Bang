#include "Bang/AnimatorStateMachineNode.h"

#include "Bang/MetaNode.h"
#include "Bang/AnimatorStateMachine.h"

USING_NAMESPACE_BANG

AnimatorStateMachineNode::AnimatorStateMachineNode()
{
    static int i = 0;
    SetName("Node " + String::ToString(i++));
}

AnimatorStateMachineNode::~AnimatorStateMachineNode()
{
}

void AnimatorStateMachineNode::SetName(const String &name)
{
    m_name = name;
}

AnimatorStateMachineConnection*
AnimatorStateMachineNode::CreateConnectionTo(uint nodeToIdx)
{
    if (GetStateMachine())
    {
        return CreateConnectionTo( GetStateMachine()->GetNode(nodeToIdx) );
    }
    return nullptr;
}

AnimatorStateMachineConnection*
AnimatorStateMachineNode::CreateConnectionTo(AnimatorStateMachineNode *nodeTo)
{
    AnimatorStateMachineConnection newConnection;
    newConnection.SetStateMachine( GetStateMachine() );
    newConnection.SetNodeFrom(this);
    newConnection.SetNodeTo(nodeTo);
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
        return &(m_connections[connectionIdx]);
    }
    return nullptr;
}

void AnimatorStateMachineNode::RemoveConnection(uint connectionIdx)
{
    ASSERT(connectionIdx < GetConnections().Size());

    auto removedConnection = &m_connections[connectionIdx];

    EventEmitter<IEventsAnimatorStateMachineNode>::PropagateToListeners(
                &IEventsAnimatorStateMachineNode::OnConnectionRemoved,
                this, removedConnection);

    m_connections.RemoveByIndex(connectionIdx);
}

AnimatorStateMachineConnection *AnimatorStateMachineNode::AddConnection(
                                AnimatorStateMachineConnection newConnection)
{
    newConnection.SetStateMachine( GetStateMachine() );
    ASSERT(newConnection.GetNodeFrom() == this);
    ASSERT(newConnection.GetNodeTo() != nullptr);
    ASSERT(newConnection.GetNodeTo() != this);

    m_connections.PushBack(newConnection);

    AnimatorStateMachineConnection *newConnectionPtr = &m_connections.Back();

    EventEmitter<IEventsAnimatorStateMachineNode>::PropagateToListeners(
                &IEventsAnimatorStateMachineNode::OnConnectionAdded,
                this, newConnectionPtr);

    return newConnectionPtr;
}

void AnimatorStateMachineNode::SetAnimatorStateMachine(
                                    AnimatorStateMachine *stateMachine)
{
    p_stateMachine = stateMachine;
}

void AnimatorStateMachineNode::SetAnimation(Animation *animation)
{
    p_animation.Set( animation );
}

const String &AnimatorStateMachineNode::GetName() const
{
    return m_name;
}

Array<AnimatorStateMachineConnection*>
AnimatorStateMachineNode::GetConnectionsTo(AnimatorStateMachineNode *nodeTo)
{
    Array<AnimatorStateMachineConnection*> connectionsToNode;
    for (AnimatorStateMachineConnection connection : GetConnections())
    {
        if (connection.GetNodeTo() == nodeTo)
        {
            connectionsToNode.PushBack(&connection);
        }
    }
    return connectionsToNode;
}
Array<const AnimatorStateMachineConnection*>
AnimatorStateMachineNode::GetConnectionsTo(AnimatorStateMachineNode *nodeTo) const
{
    Array<AnimatorStateMachineConnection*> connectionsTo =
        const_cast<AnimatorStateMachineNode*>(this)->GetConnectionsTo(nodeTo);

    Array<const AnimatorStateMachineConnection*> connectionsToConst;
    for (AnimatorStateMachineConnection *conn : connectionsTo)
    {
        connectionsToConst.PushBack(conn);
    }

    return connectionsToConst;
}


Animation *AnimatorStateMachineNode::GetAnimation() const
{
    return p_animation.Get();
}

AnimatorStateMachine *AnimatorStateMachineNode::GetStateMachine() const
{
    return p_stateMachine;
}

const Array<AnimatorStateMachineConnection>&
AnimatorStateMachineNode::GetConnections() const
{
    return m_connections;
}

void AnimatorStateMachineNode::CloneInto(
                            AnimatorStateMachineNode *nodeToCloneTo) const
{
    nodeToCloneTo->SetName( GetName() );
    nodeToCloneTo->SetAnimation( GetAnimation() );
    // for (const AnimatorStateMachineConnection &conn : GetConnections())
    // {
    //     AnimatorStateMachineNode *connNodeFrom = conn.GetNodeFrom();
    //     AnimatorStateMachineNode *connNodeTo   = conn.GetNodeTo();
    //     connNodeFrom = (connNodeFrom == this ? nodeToCloneTo : connNodeFrom);
    //     connNodeTo   = (connNodeTo   == this ? nodeToCloneTo : connNodeTo);
    //
    //     AnimatorStateMachineConnection *newConn =
    //                 connNodeFrom->CreateConnectionTo( connNodeTo );
    //     BANG_UNUSED(newConn);
    // }
}

void AnimatorStateMachineNode::ImportMeta(const MetaNode &metaNode)
{
    Serializable::ImportMeta(metaNode);

    if (metaNode.Contains("NodeName"))
    {
        SetName( metaNode.Get<String>("NodeName") );
    }

    for (const MetaNode &childMetaNode : metaNode.GetChildren())
    {
        AnimatorStateMachineConnection newConnection;
        newConnection.ImportMeta(childMetaNode);
        AddConnection(newConnection);
    }
}

void AnimatorStateMachineNode::ExportMeta(MetaNode *metaNode) const
{
    Serializable::ExportMeta(metaNode);

    metaNode->Set("NodeName", GetName());
    for (const AnimatorStateMachineConnection &smConn : GetConnections())
    {
        MetaNode smConnMeta;
        smConn.ExportMeta(&smConnMeta);
        metaNode->AddChild(smConnMeta);
    }
}

