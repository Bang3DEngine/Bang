#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Bang/AnimatorStateMachineVariable.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/IEventsAnimatorStateMachine.h"
#include "Bang/Map.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "Bang/Time.h"
#include "Bang/Transformation.h"

namespace Bang
{
class Animation;
class AnimatorStateMachine;
class AnimatorStateMachinePlayer;
class ICloneable;

class Animator : public Component,
                 public EventListener<IEventsAnimatorStateMachine>
{
    COMPONENT(Animator)

public:
    static constexpr int MaxNumBones = 128;

    Animator();
    virtual ~Animator() override;

    // Component
    void OnStart() override;
    void OnUpdate() override;

    void SetStateMachine(AnimatorStateMachine *stateMachine);
    void SetPlayOnStart(bool playOnStart);
    void SetVariableVariant(const String &varName,
                            const Variant &variableVariant);
    void SetVariableFloat(const String &varName, float value);
    void SetVariableBool(const String &varName, bool value);
    void SetVariableTrigger(const String &varName, bool value);

    void Play();
    void Stop();
    void Pause();

    bool IsPlaying() const;
    bool GetPlayOnStart() const;
    Variant GetVariableVariant(const String &varName) const;
    AnimatorStateMachineVariable::Type GetVariableType(
        const String &varName) const;
    float GetVariableFloat(const String &varName) const;
    bool GetVariableBool(const String &varName) const;
    bool GetVariableTrigger(const String &varName) const;
    AnimatorStateMachine *GetStateMachine() const;
    const Array<AnimatorStateMachinePlayer *> &GetPlayers() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Time m_animationTime;
    Time m_prevFrameTime;

    AH<AnimatorStateMachine> m_stateMachine;
    Map<String, Variant> m_variableNameToValue;
    Array<AnimatorStateMachinePlayer *> m_animatorStateMachinePlayers;

    bool m_playOnStart = true;
    bool m_playing = false;

    void ClearPlayers();

    void SetSkinnedMeshRendererBoneTransformations(
        const Map<String, Transformation> &boneAnimTransformations);

    // IEventsAnimatorStateMachine
    void OnLayerAdded(AnimatorStateMachine *stateMachine,
                      AnimatorStateMachineLayer *stateMachineLayer) override;

    void OnLayerRemoved(AnimatorStateMachine *stateMachine,
                        AnimatorStateMachineLayer *stateMachineLayer) override;
};
}

#endif  // ANIMATOR_H
