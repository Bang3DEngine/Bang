#include "Bang/Animator.h"

#include "Bang/Mesh.h"
#include "Bang/Material.h"
#include "Bang/MetaNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/SkinnedMeshRenderer.h"
#include "Bang/AnimatorStateMachineNode.h"
#include "Bang/AnimatorStateMachineConnection.h"

USING_NAMESPACE_BANG

Animator::Animator()
{
    CONSTRUCT_CLASS_ID(Animator);
    m_initCrossFadeTime.SetInfinity();
    m_endCrossFadeTime.SetInfinity();
}

Animator::~Animator()
{
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

    if (GetStateMachine())
    {
        if (GetCurrentAnimation() && IsPlaying())
        {
            m_animationTime += passedTime * GetCurrentAnimation()->GetSpeed();
            PropagateAnimatorEvents(GetCurrentAnimationIndex(),
                                    m_animationTime);

            if (now >= m_endCrossFadeTime)
            {
                EndCrossFade();
            }

            Map< String, Matrix4 > boneNameToCurrentMatrices;
            if (GetCurrentAnimation())
            {
                if (GetCurrentTargetCrossFadeAnimation())
                {
                    Time crossFadeAnimationTime = (now - m_initCrossFadeTime);
                    Time crossFadeAnimationTotalTime = (m_endCrossFadeTime -
                                                        m_initCrossFadeTime);

                    // Cross-fade
                    boneNameToCurrentMatrices =
                         Animation::GetBoneCrossFadeAnimationMatrices(
                            GetCurrentAnimation(),
                            m_animationTime,
                            GetCurrentTargetCrossFadeAnimation(),
                            crossFadeAnimationTime,
                            crossFadeAnimationTotalTime);

                    PropagateAnimatorEvents(GetCurrentTargetCrossFadeAnimationIndex(),
                                            crossFadeAnimationTime);
                }
                else
                {
                    // Direct
                    boneNameToCurrentMatrices = GetCurrentAnimation()->
                         GetBoneAnimationMatricesForTime(m_animationTime);
                }
            }
            SetSkinnedMeshRendererCurrentBoneMatrices(boneNameToCurrentMatrices);
        }
    }
}

void Animator::SetStateMachine(AnimatorStateMachine *stateMachine)
{
    if (stateMachine != GetStateMachine())
    {
        m_stateMachine.Set(stateMachine);
    }
}

void Animator::AddAnimation(Animation *animation, uint index_)
{
    RH<Animation> animationRH(animation);

    uint index = Math::Clamp(index_, 0, GetAnimations().Size());
    p_animations.Insert(animationRH, index);

    const bool setCurrentAnimation = (GetAnimations().Size() == 1);
    if (setCurrentAnimation)
    {
        ChangeCurrentAnimation(0);
    }
}

void Animator::RemoveAnimationByIndex(Animation *animation)
{
    uint index = SCAST<uint>(GetAnimations().IndexOf(RH<Animation>(animation)));
    if (index != -1u)
    {
        RemoveAnimationByIndex(index);
    }
}

void Animator::RemoveAnimationByIndex(uint animationIndex)
{
    ASSERT(animationIndex < GetAnimations().Size());
    if (GetCurrentAnimationIndex() == animationIndex)
    {
        ClearCurrentAnimation();
    }
    p_animations.RemoveByIndex(animationIndex);
}

void Animator::SetAnimation(uint animationIndex, Animation *animation)
{
    ASSERT(animationIndex < GetAnimations().Size());
    p_animations[animationIndex] = RH<Animation>(animation);
}

void Animator::ChangeCurrentAnimation(uint animationIndex)
{
    ASSERT(animationIndex < GetAnimations().Size());
    m_currentAnimationIndex = animationIndex;
    m_currentTargetCrossFadeAnimationIndex = -1u;
}

void Animator::ChangeCurrentAnimationCrossFade(uint animationIndex,
                                               Time crossFadeTime)
{
    ASSERT(animationIndex < GetAnimations().Size());
    if (animationIndex != GetCurrentAnimationIndex() &&
        animationIndex != GetCurrentTargetCrossFadeAnimationIndex())
    {
        m_currentTargetCrossFadeAnimationIndex = animationIndex;
        Time now = Time::GetNow();
        m_initCrossFadeTime = now;
        m_endCrossFadeTime  = (now + crossFadeTime);
    }
}

void Animator::ChangeCurrentAnimationCrossFade(uint animationIndex,
                                               double crossFadeTimeSeconds)
{
    ChangeCurrentAnimationCrossFade(animationIndex,
                                    Time::Seconds(crossFadeTimeSeconds));
}

void Animator::ClearCurrentAnimation()
{
    m_currentAnimationIndex = -1u;
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
    Array<SkinnedMeshRenderer*> smrs =
                        GetGameObject()->GetComponents<SkinnedMeshRenderer>();
    for (SkinnedMeshRenderer *smr : smrs)
    {
        for (const auto &pair : boneAnimMatrices)
        {
            const String &boneName = pair.first;
            const Matrix4 &boneAnimMatrix = pair.second;
            GameObject *boneGo = smr->GetBoneGameObject(boneName);
            if (boneGo && smr->GetActiveMesh()->
                          GetBonesPool().ContainsKey(boneName))
            {
                boneGo->GetTransform()->FillFromMatrix( boneAnimMatrix );
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
    Map< String, Matrix4 > boneNameToAnimationMatrices;
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

AnimatorStateMachine* Animator::GetStateMachine() const
{
    return m_stateMachine.Get();
}

Animation* Animator::GetAnimation(uint animationIndex) const
{
    if (animationIndex < GetAnimations().Size())
    {
        return p_animations[animationIndex].Get();
    }
    return nullptr;
}

const Array<RH<Animation> > &Animator::GetAnimations() const
{
    return p_animations;
}

void Animator::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Animator *animatorClone = SCAST<Animator*>(clone);
    for (const RH<Animation> &animationRH : GetAnimations())
    {
        animatorClone->AddAnimation( animationRH.Get() );
    }

    animatorClone->m_stateMachine = m_stateMachine;
}

void Animator::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    Array<GUID> newAnimationGUIDs = metaNode.GetArray<GUID>("Animations");
    bool someAnimationDiffers = (GetAnimations().Size() != newAnimationGUIDs.Size());
    if (!someAnimationDiffers)
    {
        for (int i = 0; i < GetAnimations().Size(); ++i)
        {
            const GUID &newAnimationGUID = newAnimationGUIDs[i];
            GUID currentAnimationGUID = GUID::Empty();
            if (Animation *currentAnimation = GetAnimations()[i].Get())
            {
                currentAnimationGUID = currentAnimation->GetGUID();
            }

            someAnimationDiffers = (newAnimationGUID != currentAnimationGUID);
            if (someAnimationDiffers)
            {
                break;
            }
        }
    }

    if (someAnimationDiffers)
    {
        // Clear animations
        while (!GetAnimations().IsEmpty())
        {
            RemoveAnimationByIndex(0u);
        }

        // Add all new animations
        for (const GUID &newAnimationGUID : newAnimationGUIDs)
        {
            RH<Animation> newAnimationRH = Resources::Load<Animation>(newAnimationGUID);
            AddAnimation(newAnimationRH.Get());
        }
    }

    if (metaNode.Contains("PlayOnStart"))
    {
        SetPlayOnStart( metaNode.Get<bool>("PlayOnStart") );
    }
}

void Animator::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    Array<GUID> animationGUIDs;
    for (const RH<Animation> &animationRH : GetAnimations())
    {
        Animation *animation = animationRH.Get();
        animationGUIDs.PushBack( animation ? animation->GetGUID() : GUID::Empty());
    }
    metaNode->SetArray("Animations", animationGUIDs);

    metaNode->Set("PlayOnStart", GetPlayOnStart());
}

void Animator::EndCrossFade()
{
    m_currentAnimationIndex = GetCurrentTargetCrossFadeAnimationIndex();
    m_currentTargetCrossFadeAnimationIndex = -1u;
    m_animationTime = (m_endCrossFadeTime - m_initCrossFadeTime);
    m_initCrossFadeTime.SetInfinity();
    m_endCrossFadeTime.SetInfinity();
}

void Animator::PropagateAnimatorEvents(uint currentAnimationIndex,
                                       Time currentAnimationTime)
{
    Animation *currentAnimation = GetAnimation(currentAnimationIndex);

    EventEmitter<IEventsAnimator>::PropagateToListeners(
        &IEventsAnimator::OnAnimationTick,
        this,
        currentAnimation,
        currentAnimationIndex,
        Time::Seconds(Animation::WrapTime(currentAnimationTime.GetSeconds(),
                                          currentAnimation->GetDurationInSeconds(),
                                          currentAnimation->GetWrapMode())),
        currentAnimationTime);
}

uint Animator::GetCurrentAnimationIndex() const
{
    return m_currentAnimationIndex;
}

Animation *Animator::GetCurrentAnimation() const
{
    return GetStateMachine() ?
                GetStateMachine()->GetCurrentNode()->GetAnimation() :
                nullptr;
}

uint Animator::GetCurrentTargetCrossFadeAnimationIndex() const
{
    return m_currentTargetCrossFadeAnimationIndex;
}

Animation *Animator::GetCurrentTargetCrossFadeAnimation() const
{
    if (GetCurrentTargetCrossFadeAnimationIndex() <= GetAnimations().Size())
    {
        return GetAnimations()[GetCurrentTargetCrossFadeAnimationIndex()].Get();
    }
    return nullptr;
}

