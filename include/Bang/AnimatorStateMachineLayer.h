#ifndef ANIMATORSTATEMACHINELAYER_H
#define ANIMATORSTATEMACHINELAYER_H

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsAnimatorStateMachineLayer.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/MetaNode.h"
#include "Bang/Set.h"
#include "Bang/String.h"

namespace Bang
{
class Animator;
class AnimatorLayerMask;
class AnimatorStateMachine;
class AnimatorStateMachineNode;
class AnimatorStateMachineVariable;
class Path;

using AnimatorBoneMask = Set<String>;

class AnimatorStateMachineLayer
    : public Serializable,
      public EventEmitterIEventsDestroyWithCheck,
      public EventEmitter<IEventsAnimatorStateMachineLayer>
{
    SERIALIZABLE(AnimatorStateMachineLayer)

public:
    AnimatorStateMachineLayer();
    virtual ~AnimatorStateMachineLayer() override;

    void AddNode(AnimatorStateMachineNode *newSMNode);
    void RemoveNode(AnimatorStateMachineNode *nodeToRemove);
    void SetEntryNode(AnimatorStateMachineNode *entryNode);
    void SetEntryNodeIdx(uint entryNodeIdx);
    void SetLayerName(const String &layerName);
    void SetLayerMask(AnimatorLayerMask *layerMask);
    void SetEnabled(bool enabled);
    void Clear();

    bool GetEnabled() const;
    const String &GetLayerName() const;
    AnimatorLayerMask *GetLayerMask() const;
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
    bool m_enabled = true;
    String m_layerName = "";
    AH<AnimatorStateMachine> p_stateMachine;
    uint m_entryNodeIdx = SCAST<uint>(-1);
    AH<AnimatorLayerMask> m_layerMask;
    Array<AnimatorStateMachineNode *> m_nodes;

    void SetStateMachine(AnimatorStateMachine *animatorSM);

    friend class AnimatorStateMachine;
};
}  // namespace Bang

#endif  // ANIMATORSTATEMACHINELAYER_H
