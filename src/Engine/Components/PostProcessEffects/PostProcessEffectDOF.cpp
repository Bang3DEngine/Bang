#include "Bang/PostProcessEffectDOF.h"

#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/Resources.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"

using namespace Bang;

PostProcessEffectDOF::PostProcessEffectDOF()
{
    SET_INSTANCE_CLASS_ID(PostProcessEffectDOF);

    m_foregroundTexture = Resources::Create<Texture2D>();
    m_foregroundTexture.Get()->CreateEmpty(1, 1);
    m_foregroundTexture.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
    m_foregroundTexture.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

    m_auxiliarTexture = Resources::Create<Texture2D>();
    m_auxiliarTexture.Get()->CreateEmpty(1, 1);
    m_auxiliarTexture.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
    m_auxiliarTexture.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

    m_dofSP.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        ShaderProgramFactory::GetEngineShadersDir().Append(
            "DepthOfField.frag")));

    SetType(PostProcessEffect::Type::AFTER_SCENE_AND_LIGHT);
}

PostProcessEffectDOF::~PostProcessEffectDOF()
{
}

void PostProcessEffectDOF::OnRender(RenderPass renderPass)
{
    PostProcessEffect::OnRender(renderPass);

    if (MustBeRendered(renderPass))
    {
        GL::Push(GL::Pushable::VIEWPORT);
        GL::Push(GL::Pushable::BLEND_STATES);
        GL::Push(GL::BindTarget::SHADER_PROGRAM);

        Vector2i dofTexSize = GL::GetViewportSize();
        GL::SetViewport(0, 0, dofTexSize.x, dofTexSize.y);

        GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        GL::Disable(GL::Enablable::BLEND);

        GEngine *ge = GEngine::GetInstance();

        Texture2D *sceneColorTexture =
            ge->GetActiveGBuffer()->GetDrawColorTexture();
        ge->BlurTexture(sceneColorTexture,
                        m_auxiliarTexture.Get(),
                        m_foregroundTexture.Get(),
                        GetBlurRadius(),
                        BlurType::KAWASE);

        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
        GL::Pop(GL::Pushable::VIEWPORT);

        Texture2D *sceneDepthTexture =
            ge->GetActiveGBuffer()->GetSceneDepthStencilTexture();

        ShaderProgram *sp = m_dofSP.Get();
        sp->Bind();
        sp->SetFloat("B_FadeRange", GetFadeRange());
        sp->SetFloat("B_ForegroundThreshold", GetForegroundThreshold());
        sp->SetTexture2D("B_SceneDepthTexture", sceneDepthTexture);
        sp->SetTexture2D("B_SceneBackgroundColorTexture", sceneColorTexture);
        sp->SetTexture2D("B_SceneForegroundColorTexture",
                         m_foregroundTexture.Get());

        ge->GetActiveGBuffer()->ApplyPass(sp, true);

        GL::Pop(GL::BindTarget::SHADER_PROGRAM);
        GL::Pop(GL::Pushable::BLEND_STATES);
    }
}

void PostProcessEffectDOF::SetBlurRadius(uint blurRadius)
{
    m_blurRadius = blurRadius;
}

void PostProcessEffectDOF::SetFadeRange(float fadeRange)
{
    m_fadeRange = fadeRange;
}

void PostProcessEffectDOF::SetForegroundThreshold(float foregroundThreshold)
{
    m_foregroundThreshold = foregroundThreshold;
}

uint PostProcessEffectDOF::GetBlurRadius() const
{
    return m_blurRadius;
}

float PostProcessEffectDOF::GetFadeRange() const
{
    return m_fadeRange;
}

float PostProcessEffectDOF::GetForegroundThreshold() const
{
    return m_foregroundThreshold;
}

void PostProcessEffectDOF::Reflect()
{
    PostProcessEffect::Reflect();

    GetReflectStructPtr()
        ->GetReflectVariablePtr("PostProcess Shader")
        ->SetHints(BANG_REFLECT_HINT_SHOWN(false));
    GetReflectStructPtr()->GetReflectVariablePtr("Type")->SetHints(
        BANG_REFLECT_HINT_SHOWN(false));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectDOF,
                                   "Foreground threshold",
                                   SetForegroundThreshold,
                                   GetForegroundThreshold,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f) +
                                       BANG_REFLECT_HINT_STEP_VALUE(0.025f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectDOF,
                                   "Fade range",
                                   SetFadeRange,
                                   GetFadeRange,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f) +
                                       BANG_REFLECT_HINT_STEP_VALUE(0.01f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectDOF,
                                   "Blur radius",
                                   SetBlurRadius,
                                   GetBlurRadius,
                                   BANG_REFLECT_HINT_MIN_VALUE(1.0f));
}
