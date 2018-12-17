#include "Bang/PostProcessEffectBloom.h"

#include "Bang/Framebuffer.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/Resources.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"

using namespace Bang;

PostProcessEffectBloom::PostProcessEffectBloom()
{
    CONSTRUCT_CLASS_ID(PostProcessEffectBloom);

    m_brightnessTexture = Resources::Create<Texture2D>();
    m_brightnessTexture.Get()->CreateEmpty(1, 1);
    m_brightnessTexture.Get()->SetFormat(GL::ColorFormat::RGBA32F);

    m_blurAuxiliarTexture = Resources::Create<Texture2D>();
    m_blurAuxiliarTexture.Get()->CreateEmpty(1, 1);
    m_blurAuxiliarTexture.Get()->SetFormat(GL::ColorFormat::RGBA32F);

    m_blurredBloomTexture = Resources::Create<Texture2D>();
    m_blurredBloomTexture.Get()->CreateEmpty(1, 1);
    m_blurredBloomTexture.Get()->SetFormat(GL::ColorFormat::RGBA32F);

    m_bloomFramebuffer = new Framebuffer();
    m_bloomFramebuffer->SetAttachmentTexture(m_brightnessTexture.Get(),
                                             GL::Attachment::COLOR0);

    p_bloomSP.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        ShaderProgramFactory::GetEngineShadersDir().Append("Bloom.frag")));
}

PostProcessEffectBloom::~PostProcessEffectBloom()
{
}

void PostProcessEffectBloom::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);

    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::BindTarget::SHADER_PROGRAM);

    if (MustBeRendered(renderPass))
    {
        ShaderProgram *sp = GetBloomShaderProgram();
        sp->Bind();

        Vector2i bloomTexSize = GL::GetViewportSize();

        GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        GL::Disable(GL::Enablable::BLEND);

        m_brightnessTexture.Get()->Resize(bloomTexSize);
        m_blurAuxiliarTexture.Get()->Resize(bloomTexSize);
        m_blurredBloomTexture.Get()->Resize(bloomTexSize);

        GEngine *ge = GEngine::GetInstance();

        m_bloomFramebuffer->Bind();
        m_bloomFramebuffer->SetAllDrawBuffers();

        // Extract bright pixels
        sp->SetBool("B_ExtractingBrightPixels", true);
        sp->SetFloat("B_Intensity", GetIntensity());
        sp->SetFloat("B_BrightnessThreshold", GetBrightnessThreshold());
        sp->SetTexture2D("B_SceneColorTexture",
                         ge->GetActiveGBuffer()->GetDrawColorTexture());
        sp->SetTexture2D(
            "B_SceneLightTexture",
            ge->GetActiveGBuffer()->GetAttachmentTex2D(GBuffer::AttLight));
        ge->GetActiveGBuffer()->BindAttachmentsForReading(sp);
        ge->RenderViewportPlane();

        if (GetBlurRadius() > 0)
        {
            ge->BlurTexture(m_brightnessTexture.Get(),
                            m_blurAuxiliarTexture.Get(),
                            m_blurredBloomTexture.Get(),
                            GetBlurRadius());
        }

        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        sp->SetBool("B_ExtractingBrightPixels", false);
        sp->SetTexture2D("B_BlurredBloomTexture", GetFinalBloomTexture());
        ge->GetActiveGBuffer()->ApplyPass(sp, true);
    }

    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    GL::Pop(GL::Pushable::BLEND_STATES);
    GL::Pop(GL::Pushable::VIEWPORT);
}

void PostProcessEffectBloom::SetIntensity(float intensity)
{
    m_intensity = intensity;
}

void PostProcessEffectBloom::SetBlurRadius(uint blurRadius)
{
    m_blurRadius = blurRadius;
}

void PostProcessEffectBloom::SetBrightnessThreshold(float brightnessThreshold)
{
    m_brightnessThreshold = brightnessThreshold;
}

float PostProcessEffectBloom::GetIntensity() const
{
    return m_intensity;
}

uint PostProcessEffectBloom::GetBlurRadius() const
{
    return m_blurRadius;
}

float PostProcessEffectBloom::GetBrightnessThreshold() const
{
    return m_brightnessThreshold;
}

Texture2D *PostProcessEffectBloom::GetFinalBloomTexture() const
{
    return GetBlurRadius() > 0 ? m_blurredBloomTexture.Get()
                               : m_brightnessTexture.Get();
}

ShaderProgram *PostProcessEffectBloom::GetBloomShaderProgram() const
{
    return p_bloomSP.Get();
}

void PostProcessEffectBloom::Reflect()
{
    Component::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectBloom,
                                   "Brightness threshold",
                                   SetBrightnessThreshold,
                                   GetBrightnessThreshold,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f) +
                                       BANG_REFLECT_HINT_STEP_VALUE(0.05f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectBloom,
                                   "Blur radius",
                                   SetBlurRadius,
                                   GetBlurRadius,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(
        PostProcessEffectBloom,
        "Intensity",
        SetIntensity,
        GetIntensity,
        BANG_REFLECT_HINT_MIN_VALUE(0.0f) + BANG_REFLECT_HINT_STEP_VALUE(0.5f));
}
