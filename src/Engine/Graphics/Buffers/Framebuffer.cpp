#include "Bang/Framebuffer.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include "Bang/GL.h"
#include "Bang/Math.h"
#include "Bang/Vector2.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureCubeMap.h"

USING_NAMESPACE_BANG

Framebuffer::Framebuffer() : Framebuffer(1,1)
{
}

Framebuffer::Framebuffer(int width, int height) : m_width(width),
                                                  m_height(height)
{
    GL::GenFramebuffers(1, &m_idGL);
}

Framebuffer::~Framebuffer()
{
    GL::DeleteFramebuffers(1, &m_idGL);
}

template<class TextureClass>
void CreateEmptyTexture(TextureClass *tex, int width, int height)
{
    ASSERT(false);
}

template<>
void CreateEmptyTexture<Texture2D>(Texture2D *tex, int width, int height)
{
    tex->CreateEmpty(width, height);
}
template<>
void CreateEmptyTexture<TextureCubeMap>(TextureCubeMap *tex, int width, int height)
{
    tex->CreateEmpty(Math::Min(width, height));
}


template<class TextureClass>
void CreateAttachment(Framebuffer *fb,
                      GL::Attachment attachment,
                      GL::ColorFormat texFormat)
{
    GL::Push(GL::BindTarget::FRAMEBUFFER);

    fb->Bind();

    RH<TextureClass> tex = Resources::Create<TextureClass>();
    tex.Get()->Bind();
    tex.Get()->SetFormat(texFormat);
    CreateEmptyTexture<TextureClass>(tex.Get(), fb->GetWidth(), fb->GetHeight());

    fb->SetAttachmentTexture(tex.Get(), attachment);
    // tex.Get()->UnBind();

    GL::Pop(GL::BindTarget::FRAMEBUFFER);
}

void Framebuffer::CreateAttachmentTex2D(GL::Attachment attachment,
                                        GL::ColorFormat texFormat)
{
    CreateAttachment<Texture2D>(this, attachment, texFormat);
}

void Framebuffer::CreateAttachmentTexCubeMap(GL::Attachment attachment,
                                             GL::ColorFormat texFormat)
{
    CreateAttachment<TextureCubeMap>(this, attachment, texFormat);
}

Texture2D* Framebuffer::GetAttachmentTex2D(GL::Attachment attachment) const
{
    if (!m_attachments_To_Texture.ContainsKey(attachment)) { return nullptr; }
    return DCAST<Texture2D*>( m_attachments_To_Texture.Get(attachment).Get() );
}
TextureCubeMap* Framebuffer::GetAttachmentTexCubeMap(GL::Attachment attachment) const
{
    if (!m_attachments_To_Texture.ContainsKey(attachment)) { return nullptr; }
    return DCAST<TextureCubeMap*>( m_attachments_To_Texture.Get(attachment).Get() );
}

void Framebuffer::SetAllDrawBuffers() const
{
    Array<GL::Attachment> colorAtts = m_attachments;
    colorAtts.Remove(GL::Attachment::DEPTH_STENCIL);
    colorAtts.Remove(GL::Attachment::DEPTH);
    SetDrawBuffers(colorAtts);
}

void Framebuffer::SetDrawBuffers(const Array<GL::Attachment> &attachments) const
{
    ASSERT(GL::GetBoundId(GL::BindTarget::DRAW_FRAMEBUFFER) == GetGLId());
    GL::DrawBuffers(attachments);
    m_currentDrawAttachments = attachments;
}

void Framebuffer::SetReadBuffer(GL::Attachment attachment) const
{
    ASSERT(GL::GetBoundId(GL::BindTarget::READ_FRAMEBUFFER) == GetGLId());
    GL::ReadBuffer(attachment);
    m_currentReadAttachment = attachment;
}

void Framebuffer::BeforeSetAttTex(Texture* tex, GL::Attachment attachment)
{
    Bind();
    m_attachments.Remove(attachment);
    m_attachments_To_Texture.Remove(attachment);
}
void Framebuffer::AfterSetAttTex(Texture* tex, GL::Attachment attachment)
{
    GL::CheckFramebufferError();

    RH<Texture> texRH(tex);
    m_attachments.PushBack(attachment);
    m_attachments_To_Texture.Add(attachment, texRH);
}


void Framebuffer::SetAttachmentTexture(Texture* tex, GL::Attachment attachment,
                                       uint mipMapLevel)
{
    GL::Push(GL::BindTarget::FRAMEBUFFER);

    BeforeSetAttTex(tex, attachment);
    GL::FramebufferTexture(GL::FramebufferTarget::READ_DRAW,
                           attachment,
                           tex->GetGLId(),
                           mipMapLevel);
    AfterSetAttTex(tex, attachment);

    GL::Pop(GL::BindTarget::FRAMEBUFFER);
}
void Framebuffer::SetAttachmentTexture2D(Texture *tex,
                                         GL::TextureTarget texTarget,
                                         GL::Attachment attachment,
                                         uint mipMapLevel)
{
    GL::Push(GL::BindTarget::FRAMEBUFFER);

    BeforeSetAttTex(tex, attachment);
    GL::FramebufferTexture2D(GL::FramebufferTarget::READ_DRAW,
                             attachment,
                             texTarget,
                             tex->GetGLId(),
                             mipMapLevel);
    AfterSetAttTex(tex, attachment);

    GL::Pop(GL::BindTarget::FRAMEBUFFER);
}

void Framebuffer::Blit(GL::Attachment srcAttachment,
                       GL::Attachment dstAttachment,
                       const AARect &readNDCRect,
                       GL::BufferBit bufferBit)
{
    GL::Push(GL::BindTarget::DRAW_FRAMEBUFFER);
    GL::Push(GL::BindTarget::READ_FRAMEBUFFER);

    Bind();
    PushDrawAttachments();
    SetReadBuffer(srcAttachment);
    SetDrawBuffers({dstAttachment});
    AARect rf ( readNDCRect * 0.5f + 0.5f );
    AARecti r ( AARect(Vector2::Floor(rf.GetMin()),
                       Vector2::Ceil(rf.GetMax())) * GetSize() );
    GL::BlitFramebuffer(r, r, GL::FilterMode::NEAREST, bufferBit);
    PopDrawAttachments();

    GL::Pop(GL::BindTarget::READ_FRAMEBUFFER);
    GL::Pop(GL::BindTarget::DRAW_FRAMEBUFFER);
}

GL::Attachment Framebuffer::GetCurrentReadAttachment() const
{
    return m_currentReadAttachment;
}

const Array<GL::Attachment>& Framebuffer::GetCurrentDrawAttachments() const
{
    return m_currentDrawAttachments;
}

Color Framebuffer::ReadColor(int x, int y, GL::Attachment attachment) const
{
    GL::Push(GL::BindTarget::FRAMEBUFFER);

    Bind();
    Texture2D* t = GetAttachmentTex2D(attachment);
    SetReadBuffer(attachment);
    Byte color[4] = {0,0,0,0};
    GL::ReadPixels(x, y, 1, 1,
                   GL::GetColorCompFrom(t->GetFormat()),
                   t->GetDataType(),
                   Cast<void*>(&color));

    GL::Pop(GL::BindTarget::FRAMEBUFFER);

    Color readColor = Color(color[0], color[1], color[2], color[3]) / 255.0f;
    return readColor;
}


void Framebuffer::Resize(int width, int height)
{
    m_width  = Math::Max(width,  1);
    m_height = Math::Max(height, 1);

    for (const auto &it : m_attachments_To_Texture)
    {
        Texture *t = it.second.Get();
        if (t) { t->Resize(m_width, m_height); }
    }
}

int Framebuffer::GetWidth() const
{
    return m_width;
}

int Framebuffer::GetHeight() const
{
    return m_height;
}

Vector2 Framebuffer::GetSize() const
{
    return Vector2(GetWidth(), GetHeight());
}

GL::BindTarget Framebuffer::GetGLBindTarget() const
{
    return GL::BindTarget::FRAMEBUFFER;
}

void Framebuffer::Bind() const
{
    GL::Bind(this);
}
void Framebuffer::UnBind() const
{
    GL::UnBind(this);
}

void Framebuffer::Export(GL::Attachment attachment,
                         const Path &filepath,
                         bool invertY) const
{
    ASSERT(GL::IsBound(this));

    GL::Flush(); GL::Finish();

    Imageb img = GetAttachmentTex2D(attachment)->ToImage();
    if (invertY) { img = img.InvertedVertically(); }
    img.Export(filepath);
}

template<class T>
void ExportDepthOrStencil(const Framebuffer *fb,
                          const Path &filepath,
                          bool depth,
                          int multiplier)
{
    ASSERT(GL::IsBound(fb));

    GL::Flush(); GL::Finish();

    T *data = new T[fb->GetWidth() * fb->GetHeight()];
    GL::ReadPixels(0, 0, fb->GetWidth(), fb->GetHeight(),
                   (depth ? GL::ColorComp::DEPTH : GL::ColorComp::STENCIL_INDEX),
                   (depth ? GL::DataType::FLOAT : GL::DataType::UNSIGNED_BYTE),
                   data);

    Array<Byte> bytes(fb->GetWidth() * fb->GetHeight() * 4);
    for (int i = 0; i < fb->GetWidth() * fb->GetHeight(); ++i)
    {
        bytes[i * 4 + 0] = Byte(data[i] * multiplier);
        bytes[i * 4 + 1] = Byte(data[i] * multiplier);
        bytes[i * 4 + 2] = Byte(data[i] * multiplier);
        bytes[i * 4 + 3] = Byte(255);
    }

    Imageb img = Imageb::LoadFromData(fb->GetWidth(), fb->GetHeight(), bytes);
    img = img.InvertedVertically();
    img.Export(filepath);

    delete[] data;
}

void Framebuffer::ExportDepth(const Path &filepath) const
{
    ExportDepthOrStencil<float>(this, filepath, true, 255);
}

void Framebuffer::ExportStencil(const Path &filepath,
                                int stencilValueMultiplier) const
{
    ExportDepthOrStencil<Byte>(this, filepath, false, stencilValueMultiplier);
}

void Framebuffer::PushDrawAttachments()
{
    m_latestDrawAttachments = m_currentDrawAttachments;
}

void Framebuffer::PopDrawAttachments()
{
    SetDrawBuffers(m_latestDrawAttachments);
}
