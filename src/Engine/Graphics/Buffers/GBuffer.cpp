#include "Bang/GBuffer.h"

#include "Bang/GL.h"
#include "Bang/Math.h"
#include "Bang/Color.h"
#include "Bang/AARect.h"
#include "Bang/GEngine.h"
#include "Bang/Texture2D.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

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
        depthStencilTex->SetFormat(GL::ColorFormat::Depth24_Stencil8);
        depthStencilTex->CreateEmpty(width, height);
    }

    // Create attachments
    Bind();
    CreateAttachmentTex2D(AttColor,        GL::ColorFormat::RGBA_UByte8);
    CreateAttachmentTex2D(AttAlbedo,       GL::ColorFormat::RGBA_UByte8);
    CreateAttachmentTex2D(AttNormal,       GL::ColorFormat::RGB10_A2_UByte);
    CreateAttachmentTex2D(AttMisc,         GL::ColorFormat::RGB10_A2_UByte);
    CreateAttachmentTex2D(AttColorRead,    GL::ColorFormat::RGBA_UByte8);
    SetSceneDepthStencil();
    UnBind();
}

GBuffer::~GBuffer()
{
}


void GBuffer::BindAttachmentsForReading(ShaderProgram *sp,
                                        bool readFromCopiedColor)
{
    if (!sp) { return; }
    ASSERT(GL::IsBound(sp));

    sp->SetTexture2D(GBuffer::GetNormalsTexName(),
                     GetAttachmentTex2D(AttNormal), false);
    sp->SetTexture2D(GBuffer::GetAlbedoTexName(),
                     GetAttachmentTex2D(AttAlbedo), false);
    sp->SetTexture2D(GBuffer::GetMiscTexName(),
                     GetAttachmentTex2D(AttMisc), false);
    sp->SetTexture2D(GBuffer::GetColorsTexName(),
                     GetAttachmentTex2D(
                         readFromCopiedColor ? AttColorRead : AttColor), false);
    sp->SetTexture2D(GBuffer::GetDepthStencilTexName(),
                     GetAttachmentTex2D(AttDepthStencil), false);
}

void GBuffer::ApplyPass_(ShaderProgram *sp, const AARect &mask)
{
    // Save state
    GL::StencilOperation prevStencilOp = GL::GetStencilOp();
    PushDrawAttachments();

    GL::SetStencilOp(GL::StencilOperation::Keep); // Dont modify stencil
    BindAttachmentsForReading(sp);
    SetColorDrawBuffer();
    GEngine::GetActive()->RenderViewportRect(sp, mask); // Render rect!

    // Restore state
    PopDrawAttachments();
    GL::SetStencilOp(prevStencilOp);
}

void GBuffer::ApplyPassBlend(ShaderProgram *sp,
                             GL::BlendFactor srcBlendFactor,
                             GL::BlendFactor dstBlendFactor,
                             const AARect &mask)
{
    // Save previous state
    GL::BlendFactor prevBlendSrcFactorColor = GL::GetBlendSrcFactorColor();
    GL::BlendFactor prevBlendDstFactorColor = GL::GetBlendDstFactorColor();
    GL::BlendFactor prevBlendSrcFactorAlpha = GL::GetBlendSrcFactorAlpha();
    GL::BlendFactor prevBlendDstFactorAlpha = GL::GetBlendDstFactorAlpha();
    bool wasBlendEnabled                    = GL::IsEnabled(GL::Enablable::Blend);

    GL::Enable(GL::Enablable::Blend);
    GL::BlendFunc(srcBlendFactor, dstBlendFactor);
    ApplyPass_(sp, mask);

    // Restore gl state
    GL::BlendFuncSeparate(prevBlendSrcFactorColor,
                          prevBlendDstFactorColor,
                          prevBlendSrcFactorAlpha,
                          prevBlendDstFactorAlpha);
    GL::SetEnabled(GL::Enablable::Blend, wasBlendEnabled, false);
}


void GBuffer::ApplyPass(ShaderProgram *sp,
                        bool willReadFromColor,
                        const AARect &mask)
{
    if (!sp) { return; }
    ASSERT(GL::IsBound(this));
    ASSERT(GL::IsBound(sp));

    // Set state
    if (willReadFromColor) { PrepareColorReadBuffer(mask); }
    ApplyPass_(sp, mask);
}

void GBuffer::PrepareColorReadBuffer(const AARect &readNDCRect)
{
    Blit(GBuffer::AttColor, GBuffer::AttColorRead,
         readNDCRect, GL::BufferBit::Color);
}

void GBuffer::SetAllDrawBuffers() const
{
    SetDrawBuffers({{GBuffer::AttColor,  GBuffer::AttAlbedo,
                     GBuffer::AttNormal, GBuffer::AttMisc
                   }});
}

void GBuffer::SetAllDrawBuffersExceptColor()
{
    SetDrawBuffers({{GBuffer::AttAlbedo, GBuffer::AttNormal, GBuffer::AttMisc}});
}

void GBuffer::SetColorDrawBuffer()
{
    SetDrawBuffers({GBuffer::AttColor});
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
        p_currentDepthStencilTexture = depthStencilTexture;
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
    m_depthStencilTexturesStack.push( GetDepthStencilTexture() );
}
void GBuffer::PopDepthStencilTexture()
{
    ASSERT( !m_depthStencilTexturesStack.empty() );
    SetDepthStencilTexture( m_depthStencilTexturesStack.top().Get() );
    m_depthStencilTexturesStack.pop();
}
Texture2D *GBuffer::GetDepthStencilTexture() const
{
    return p_currentDepthStencilTexture;
}

String GBuffer::GetMiscTexName() { return "B_GTex_Misc"; }
String GBuffer::GetColorsTexName() { return "B_GTex_Color"; }
String GBuffer::GetNormalsTexName() { return "B_GTex_Normal"; }
String GBuffer::GetAlbedoTexName() { return "B_GTex_AlbedoColor"; }
String GBuffer::GetDepthStencilTexName() { return "B_GTex_DepthStencil"; }
