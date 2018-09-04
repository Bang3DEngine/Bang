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
}

Animator::~Animator()
{
}

void Animator::OnStart()
{
    Component::OnStart();

    m_prevFrameTimeMillis = Time::GetNow_Millis();
    m_animationTimeSeconds = 0.0f;
}

#include "Bang/Input.h"
void Animator::OnUpdate()
{
    Component::OnUpdate();

    Time::TimeT passedTimeMillis = (Time::GetNow_Millis() - m_prevFrameTimeMillis);
    m_prevFrameTimeMillis = Time::GetNow_Millis();

    if (Input::GetKeyDown(Key::X))
    {
        if (IsPlaying()) Stop(); else Play();
    }

    if (GetAnimation() && IsPlaying())
    {
        double passedTimeSeconds = (passedTimeMillis / double(1e3));
        m_animationTimeSeconds += passedTimeSeconds * GetAnimation()->GetSpeed();
    }
}

void Animator::OnRender(RenderPass rp)
{
    Component::OnRender(rp);

    if (rp == RenderPass::SCENE || rp == RenderPass::SCENE_TRANSPARENT)
    {
        if (GetAnimation() && IsPlaying())
        {
            Map< String, Matrix4 > boneNameToCurrentMatrices =
               GetAnimation()->GetBoneAnimationMatricesForSecond(m_animationTimeSeconds);
            SetSkinnedMeshRendererCurrentBoneMatrices(rp, boneNameToCurrentMatrices);
        }
    }
}

void Animator::SetSkinnedMeshRendererCurrentBoneMatrices(
                                RenderPass rp,
                                const Map<String, Matrix4> &boneAnimMatrices)
{
    Array<SkinnedMeshRenderer*> smrs =
                        GetGameObject()->GetComponents<SkinnedMeshRenderer>();
    for (SkinnedMeshRenderer *smr : smrs)
    {
        if (Material *mat = smr->GetActiveMaterial())
        {
            if (mat->GetRenderPass() == rp)
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
        }
    }

    for (SkinnedMeshRenderer *smr : smrs)
    {
        smr->UpdateBonesMatricesFromTransformMatrices();
    }
}

void Animator::SetAnimation(Animation *animation)
{
    if (animation != GetAnimation())
    {
        Stop();
        p_animation.Set(animation);
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

Animation *Animator::GetAnimation() const
{
    return p_animation.Get();
}

void Animator::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Animator *animatorClone = SCAST<Animator*>(clone);
    animatorClone->SetAnimation( GetAnimation() );
}

void Animator::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("Animation"))
    {
        RH<Animation> animation = Resources::Load<Animation>(
                                             metaNode.Get<GUID>("Animation") );
        SetAnimation( animation.Get() );
    }
}

void Animator::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("Animation", GetAnimation() ? GetAnimation()->GetGUID() :
                                                GUID::Empty());
}

