#ifndef ANIMATORSTATEMACHINELAYER_H
#define ANIMATORSTATEMACHINELAYER_H

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/MetaNode.h"
#include "Bang/Resource.h"
#include "Bang/String.h"

namespace Bang
{
class AnimatorStateMachine;
class AnimatorStateMachineNode;
class AnimatorStateMachineVariable;
class IEventsAnimatorStateMachineLayer;
class IEventsDestroy;
class Path;

class AnimatorStateMachineLayer
    : public Serializable,
      public EventEmitter<IEventsDestroy>,
      public EventEmitter<IEventsAnimatorStateMachineLayer>
{
    SERIALIZABLE(AnimatorStateMachineLayer)

public:
    AnimatorStateMachineLayer();
    virtual ~AnimatorStateMachineLayer() override;

    AnimatorStateMachineNode *CreateAndAddNode();
    void RemoveNode(AnimatorStateMachineNode *nodeToRemove);
    void SetEntryNode(AnimatorStateMachineNode *entryNode);
    void SetEntryNodeIdx(uint entryNodeIdx);
    void SetLayerName(const String &layerName);
    void SetBoneName(const String &boneName);
    void Clear();

    const String &GetLayerName() const;
    const String &GetBoneName() const;
    AnimatorStateMachineNode *GetNode(uint nodeIdx);
    const AnimatorStateMachineNode *GetNode(uint nodeIdx) const;
    AnimatorStateMachine *GetStateMachine() const;
    AnimatorStateMachineNode *GetEntryNodeOrFirstFound() const;
    AnimatorStateMachineNode *GetEntryNode() const;
    uint GetEntryNodeIdx() const;

    const Array<AnimatorStateMachineNode *> &GetNodes() const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    String m_layerName = "";
    String m_boneName = "";
    RH<AnimatorStateMachine> p_stateMachine;
    uint m_entryNodeIdx = -1u;
    Array<AnimatorStateMachineNode *> m_nodes;

    void SetStateMachine(AnimatorStateMachine *animatorSM);

    friend class AnimatorStateMachine;
};
}

#endif  // ANIMATORSTATEMACHINELAYER_H
