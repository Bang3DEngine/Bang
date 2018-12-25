#include "Bang/GBuffer.h"

#include <memory>

#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/ShaderProgram.h"
#include "Bang/Texture2D.h"

using namespace Bang;

const GL::Attachment GBuffer::AttColor = GL::Attachment::COLOR0;
const GL::Attachment GBuffer::AttAlbedo = GL::Attachment::COLOR1;
const GL::Attachment GBuffer::AttLight = GL::Attachment::COLOR2;
const GL::Attachment GBuffer::AttNormal = GL::Attachment::COLOR3;
const GL::Attachment GBuffer::AttMisc = GL::Attachment::COLOR4;
const GL::Attachment GBuffer::AttDepthStencil = GL::Attachment::DEPTH_STENCIL;

GBuffer::GBuffer(int width, int height) : Framebuffer(width, height)
{
    // Create depth textures
    m_sceneDepthStencilTexture = Assets::Create<Texture2D>();
    m_canvasDepthStencilTexture = Assets::Create<Texture2D>();
    m_overlayDepthStencilTexture = Assets::Create<Texture2D>();
    auto depthStencilTexs = {m_sceneDepthStencilTexture.Get(),
                             m_canvasDepthStencilTexture.Get(),
                             m_overlayDepthStencilTexture.Get()};
    for (Texture2D *depthStencilTex : depthStencilTexs)
    {
        depthStencilTex->Bind();
        depthStencilTex->SetFormat(GL::ColorFormat::DEPTH24_STENCIL8);
        depthStencilTex->CreateEmpty(width, height);
    }

    // Create color textures
    m_colorTexture0 = Assets::Create<Texture2D>();
    m_colorTexture1 = Assets::Create<Texture2D>();
    p_drawColorTexture = GetColorTexture0();
    p_readColorTexture = GetColorTexture1();

    // Create attachments
    Bind();
    SetAttachmentTexture(GetColorTexture0(), GBuffer::AttColor);
    CreateAttachmentTex2D(GBuffer::AttAlbedo, GL::ColorFormat::RGBA8);
    CreateAttachmentTex2D(GBuffer::AttLight, GL::ColorFormat::RGBA16F);
    CreateAttachmentTex2D(GBuffer::AttNormal, GL::ColorFormat::RGB10_A2);
    CreateAttachmentTex2D(GBuffer::AttMisc, GL::ColorFormat::RGB10_A2);
    SetSceneDepthStencil();
    UnBind();

    SetHDR(true);
}

GBuffer::~GBuffer()
{
}

void GBuffer::BindAttachmentsForReading(ShaderProgram *sp)
{
    ASSERT(GL::IsBound(sp));

    sp->SetTexture2D(GBuffer::GetNormalsTexName(),
                     GetAttachmentTex2D(GBuffer::AttNormal),
                     false);
    sp->SetTexture2D(GBuffer::GetAlbedoTexName(),
                     GetAttachmentTex2D(GBuffer::AttAlbedo),
                     false);
    sp->SetTexture2D(GBuffer::GetLightTexName(),
                     GetAttachmentTex2D(GBuffer::AttLight),
                     false);
    sp->SetTexture2D(
        GBuffer::GetMiscTexName(), GetAttachmentTex2D(GBuffer::AttMisc), false);
    sp->SetTexture2D(GBuffer::GetColorsTexName(), GetReadColorTexture(), false);
    sp->SetTexture2D(GBuffer::GetDepthStencilTexName(),
                     GetAttachmentTex2D(GBuffer::AttDepthStencil),
                     false);
}

void GBuffer::ApplyPass_(ShaderProgram *sp, const AARect &mask)
{
    GL::Push(GL::Pushable::CULL_FACE);
    GL::Push(GL::Pushable::STENCIL_STATES);
    PushDrawAttachments();

    GL::SetStencilOp(GL::StencilOperation::KEEP);  // Dont modify stencil
    GL::Disable(GL::Enablable::CULL_FACE);

    BindAttachmentsForReading(sp);

    SetColorDrawBuffer();
    GEngine::GetInstance()->RenderViewportRect(sp, mask);  // Render rect!

    PopDrawAttachments();
    GL::Pop(GL::Pushable::STENCIL_STATES);
    GL::Pop(GL::Pushable::CULL_FACE);
}

void GBuffer::ApplyPassBlend(ShaderProgram *sp,
                             GL::BlendFactor srcBlendFactor,
                             GL::BlendFactor dstBlendFactor,
                             const AARect &mask)
{
    GL::Push(GL::Pushable::BLEND_STATES);

    GL::Enable(GL::Enablable::BLEND);
    GL::BlendFunc(srcBlendFactor, dstBlendFactor);
    ApplyPass_(sp, mask);

    GL::Pop(GL::Pushable::BLEND_STATES);
}

void GBuffer::ApplyPass(ShaderProgram *sp,
                        bool willReadFromColor,
                        const AARect &mask)
{
    ASSERT(GL::IsBound(this));
    ASSERT(GL::IsBound(sp));

    // Ping pong
    if (willReadFromColor)
    {
        PingPongColorBuffers();
    }

    GL::Push(GL::Pushable::BLEND_STATES);

    GL::Disable(GL::Enablable::BLEND);
    ApplyPass_(sp, mask);

    GL::Pop(GL::Pushable::BLEND_STATES);
}

bool GBuffer::Resize(const Vector2i &size)
{
    bool resized = false;
    resized |= Framebuffer::Resize(size);
    resized |= GetColorTexture0()->Resize(size);
    resized |= GetColorTexture1()->Resize(size);
    return resized;
}

void GBuffer::SetAllDrawBuffers() const
{
    SetDrawBuffers({{GBuffer::AttColor,
                     GBuffer::AttAlbedo,
                     GBuffer::AttLight,
                     GBuffer::AttNormal,
                     GBuffer::AttMisc}});
}

void GBuffer::SetAllDrawBuffersExceptColor()
{
    SetDrawBuffers({{GBuffer::AttAlbedo,
                     GBuffer::AttLight,
                     GBuffer::AttNormal,
                     GBuffer::AttMisc}});
}

void GBuffer::SetColorDrawBuffer()
{
    SetDrawBuffers({GBuffer::AttColor});
}

void GBuffer::SetLightDrawBuffer()
{
    SetDrawBuffers({GBuffer::AttLight});
}

void GBuffer::SetHDR(bool hdr)
{
    m_hdr = hdr;

    GL::ColorFormat colorFormat =
        (GetHDR() ? GL::ColorFormat::RGBA16F : GL::ColorFormat::RGBA8);
    GetColorTexture0()->SetFormat(colorFormat);
    GetColorTexture1()->SetFormat(colorFormat);
    GetAttachmentTex2D(GBuffer::AttLight)->SetFormat(colorFormat);
}

bool GBuffer::GetHDR() const
{
    return m_hdr;
}

void GBuffer::SetDepthStencilTexture(Texture2D *depthStencilTexture)
{
    if (depthStencilTexture != GetDepthStencilTexture())
    {
        SetAttachmentTexture(depthStencilTexture, GBuffer::AttDepthStencil);
        if (GetWidth() > 0 && GetHeight() > 0)
        {
            Resize(Vector2i(GetWidth(), GetHeight()));
        }
        p_currentDepthStencilTexture.Set(depthStencilTexture);
    }
}

Texture2D *GBuffer::GetSceneDepthStencilTexture() const
{
    return m_sceneDepthStencilTexture.Get();
}

Texture2D *GBuffer::GetCanvasDepthStencilTexture() const
{
    return m_canvasDepthStencilTexture.Get();
}

Texture2D *GBuffer::GetOverlayDepthStencilTexture() const
{
    return m_overlayDepthStencilTexture.Get();
}
void GBuffer::SetSceneDepthStencil()
{
    SetDepthStencilTexture(GetSceneDepthStencilTexture());
}
void GBuffer::SetCanvasDepthStencil()
{
    SetDepthStencilTexture(GetCanvasDepthStencilTexture());
}
void GBuffer::SetOverlayDepthStencil()
{
    SetDepthStencilTexture(GetOverlayDepthStencilTexture());
}
void GBuffer::PushDepthStencilTexture()
{
    m_depthStencilTexturesStack.push(AH<Texture2D>(GetDepthStencilTexture()));
}
void GBuffer::PopDepthStencilTexture()
{
    ASSERT(!m_depthStencilTexturesStack.empty());
    SetDepthStencilTexture(m_depthStencilTexturesStack.top().Get());
    m_depthStencilTexturesStack.pop();
}
Texture2D *GBuffer::GetDepthStencilTexture() const
{
    return p_currentDepthStencilTexture.Get();
}

Texture2D *GBuffer::GetColorTexture0() const
{
    return m_colorTexture0.Get();
}

Texture2D *GBuffer::GetColorTexture1() const
{
    return m_colorTexture1.Get();
}

Texture2D *GBuffer::GetDrawColorTexture() const
{
    return p_drawColorTexture;
}

Texture2D *GBuffer::GetReadColorTexture() const
{
    return p_readColorTexture;
}

String GBuffer::GetMiscTexName()
{
    return "B_GTex_Misc";
}

String GBuffer::GetLightTexName()
{
    return "B_GTex_Light";
}
String GBuffer::GetColorsTexName()
{
    return "B_GTex_Color";
}
String GBuffer::GetNormalsTexName()
{
    return "B_GTex_Normal";
}
String GBuffer::GetAlbedoTexName()
{
    return "B_GTex_AlbedoColor";
}
String GBuffer::GetDepthStencilTexName()
{
    return "B_GTex_DepthStencil";
}

void GBuffer::PingPongColorBuffers()
{
    std::swap(p_readColorTexture, p_drawColorTexture);

    SetAttachmentTexture(GetDrawColorTexture(), GBuffer::AttColor);
    ASSERT(GetDrawColorTexture() != GetReadColorTexture());
}
