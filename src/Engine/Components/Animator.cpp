#include "Bang/Animator.h"

#include "Bang/Mesh.h"
#include "Bang/Material.h"
#include "Bang/MetaNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/SkinnedMeshRenderer.h"

USING_NAMESPACE_BANG

Animator::Animator()
{
    CONSTRUCT_CLASS_ID(Animator)
}

Animator::~Animator()
{
}

void Animator::OnStart()
{
    Component::OnStart();

    m_prevFrameTimeMillis = Time::GetNow_Millis();
    m_animationTimeSeconds = 0.0f;

    if (GetPlayOnStart())
    {
        Play();
    }
}

void Animator::OnUpdate()
{
    Component::OnUpdate();

    Time::TimeT passedTimeMillis = (Time::GetNow_Millis() - m_prevFrameTimeMillis);
    m_prevFrameTimeMillis = Time::GetNow_Millis();

    if (GetCurrentAnimation() && IsPlaying())
    {
        double passedTimeSeconds = (passedTimeMillis / double(1e3));
        m_animationTimeSeconds += passedTimeSeconds *
                                  GetCurrentAnimation()->GetSpeed();

        Map< String, Matrix4 > boneNameToCurrentMatrices = GetCurrentAnimation()->
                 GetBoneAnimationMatricesForSecond(m_animationTimeSeconds);
        SetSkinnedMeshRendererCurrentBoneMatrices(boneNameToCurrentMatrices);
    }
}

void Animator::OnRender(RenderPass rp)
{
    Component::OnRender(rp);
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

void Animator::Play()
{
    m_playing = true;
}

void Animator::Stop()
{
    m_playing = false;
    m_animationTimeSeconds = 0.0;
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

uint Animator::GetCurrentAnimationIndex() const
{
    return m_currentAnimationIndex;
}

Animation *Animator::GetCurrentAnimation() const
{
    return GetAnimation( GetCurrentAnimationIndex() );
}

