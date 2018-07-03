#include "Bang/Animator.h"

#include "Bang/Material.h"
#include "Bang/GameObject.h"
#include "Bang/MeshRenderer.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

Animator::Animator()
{
}

Animator::~Animator()
{
}

void Animator::OnUpdate()
{
    Component::OnUpdate();

    Time::TimeT passedTimeMillis = (Time::GetNow_Millis() - m_prevFrameTimeMillis);
    m_prevFrameTimeMillis = Time::GetNow_Millis();

    double passedTimeSeconds = (passedTimeMillis / double(1e3));
    if (IsPlaying())
    {
        m_animationTimeSeconds += passedTimeSeconds;
    }

    if (GetAnimation())
    {
        List<MeshRenderer*> mrs = GetGameObject()->GetComponents<MeshRenderer>();
        for (MeshRenderer *mr : mrs)
        {
            if (Material *mat = mr->GetActiveMaterial())
            {
                if (ShaderProgram *sp = mat->GetShaderProgram())
                {
                    GL::Push(GL::Pushable::SHADER_PROGRAM);

                    sp->Bind();


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

