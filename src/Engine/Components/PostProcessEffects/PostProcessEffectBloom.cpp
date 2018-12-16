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

    m_bloomFramebuffer = new Framebuffer();
    m_bloomFramebuffer->CreateAttachmentTex2D(GL::Attachment::COLOR0,
                                              GL::ColorFormat::RGBA32F);

    m_blurAuxiliarTexture = Resources::Create<Texture2D>();
    m_blurAuxiliarTexture.Get()->CreateEmpty(1, 1);
    m_blurAuxiliarTexture.Get()->SetFormat(GL::ColorFormat::RGBA32F);

    m_blurredBloomTexture = Resources::Create<Texture2D>();
    m_blurredBloomTexture.Get()->CreateEmpty(1, 1);
    m_blurredBloomTexture.Get()->SetFormat(GL::ColorFormat::RGBA32F);

    p_bloomSP.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        ShaderProgramFactory::GetEngineShadersDir().Append("Bloom.frag")));
}

PostProcessEffectBloom::~PostProcessEffectBloom()
{
}

#include "Bang/Input.h"
void PostProcessEffectBloom::OnRender(RenderPass renderPass)
{
    PostProcessEffect::OnRender(renderPass);

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

        m_bloomFramebuffer->Resize(bloomTexSize);

        m_bloomFramebuffer->Bind();
        m_bloomFramebuffer->SetAllDrawBuffers();
        GL::ClearColorBuffer(Color::Zero());

        // Extract bright pixels
        GEngine *ge = GEngine::GetInstance();
        sp->SetBool("B_ExtractingBrightPixels", true);
        sp->SetFloat("B_Intensity", GetIntensity());
        sp->SetFloat("B_BrightnessThreshold", GetBrightnessThreshold());
        ge->GetActiveGBuffer()->BindAttachmentsForReading(sp);
        ge->RenderViewportPlane();

        ge->BlurTexture(
            m_bloomFramebuffer->GetAttachmentTex2D(GL::Attachment::COLOR0),
            m_blurAuxiliarTexture.Get(),
            m_blurredBloomTexture.Get(),
            GetBlurRadius(),
            true,
            true);

        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        sp->SetBool("B_ExtractingBrightPixels", false);
        sp->SetTexture2D("B_SceneTexture",
                         ge->GetActiveGBuffer()->GetDrawColorTexture());
        sp->SetTexture2D("B_BlurredBloomTexture", m_blurredBloomTexture.Get());
        ge->GetActiveGBuffer()->ApplyPass(sp, true);

        if (Input::GetKeyDown(Key::Z))
        {
            m_bloomFramebuffer->GetAttachmentTex2D(GL::Attachment::COLOR0)
                ->ToImage()
                .Export(Path("bloomFB.png"));
            m_blurAuxiliarTexture.Get()->ToImage().Export(Path("aux.png"));
            m_blurredBloomTexture.Get()->ToImage().Export(Path("bBloom.png"));
        }
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
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectBloom,
                                   "Blur radius",
                                   SetBlurRadius,
                                   GetBlurRadius,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectBloom,
                                   "Intensity",
                                   SetIntensity,
                                   GetIntensity,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));
}
