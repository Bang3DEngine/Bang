#ifndef ANIMATORSTATEMACHINENODE_H
#define ANIMATORSTATEMACHINENODE_H

#include "Bang/Bang.h"
#include "Bang/Animation.h"
#include "Bang/Serializable.h"
#include "Bang/ResourceHandle.h"
#include "Bang/AnimatorStateMachineConnection.h"
#include "Bang/IEventsAnimatorStateMachineNode.h"

NAMESPACE_BANG_BEGIN

class AnimatorStateMachineNode :
                public Serializable,
                public EventEmitter<IEventsDestroy>,
                public EventEmitter<IEventsAnimatorStateMachineNode>
{
    SERIALIZABLE(AnimatorStateMachineNode);

public:
    AnimatorStateMachineNode(AnimatorStateMachine *stateMachine);
	virtual ~AnimatorStateMachineNode();

    void SetName(const String &name);
    AnimatorStateMachineConnection* CreateConnectionTo(
                                            AnimatorStateMachineNode *nodeTo);
    const AnimatorStateMachineConnection* GetConnection(uint connectionIdx) const;
    AnimatorStateMachineConnection* GetConnection(uint connectionIdx);
    void RemoveConnection(AnimatorStateMachineConnection *connection);

    void SetAnimation(Animation *animation);

    const String &GetName() const;
    Animation* GetAnimation() const;
    Array<AnimatorStateMachineConnection*> GetConnectionsTo(
                                      AnimatorStateMachineNode *nodeTo);
    Array<const AnimatorStateMachineConnection*> GetConnectionsTo(
                                      AnimatorStateMachineNode *nodeTo) const;
    const Array<AnimatorStateMachineConnection*>& GetConnections() const;

    void CloneInto(AnimatorStateMachineNode *clone) const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    String m_name = "Node";
    RH<Animation> p_animation;
    DPtr<AnimatorStateMachine> p_stateMachine;
    Array<AnimatorStateMachineConnection*> m_connections;

    AnimatorStateMachineConnection* AddConnection(
                        AnimatorStateMachineConnection *connection);
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINENODE_H

