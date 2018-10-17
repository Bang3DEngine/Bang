#include "Bang/Animator.h"

#include <functional>
#include <utility>

#include "Bang/Animation.h"
#include "Bang/AnimatorStateMachine.h"
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
    m_animationStateMachinePlayer = new AnimatorStateMachinePlayer();
}

Animator::~Animator()
{
    delete m_animationStateMachinePlayer;
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
        ASSERT(GetPlayer());
        GetPlayer()->Step(passedTime);

        if (Animation *currentAnim = GetPlayer()->GetCurrentAnimation())
        {
            const Time currentAnimTime = GetPlayer()->GetCurrentNodeTime();

            Map<String, Matrix4> boneNameToCurrentMatrices;
            if (Animation *nextAnim = GetPlayer()->GetNextAnimation())
            {
                // Cross fading
                ASSERT(GetPlayer()->GetCurrentTransition());
                boneNameToCurrentMatrices =
                    Animation::GetBoneCrossFadeAnimationMatrices(
                        currentAnim,
                        currentAnimTime,
                        nextAnim,
                        GetPlayer()->GetCurrentTransitionTime(),
                        GetPlayer()->GetCurrentTransitionDuration());
            }
            else
            {
                // Simple animation
                boneNameToCurrentMatrices =
                    currentAnim->GetBoneAnimationMatricesForTime(
                        currentAnimTime);
            }

            SetSkinnedMeshRendererCurrentBoneMatrices(
                boneNameToCurrentMatrices);
        }
    }
}

void Animator::SetStateMachine(AnimatorStateMachine *stateMachine)
{
    if (stateMachine != GetStateMachine())
    {
        m_stateMachine.Set(stateMachine);
        GetPlayer()->SetStateMachine(GetStateMachine());
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

Map<String, Matrix4> Animator::GetBoneAnimationMatrices(Animation *animation,
                                                        Time animationTime)
{
    Map<String, Matrix4> boneNameToAnimationMatrices;
    if (animation)
    {
        boneNameToAnimationMatrices =
            animation->GetBoneAnimationMatricesForTime(animationTime);
    }
    return boneNameToAnimationMatrices;
}

Map<String, Matrix4> Animator::GetBoneCrossFadeAnimationMatrices(
    Animation *prevAnimation,
    Time prevAnimationTime,
    Animation *nextAnimation,
    Time currentCrossFadeTime,
    Time totalCrossFadeTime)
{
    Map<String, Matrix4> boneCrossFadeAnimationMatrices =
        Animation::GetBoneCrossFadeAnimationMatrices(prevAnimation,
                                                     prevAnimationTime,
                                                     nextAnimation,
                                                     currentCrossFadeTime,
                                                     totalCrossFadeTime);
    return boneCrossFadeAnimationMatrices;
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

AnimatorStateMachinePlayer *Animator::GetPlayer() const
{
    return m_animationStateMachinePlayer;
}
