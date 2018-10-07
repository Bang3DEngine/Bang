#ifndef ANIMATORSTATEMACHINENODE_H
#define ANIMATORSTATEMACHINENODE_H

#include "Bang/Bang.h"
#include "Bang/Animation.h"
#include "Bang/ResourceHandle.h"
#include "Bang/AnimatorStateMachineConnection.h"

NAMESPACE_BANG_BEGIN

class AnimatorStateMachineNode
{
public:
	AnimatorStateMachineNode();
	virtual ~AnimatorStateMachineNode();

    void SetName(const String &name);
    AnimatorStateMachineConnection* CreateConnection(uint nodeToIdx);
    AnimatorStateMachineConnection* CreateConnection(
                                            AnimatorStateMachineNode *nodeTo);
    const AnimatorStateMachineConnection* GetConnection(uint connectionIdx) const;
    AnimatorStateMachineConnection* GetConnection(uint connectionIdx);
    void RemoveConnection(uint connectionIdx);

    void SetAnimation(Animation *animation);

    const String &GetName() const;
    Animation* GetAnimation() const;
    AnimatorStateMachine *GetStateMachine() const;
    Array<AnimatorStateMachineConnection*> GetConnectionsTo(
                                      AnimatorStateMachineNode *nodeTo);
    Array<const AnimatorStateMachineConnection*> GetConnectionsTo(
                                      AnimatorStateMachineNode *nodeTo) const;
    const Array<AnimatorStateMachineConnection>& GetConnections() const;

private:
    String m_name = "Node";
    RH<Animation> p_animation;
    AnimatorStateMachine *p_stateMachine = nullptr;
    Array<AnimatorStateMachineConnection> m_connections;

    AnimatorStateMachineConnection* AddConnection(
                        AnimatorStateMachineConnection connection);
    void SetAnimatorStateMachine(AnimatorStateMachine *stateMachine);

    friend class AnimatorStateMachine;
};

NAMESPACE_BANG_END

#endif // ANIMATORSTATEMACHINENODE_H

