#include "Bang/PostProcessEffectDOF.h"

#include "Bang/Assets.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"

using namespace Bang;

PostProcessEffectDOF::PostProcessEffectDOF()
{
    SET_INSTANCE_CLASS_ID(PostProcessEffectDOF);

    m_blurredTexture = Assets::Create<Texture2D>();
    m_blurredTexture.Get()->CreateEmpty(1, 1);
    m_blurredTexture.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
    m_blurredTexture.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

    m_blurAuxiliarTexture = Assets::Create<Texture2D>();
    m_blurAuxiliarTexture.Get()->CreateEmpty(1, 1);
    m_blurAuxiliarTexture.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
    m_blurAuxiliarTexture.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

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
                        m_blurAuxiliarTexture.Get(),
                        m_blurredTexture.Get(),
                        GetBlurRadius(),
                        BlurType::KAWASE);

        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
        GL::Pop(GL::Pushable::VIEWPORT);

        Texture2D *sceneDepthTexture =
            ge->GetActiveGBuffer()->GetSceneDepthStencilTexture();

        ShaderProgram *sp = m_dofSP.Get();
        sp->Bind();
        Camera *cam = Camera::GetActive();
        sp->SetMatrix4("B_ViewInv", cam->GetViewMatrix().Inversed());
        sp->SetMatrix4("B_ProjectionInv",
                       cam->GetProjectionMatrix().Inversed());
        sp->SetFloat("B_FocusRange", GetFocusRange());
        sp->SetFloat("B_FocusDistanceWorld", GetFocusDistanceWorld());
        sp->SetFloat("B_Fading", GetFading());
        sp->SetTexture2D("B_SceneDepthTexture", sceneDepthTexture);
        sp->SetTexture2D("B_SceneColorTexture", sceneColorTexture);
        sp->SetTexture2D("B_BlurredSceneColorTexture", m_blurredTexture.Get());

        ge->GetActiveGBuffer()->ApplyPass(sp, true);

        GL::Pop(GL::BindTarget::SHADER_PROGRAM);
        GL::Pop(GL::Pushable::BLEND_STATES);
    }
}

void PostProcessEffectDOF::SetFading(float fading)
{
    m_fading = fading;
}

void PostProcessEffectDOF::SetBlurRadius(uint blurRadius)
{
    m_blurRadius = blurRadius;
}

void PostProcessEffectDOF::SetFocusRange(float focusRange)
{
    m_focusRange = focusRange;
}

void PostProcessEffectDOF::SetFocusDistanceWorld(float focusDistanceWorld)
{
    m_focusDistanceWorld = focusDistanceWorld;
}

float PostProcessEffectDOF::GetFading() const
{
    return m_fading;
}

uint PostProcessEffectDOF::GetBlurRadius() const
{
    return m_blurRadius;
}

float PostProcessEffectDOF::GetFocusRange() const
{
    return m_focusRange;
}

float PostProcessEffectDOF::GetFocusDistanceWorld() const
{
    return m_focusDistanceWorld;
}

void PostProcessEffectDOF::Reflect()
{
    PostProcessEffect::Reflect();

    GetReflectStructPtr()
        ->GetReflectVariablePtr("PostProcess Shader")
        ->SetHints(BANG_REFLECT_HINT_SHOWN(false));
    GetReflectStructPtr()->GetReflectVariablePtr("Type")->SetHints(
        BANG_REFLECT_HINT_SHOWN(false));

    BANG_REFLECT_VAR_MEMBER_HINTED(
        PostProcessEffectDOF,
        "Focus distance",
        SetFocusDistanceWorld,
        GetFocusDistanceWorld,
        BANG_REFLECT_HINT_MIN_VALUE(0.0f) + BANG_REFLECT_HINT_STEP_VALUE(0.5f));

    BANG_REFLECT_VAR_MEMBER_HINTED(
        PostProcessEffectDOF,
        "Focus range",
        SetFocusRange,
        GetFocusRange,
        BANG_REFLECT_HINT_MIN_VALUE(0.0f) + BANG_REFLECT_HINT_STEP_VALUE(0.5f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectDOF,
                                   "Fading",
                                   SetFading,
                                   GetFading,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 1.0f) +
                                       BANG_REFLECT_HINT_STEP_VALUE(0.05f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectDOF,
                                   "Blur radius",
                                   SetBlurRadius,
                                   GetBlurRadius,
                                   BANG_REFLECT_HINT_MIN_VALUE(1.0f));
}
