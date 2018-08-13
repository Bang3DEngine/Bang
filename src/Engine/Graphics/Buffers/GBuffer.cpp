#include "Bang/GBuffer.h"

#include "Bang/GL.h"
#include "Bang/Math.h"
#include "Bang/Color.h"
#include "Bang/AARect.h"
#include "Bang/GEngine.h"
#include "Bang/Texture2D.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

const GL::Attachment GBuffer::AttColor0       = GL::Attachment::COLOR0;
const GL::Attachment GBuffer::AttColor1       = GL::Attachment::COLOR1;
const GL::Attachment GBuffer::AttAlbedo       = GL::Attachment::COLOR2;
const GL::Attachment GBuffer::AttNormal       = GL::Attachment::COLOR3;
const GL::Attachment GBuffer::AttMisc         = GL::Attachment::COLOR4;
const GL::Attachment GBuffer::AttDepthStencil = GL::Attachment::DEPTH_STENCIL;

GBuffer::GBuffer(int width, int height) : Framebuffer(width, height)
{
    // Create depth textures
    m_sceneDepthStencilTexture   = Resources::Create<Texture2D>();
    m_canvasDepthStencilTexture  = Resources::Create<Texture2D>();
    m_overlayDepthStencilTexture = Resources::Create<Texture2D>();
    auto depthStencilTexs = {m_sceneDepthStencilTexture.Get(),
                             m_canvasDepthStencilTexture.Get(),
                             m_overlayDepthStencilTexture.Get()};
    for (Texture2D *depthStencilTex : depthStencilTexs)
    {
        depthStencilTex->Bind();
        depthStencilTex->SetFormat(GL::ColorFormat::DEPTH24_STENCIL8);
        depthStencilTex->CreateEmpty(width, height);
    }

    // Create attachments
    Bind();
    CreateAttachmentTex2D(GBuffer::AttColor0,  GL::ColorFormat::RGBA8);
    CreateAttachmentTex2D(GBuffer::AttColor1,  GL::ColorFormat::RGBA8);
    CreateAttachmentTex2D(GBuffer::AttAlbedo,  GL::ColorFormat::RGBA8);
    CreateAttachmentTex2D(GBuffer::AttNormal,  GL::ColorFormat::RGB10_A2);
    CreateAttachmentTex2D(GBuffer::AttMisc,    GL::ColorFormat::RGB10_A2);
    SetSceneDepthStencil();
    UnBind();

    m_drawColorAttachment = m_readColorAttachment = GBuffer::AttColor0;
}

GBuffer::~GBuffer()
{
}


void GBuffer::BindAttachmentsForReading(ShaderProgram *sp)
{
    ASSERT(GL::IsBound(sp));

    sp->SetTexture2D(GBuffer::GetNormalsTexName(),
                     GetAttachmentTex2D(GBuffer::AttNormal), false);
    sp->SetTexture2D(GBuffer::GetAlbedoTexName(),
                     GetAttachmentTex2D(GBuffer::AttAlbedo), false);
    sp->SetTexture2D(GBuffer::GetMiscTexName(),
                     GetAttachmentTex2D(GBuffer::AttMisc), false);
    sp->SetTexture2D(GBuffer::GetColorsTexName(),
                     GetAttachmentTex2D(m_readColorAttachment), false);
    sp->SetTexture2D(GBuffer::GetDepthStencilTexName(),
                     GetAttachmentTex2D(GBuffer::AttDepthStencil), false);
}

void GBuffer::ApplyPass_(ShaderProgram *sp, const AARect &mask)
{
    // Save state
    GL::Push(GL::Pushable::STENCIL_STATES);
    PushDrawAttachments();

    GL::SetStencilOp(GL::StencilOperation::KEEP); // Dont modify stencil

    BindAttachmentsForReading(sp);
    SetColorDrawBuffer();
    GEngine::GetInstance()->RenderViewportRect(sp, mask); // Render rect!

    PopDrawAttachments();
    GL::Pop(GL::Pushable::STENCIL_STATES);
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
        m_readColorAttachment = m_drawColorAttachment;
        m_drawColorAttachment = (m_readColorAttachment == GBuffer::AttColor0 ?
                                     GBuffer::AttColor1 : GBuffer::AttColor0);
        ASSERT(m_drawColorAttachment != m_readColorAttachment);
    }

    ApplyPass_(sp, mask);

    m_readColorAttachment = m_drawColorAttachment; // Read again from same buffer
}

void GBuffer::SetAllDrawBuffers() const
{
    SetDrawBuffers({{m_drawColorAttachment,
                     GBuffer::AttAlbedo,
                     GBuffer::AttNormal,
                     GBuffer::AttMisc
                   }});
}

void GBuffer::SetAllDrawBuffersExceptColor()
{
    SetDrawBuffers({{GBuffer::AttAlbedo,
                     GBuffer::AttNormal,
                     GBuffer::AttMisc}});
}

void GBuffer::SetColorDrawBuffer()
{
    SetDrawBuffers({m_drawColorAttachment});
}

void GBuffer::SetDepthStencilTexture(Texture2D *depthStencilTexture)
{
    if (depthStencilTexture != GetDepthStencilTexture())
    {
        SetAttachmentTexture(depthStencilTexture, GBuffer::AttDepthStencil);
        if (GetWidth() > 0 && GetHeight() > 0)
        {
            Resize( GetWidth(), GetHeight() );
        }
        p_currentDepthStencilTexture.Set(depthStencilTexture);
    }
}

GL::Attachment GBuffer::GetLastDrawnColorAttachment() const
{
    return m_drawColorAttachment;
}

Texture2D *GBuffer::GetLastDrawnColorTexture() const
{
    return GetAttachmentTex2D( GetLastDrawnColorAttachment() );
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
    SetDepthStencilTexture( GetSceneDepthStencilTexture() );
}
void GBuffer::SetCanvasDepthStencil()
{
    SetDepthStencilTexture( GetCanvasDepthStencilTexture() );
}
void GBuffer::SetOverlayDepthStencil()
{
    SetDepthStencilTexture( GetOverlayDepthStencilTexture() );
}
void GBuffer::PushDepthStencilTexture()
{
    m_depthStencilTexturesStack.push( RH<Texture2D>(GetDepthStencilTexture()) );
}
void GBuffer::PopDepthStencilTexture()
{
    ASSERT( !m_depthStencilTexturesStack.empty() );
    SetDepthStencilTexture( m_depthStencilTexturesStack.top().Get() );
    m_depthStencilTexturesStack.pop();
}
Texture2D *GBuffer::GetDepthStencilTexture() const
{
    return p_currentDepthStencilTexture.Get();
}

String GBuffer::GetMiscTexName() { return "B_GTex_Misc"; }
String GBuffer::GetColorsTexName() { return "B_GTex_Color"; }
String GBuffer::GetNormalsTexName() { return "B_GTex_Normal"; }
String GBuffer::GetAlbedoTexName() { return "B_GTex_AlbedoColor"; }
String GBuffer::GetDepthStencilTexName() { return "B_GTex_DepthStencil"; }
