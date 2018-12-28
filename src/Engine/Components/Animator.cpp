#include "Bang/Animator.h"

#include <functional>
#include <utility>

#include "Bang/Animation.h"
#include "Bang/AnimatorLayerMask.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineLayer.h"
#include "Bang/AnimatorStateMachinePlayer.h"
#include "Bang/AnimatorStateMachineVariable.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/Map.tcc"
#include "Bang/Matrix4.tcc"
#include "Bang/Mesh.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/SkinnedMeshRenderer.h"
#include "Bang/Transform.h"

namespace Bang
{
class ICloneable;
}

using namespace Bang;

Animator::Animator()
{
    SET_INSTANCE_CLASS_ID(Animator);
}

Animator::~Animator()
{
    ClearPlayers();
}

void Animator::OnStart()
{
    Component::OnStart();

    m_prevFrameTime = Time::GetNow();
    m_animationTime.SetNanos(0);

    if (GetPlayOnStart())
    {
        Play();
    }
}

void Animator::OnUpdate()
{
    Component::OnUpdate();

    Time now = Time::GetNow();
    Time passedTime = (now - m_prevFrameTime);
    m_prevFrameTime = now;

    AnimatorStateMachine *sm = GetStateMachine();
    if (sm && IsPlaying())
    {
        Map<String, Transformation> combinedLayersBonesTransformations;
        for (AnimatorStateMachinePlayer *player : GetPlayers())
        {
            player->Step(this, passedTime);

            if (player->GetCurrentAnimation())
            {
                const Time currentAnimTime = player->GetCurrentNodeTime();

                AnimatorStateMachineLayer *layer =
                    player->GetStateMachineLayer();
                ASSERT(layer);

                if (layer->GetEnabled())
                {
                    bool hasLayerMask = (layer->GetLayerMask() != nullptr);
                    Set<String> layerMask;
                    if (hasLayerMask)
                    {
                        layerMask =
                            layer->GetLayerMask()->GetBoneMaskNamesSet(this);
                    }

                    Map<String, Transformation> layerBoneNameToBoneTransform;
                    if (AnimatorStateMachineNode *nextNode =
                            player->GetNextNode())
                    {
                        // Cross fading
                        Map<String, Transformation> prevBoneTransformations =
                            player->GetCurrentNode()->GetBoneTransformations(
                                player->GetCurrentNodeTime(), this);

                        Map<String, Transformation> nextBoneTransformations =
                            nextNode->GetBoneTransformations(
                                player->GetCurrentTransitionTime(), this);

                        double totalCrossFadeSeconds = Math::Max(
                            player->GetCurrentTransitionDuration().GetSeconds(),
                            0.01);
                        float nextWeight =
                            (player->GetCurrentTransitionTime().GetSeconds() /
                             totalCrossFadeSeconds);

                        ASSERT(player->GetCurrentTransition());

                        layerBoneNameToBoneTransform =
                            Animation::GetInterpolatedBoneTransformations(
                                prevBoneTransformations,
                                nextBoneTransformations,
                                nextWeight);
                    }
                    else
                    {
                        // Simple animation
                        layerBoneNameToBoneTransform =
                            player->GetCurrentNode()->GetBoneTransformations(
                                currentAnimTime, this);
                    }

                    for (const auto &pair : layerBoneNameToBoneTransform)
                    {
                        const String &layerBoneName = pair.first;
                        const Transformation &layerBoneTransform = pair.second;

                        bool considerThisBone =
                            (!hasLayerMask ||
                             layerMask.Contains(layerBoneName));
                        if (considerThisBone)
                        {
                            auto it = combinedLayersBonesTransformations.Find(
                                layerBoneName);
                            if (it != combinedLayersBonesTransformations.End())
                            {
                                Transformation
                                    &combinedLayerBoneTransformation =
                                        it->second;
                                combinedLayerBoneTransformation.Translate(
                                    layerBoneTransform.GetPosition());
                                combinedLayerBoneTransformation.Rotate(
                                    layerBoneTransform.GetRotation());
                                combinedLayerBoneTransformation.Scale(
                                    layerBoneTransform.GetScale());
                            }
                            else
                            {
                                combinedLayersBonesTransformations.Add(
                                    layerBoneName, layerBoneTransform);
                            }
                        }
                    }
                }
            }
        }

        SetSkinnedMeshRendererBoneTransformations(
            combinedLayersBonesTransformations);
    }
}

void Animator::SetStateMachine(AnimatorStateMachine *stateMachine)
{
    if (stateMachine != GetStateMachine())
    {
        m_stateMachine.Set(stateMachine);

        if (GetStateMachine())
        {
            for (AnimatorStateMachineLayer *layer :
                 GetStateMachine()->GetLayers())
            {
                OnLayerAdded(GetStateMachine(), layer);
            }
        }
    }
}

void Animator::SetPlayOnStart(bool playOnStart)
{
    if (playOnStart != GetPlayOnStart())
    {
        m_playOnStart = playOnStart;
    }
}

void Animator::SetVariableVariant(const String &varName,
                                  const Variant &variableVariant)
{
    m_variableNameToValue[varName] = variableVariant;
}

void Animator::SetVariableFloat(const String &varName, float value)
{
    m_variableNameToValue[varName].SetFloat(value);
}

void Animator::SetVariableBool(const String &varName, bool value)
{
    m_variableNameToValue[varName].SetBool(value);
}

void Animator::SetVariableTrigger(const String &varName, bool value)
{
    SetVariableBool(varName, value);
}

void Animator::SetSkinnedMeshRendererBoneTransformations(
    const Map<String, Transformation> &boneAnimMatrices)
{
    Array<SkinnedMeshRenderer *> smrs =
        GetGameObject()->GetComponents<SkinnedMeshRenderer>();
    for (SkinnedMeshRenderer *smr : smrs)
    {
        GameObject *rootBoneGo = smr->GetRootBoneGameObject();
        if (!rootBoneGo)
        {
            continue;
        }

        Array<GameObject *> allBoneGos = rootBoneGo->GetChildrenRecursively();
        for (GameObject *boneGo : allBoneGos)
        {
            const String &boneName = boneGo->GetName();
            auto it = boneAnimMatrices.Find(boneGo->GetName());
            if (it != boneAnimMatrices.End())
            {
                const Transformation &boneAnimTransformation = it->second;
                if (GameObject *boneGo = smr->GetBoneGameObject(boneName))
                {
                    boneGo->GetTransform()->FillFromTransformation(
                        boneAnimTransformation);
                }
            }
            else
            {
                boneGo->GetTransform()->FillFromTransformation(
                    smr->GetInitialTransformationFor(boneName));
            }
        }
        smr->UpdateBonesMatricesFromTransformMatrices();
    }
}

void Animator::OnLayerAdded(AnimatorStateMachine *stateMachine,
                            AnimatorStateMachineLayer *stateMachineLayer)
{
    ASSERT(stateMachine == GetStateMachine());

    AnimatorStateMachinePlayer *player = new AnimatorStateMachinePlayer();
    player->SetStateMachineLayer(stateMachineLayer);
    m_animatorStateMachinePlayers.PushBack(player);

    const Array<AnimatorStateMachineVariable *> &vars =
        stateMachine->GetVariableDefaults();
    for (AnimatorStateMachineVariable *var : vars)
    {
        SetVariableVariant(var->GetName(), var->GetVariant());
    }
}

void Animator::OnLayerRemoved(AnimatorStateMachine *stateMachine,
                              AnimatorStateMachineLayer *stateMachineLayer)
{
    ASSERT(stateMachine == GetStateMachine());

    for (AnimatorStateMachinePlayer *player : GetPlayers())
    {
        if (player->GetStateMachineLayer() == stateMachineLayer)
        {
            m_animatorStateMachinePlayers.Remove(player);
            break;
        }
    }
}

void Animator::Play()
{
    m_playing = true;
}

void Animator::Stop()
{
    m_playing = false;
    m_animationTime.SetNanos(0);
}

void Animator::Pause()
{
    m_playing = false;
}

bool Animator::IsPlaying() const
{
    return m_playing;
}

bool Animator::GetPlayOnStart() const
{
    return m_playOnStart;
}

Variant Animator::GetVariableVariant(const String &varName) const
{
    if (m_variableNameToValue.ContainsKey(varName))
    {
        return m_variableNameToValue.Get(varName);
    }
    return Variant();
}

AnimatorStateMachineVariable::Type Animator::GetVariableType(
    const String &varName) const
{
    if (GetStateMachine())
    {
        if (AnimatorStateMachineVariable *var =
                GetStateMachine()->GetVariableDefault(varName))
        {
            return var->GetType();
        }
    }
    return AnimatorStateMachineVariable::Type::FLOAT;
}

float Animator::GetVariableFloat(const String &varName) const
{
    return GetVariableVariant(varName).GetFloat();
}

bool Animator::GetVariableBool(const String &varName) const
{
    return GetVariableVariant(varName).GetBool();
}

bool Animator::GetVariableTrigger(const String &varName) const
{
    return GetVariableBool(varName);
}

AnimatorStateMachine *Animator::GetStateMachine() const
{
    return m_stateMachine.Get();
}

const Array<AnimatorStateMachinePlayer *> &Animator::GetPlayers() const
{
    return m_animatorStateMachinePlayers;
}

void Animator::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Component::CloneInto(clone, cloneGUID);

    Animator *animatorClone = SCAST<Animator *>(clone);
    animatorClone->SetPlayOnStart(GetPlayOnStart());
    animatorClone->SetStateMachine(GetStateMachine());
}

void Animator::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("PlayOnStart"))
    {
        SetPlayOnStart(metaNode.Get<bool>("PlayOnStart"));
    }

    if (metaNode.Contains("StateMachine"))
    {
        SetStateMachine(Assets::Load<AnimatorStateMachine>(
                            metaNode.Get<GUID>("StateMachine"))
                            .Get());
    }
}

void Animator::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set(
        "StateMachine",
        GetStateMachine() ? GetStateMachine()->GetGUID() : GUID::Empty());
    metaNode->Set("PlayOnStart", GetPlayOnStart());
}

void Animator::ClearPlayers()
{
    for (AnimatorStateMachinePlayer *player : GetPlayers())
    {
        delete player;
    }
}
