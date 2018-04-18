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
    Bind();
    CreateAttachmentTex2D(AttColor,        GL::ColorFormat::RGBA_UByte8);
    CreateAttachmentTex2D(AttAlbedo,       GL::ColorFormat::RGBA_UByte8);
    CreateAttachmentTex2D(AttNormal,       GL::ColorFormat::RGB10_A2_UByte);
    CreateAttachmentTex2D(AttMisc,         GL::ColorFormat::RGB10_A2_UByte);
    CreateAttachmentTex2D(AttColorRead,    GL::ColorFormat::RGBA_UByte8);
    CreateAttachmentTex2D(AttDepthStencil, GL::ColorFormat::Depth24_Stencil8);
    UnBind();
}

GBuffer::~GBuffer()
{
}

void GBuffer::BindAttachmentsForReading(ShaderProgram *sp)
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
                     GetAttachmentTex2D(AttColorRead), false);
    sp->SetTexture2D(GBuffer::GetDepthStencilTexName(),
                     GetAttachmentTex2D(AttDepthStencil), false);
}


void GBuffer::ApplyPass(ShaderProgram *sp,
                        bool willReadFromColor,
                        const AARect &mask)
{
    if (!sp) { return; }
    ASSERT(GL::IsBound(this));
    ASSERT(GL::IsBound(sp));

    // Save state
    GL::StencilOperation prevStencilOp = GL::GetStencilOp();
    PushDrawAttachments();

    // Set state
    GL::SetStencilOp(GL::StencilOperation::Keep); // Dont modify stencil
    if (willReadFromColor) { PrepareColorReadBuffer(mask); }
    BindAttachmentsForReading(sp);
    SetColorDrawBuffer();

    GEngine::GetActive()->RenderViewportRect(sp, mask);

    // Restore state
    PopDrawAttachments();
    GL::SetStencilOp(prevStencilOp);
}

void GBuffer::PrepareColorReadBuffer(const AARect &readNDCRect)
{
    Blit(GBuffer::AttColor, {GBuffer::AttColorRead},
         readNDCRect, GL::BufferBit::Color);
}

void GBuffer::SetAllDrawBuffers() const
{
    SetDrawBuffers({GBuffer::AttColor,  GBuffer::AttAlbedo,
                    GBuffer::AttNormal, GBuffer::AttMisc
                   });
}

void GBuffer::SetAllDrawBuffersExceptColor()
{
    SetDrawBuffers({GBuffer::AttAlbedo, GBuffer::AttNormal, GBuffer::AttMisc});
}

void GBuffer::SetColorDrawBuffer()
{
    SetDrawBuffers({GBuffer::AttColor});
}

void GBuffer::ClearAllBuffersExceptColor()
{
    GL::ClearStencilBuffer(0);
    ClearDepth(1.0f);

    SetDrawBuffers({GBuffer::AttNormal, GBuffer::AttAlbedo, GBuffer::AttMisc});
    GL::ClearColorBuffer(Color::Zero);
}

void GBuffer::ClearBuffersAndBackground(const Color &backgroundColor)
{
    ClearAllBuffersExceptColor();
    SetDrawBuffers({GBuffer::AttColor}); // , GBuffer::AttColorRead});
    GL::ClearColorBuffer(backgroundColor);
}

String GBuffer::GetMiscTexName() { return "B_GTex_Misc"; }
String GBuffer::GetColorsTexName() { return "B_GTex_Color"; }
String GBuffer::GetNormalsTexName() { return "B_GTex_Normal"; }
String GBuffer::GetAlbedoTexName() { return "B_GTex_AlbedoColor"; }
String GBuffer::GetDepthStencilTexName() { return "B_GTex_DepthStencil"; }
