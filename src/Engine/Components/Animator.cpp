#include "Bang/Animator.h"

#include "Bang/Mesh.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/SkinnedMeshRenderer.h"

USING_NAMESPACE_BANG

Array<Matrix4> Animator::s_identityMatrices;
Map<String, Matrix4> Animator::s_identityBoneMatrices;

Animator::Animator()
{
    if (Animator::s_identityMatrices.IsEmpty())
    {
        for (int i = 0; i < Animator::MaxNumBones; ++i)
        {
            Animator::s_identityMatrices.PushBack(Matrix4::Identity);
            Animator::s_identityBoneMatrices.Add("", Matrix4::Identity);
        }
    }
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
        m_animationTimeSeconds += passedTimeSeconds;
    }
}

void Animator::OnRender(RenderPass rp)
{
    Component::OnRender(rp);

    if (GetAnimation() && IsPlaying())
    {
        Map< String, Matrix4 > boneNameToCurrentMatrices =
           GetAnimation()->GetBoneAnimationMatricesForSecond(m_animationTimeSeconds);
        SetSkinnedMeshRendererCurrentBoneMatrices(rp, boneNameToCurrentMatrices);
    }
}

void Animator::SetSkinnedMeshRendererCurrentBoneMatrices(
                                RenderPass rp,
                                const Map<String, Matrix4> &boneAnimMatrices)
{
    List<SkinnedMeshRenderer*> smrs =
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

