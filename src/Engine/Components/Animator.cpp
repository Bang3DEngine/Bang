#include "Bang/Animator.h"

#include "Bang/Material.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/SkinnedMeshRenderer.h"

USING_NAMESPACE_BANG

Array<Matrix4> Animator::s_identityMatrices;

Animator::Animator()
{
    if (Animator::s_identityMatrices.IsEmpty())
    {
        for (int i = 0; i < Animator::MaxNumBones; ++i)
        {
            Animator::s_identityMatrices.PushBack( Matrix4(1.0f) );
        }
    }
}

Animator::~Animator()
{
}

void Animator::OnStart()
{
    m_prevFrameTimeMillis = Time::GetNow_Millis();
    m_animationTimeSeconds = 0.0f;
}

void Animator::OnUpdate()
{
    Component::OnUpdate();

    Time::TimeT passedTimeMillis = (Time::GetNow_Millis() - m_prevFrameTimeMillis);
    m_prevFrameTimeMillis = Time::GetNow_Millis();

    if (IsPlaying())
    {
        double passedTimeSeconds = (passedTimeMillis / double(1e3));
        m_animationTimeSeconds += passedTimeSeconds;
    }
}

void Animator::OnRender(RenderPass rp)
{
    Component::OnRender(rp);

    if (GetAnimation())
    {
        Array<Matrix4> currentAnimationMatrices;
        for (int i = 0; i < MaxNumBones; ++i)
        {
            currentAnimationMatrices.PushBack(
                        Matrix4(1.0f + m_animationTimeSeconds * 0.01f) );
        }
        SetSkinnedMeshRendererCurrentBoneMatrices(rp, currentAnimationMatrices);
    }
    else
    {
        SetSkinnedMeshRendererCurrentBoneMatrices(rp, Animator::s_identityMatrices);
    }
}

void Animator::SetSkinnedMeshRendererCurrentBoneMatrices(
                RenderPass rp,
                const Array<Matrix4> &boneMatrices)
{
    List<SkinnedMeshRenderer*> smrs = GetGameObject()->GetComponents<SkinnedMeshRenderer>();
    for (SkinnedMeshRenderer *smr : smrs)
    {
        if (Material *mat = smr->GetActiveMaterial())
        {
            if (mat->GetRenderPass() == rp)
            {
                if (ShaderProgram *sp = mat->GetShaderProgram())
                {
                    GL::Push(GL::Pushable::SHADER_PROGRAM);

                    sp->Bind();
                    sp->SetMatrix4Array("B_BoneAnimationMatrices",
                                        boneMatrices,
                                        false);

                    GL::Pop(GL::Pushable::SHADER_PROGRAM);
                }
            }
        }
    }
}

void Animator::SetAnimation(Animation *animation)
{
    p_animation.Set(animation);
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

void Animator::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);

    if (xmlInfo.Contains("Animation"))
    {
        RH<Animation> animation = Resources::Load<Animation>(
                                             xmlInfo.Get<GUID>("Animation") );
        SetAnimation( animation.Get() );
    }
}

void Animator::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("Animation", GetAnimation() ? GetAnimation()->GetGUID() :
                                               GUID::Empty());
}

