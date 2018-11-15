#include "Bang/Animator.h"

#include <functional>
#include <utility>

#include "Bang/Animation.h"
#include "Bang/AnimatorLayerMask.h"
#include "Bang/AnimatorStateMachine.h"
#include "Bang/AnimatorStateMachineLayer.h"
#include "Bang/AnimatorStateMachinePlayer.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/Map.tcc"
#include "Bang/Matrix4.tcc"
#include "Bang/Mesh.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/SkinnedMeshRenderer.h"
#include "Bang/Transform.h"

namespace Bang
{
class ICloneable;
}

using namespace Bang;

Animator::Animator()
{
    CONSTRUCT_CLASS_ID(Animator);
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
        Map<String, Animation::BoneTransformation>
            combinedLayersBonesTransformations;
        for (AnimatorStateMachinePlayer *player : GetPlayers())
        {
            player->Step(passedTime);

            if (Animation *currentAnim = player->GetCurrentAnimation())
            {
                const Time currentAnimTime = player->GetCurrentNodeTime();

                AnimatorStateMachineLayer *layer =
                    player->GetStateMachineLayer();
                ASSERT(layer);

                if (layer->GetEnabled())
                {
                    bool hasLayerMask = (layer->GetLayerMask() != nullptr);
                    Set<String> layerMask =
                        hasLayerMask
                            ? layer->GetLayerMask()->GetBoneMaskNamesSet(this)
                            : Set<String>();

                    Map<String, Animation::BoneTransformation>
                        layerBoneNameToBoneTransform;
                    if (AnimatorStateMachineNode *nextNode =
                            player->GetNextNode())
                    {
                        // Cross fading
                        Map<String, Animation::BoneTransformation>
                            prevBoneTransformations =
                                player->GetCurrentNode()
                                    ->GetBoneTransformations(
                                        player->GetCurrentNodeTime());

                        Map<String, Animation::BoneTransformation>
                            nextBoneTransformations =
                                nextNode->GetBoneTransformations(
                                    player->GetCurrentTransitionTime());

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
                                currentAnimTime);
                    }

                    for (const auto &pair : layerBoneNameToBoneTransform)
                    {
                        const String &layerBoneName = pair.first;
                        const Animation::BoneTransformation
                            &layerBoneTransform = pair.second;

                        bool considerThisBone =
                            (!hasLayerMask ||
                             layerMask.Contains(layerBoneName));
                        if (considerThisBone)
                        {
                            auto it = combinedLayersBonesTransformations.Find(
                                layerBoneName);
                            if (it != combinedLayersBonesTransformations.End())
                            {
                                Animation::BoneTransformation
                                    &combinedLayerBoneTransformation =
                                        it->second;
                                combinedLayerBoneTransformation.position +=
                                    layerBoneTransform.position;
                                combinedLayerBoneTransformation.rotation *=
                                    layerBoneTransform.rotation;
                                combinedLayerBoneTransformation.scale *=
                                    layerBoneTransform.scale;
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

        Map<String, Matrix4> totalBoneNameToBoneMatrices =
            Animation::GetBoneMatrices(combinedLayersBonesTransformations);
        SetSkinnedMeshRendererCurrentBoneMatrices(totalBoneNameToBoneMatrices);
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

void Animator::SetSkinnedMeshRendererCurrentBoneMatrices(
    const Map<String, Matrix4> &boneAnimMatrices)
{
    Array<SkinnedMeshRenderer *> smrs =
        GetGameObject()->GetComponents<SkinnedMeshRenderer>();
    for (SkinnedMeshRenderer *smr : smrs)
    {
        for (const auto &pair : boneAnimMatrices)
        {
            const String &boneName = pair.first;
            const Matrix4 &boneAnimMatrix = pair.second;
            GameObject *boneGo = smr->GetBoneGameObject(boneName);
            if (boneGo &&
                smr->GetActiveMesh()->GetBonesPool().ContainsKey(boneName))
            {
                boneGo->GetTransform()->FillFromMatrix(boneAnimMatrix);
            }
        }
    }

    for (SkinnedMeshRenderer *smr : smrs)
    {
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

AnimatorStateMachine *Animator::GetStateMachine() const
{
    return m_stateMachine.Get();
}

const Array<AnimatorStateMachinePlayer *> &Animator::GetPlayers() const
{
    return m_animatorStateMachinePlayers;
}

void Animator::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);

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
        SetStateMachine(Resources::Load<AnimatorStateMachine>(
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
