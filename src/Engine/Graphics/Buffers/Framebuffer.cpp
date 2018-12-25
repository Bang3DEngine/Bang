#include "Bang/Framebuffer.h"

#include <unordered_map>
#include <utility>
#include <vector>

#include "Bang/AARect.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/AssetHandle.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/GL.h"
#include "Bang/Image.h"
#include "Bang/ImageIO.h"
#include "Bang/Math.h"
#include "Bang/Texture.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureCubeMap.h"
#include "Bang/UMap.tcc"
#include "Bang/Vector2.h"

namespace Bang
{
class Path;
}

using namespace Bang;

Framebuffer::Framebuffer() : Framebuffer(1, 1)
{
}

Framebuffer::Framebuffer(int width, int height)
{
    m_size = Vector2i(width, height);
    GL::GenFramebuffers(1, &m_idGL);
}

Framebuffer::~Framebuffer()
{
    GL::DeleteFramebuffers(1, &m_idGL);
}

template <class TextureClass>
void CreateEmptyTexture(TextureClass *tex, int width, int height)
{
    ASSERT(false);
}

template <>
void CreateEmptyTexture<Texture2D>(Texture2D *tex, int width, int height)
{
    tex->CreateEmpty(width, height);
}
template <>
void CreateEmptyTexture<TextureCubeMap>(TextureCubeMap *tex,
                                        int width,
                                        int height)
{
    tex->CreateEmpty(Math::Min(width, height));
}

template <class TextureClass>
void CreateAttachment(Framebuffer *fb,
                      GL::Attachment attachment,
                      GL::ColorFormat texFormat)
{
    GL::Push(GL::BindTarget::FRAMEBUFFER);

    fb->Bind();

    AH<TextureClass> tex = Assets::Create<TextureClass>();
    tex.Get()->Bind();
    tex.Get()->SetFormat(texFormat);
    CreateEmptyTexture<TextureClass>(
        tex.Get(), fb->GetWidth(), fb->GetHeight());

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

Texture2D *Framebuffer::GetAttachmentTex2D(GL::Attachment attachment) const
{
    if (!m_attachments_To_Texture.ContainsKey(attachment))
    {
        return nullptr;
    }
    return DCAST<Texture2D *>(m_attachments_To_Texture.Get(attachment).Get());
}
TextureCubeMap *Framebuffer::GetAttachmentTexCubeMap(
    GL::Attachment attachment) const
{
    if (!m_attachments_To_Texture.ContainsKey(attachment))
    {
        return nullptr;
    }
    return DCAST<TextureCubeMap *>(
        m_attachments_To_Texture.Get(attachment).Get());
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

void Framebuffer::BeforeSetAttTex(GL::Attachment attachment)
{
    Bind();
    m_attachments.Remove(attachment);
    m_attachments_To_Texture.Remove(attachment);
}
void Framebuffer::AfterSetAttTex(Texture *tex, GL::Attachment attachment)
{
    ASSERT(GL::CheckFramebufferError());

    AH<Texture> texAH(tex);
    m_attachments.PushBack(attachment);
    m_attachments_To_Texture.Add(attachment, texAH);
}

void Framebuffer::SetAttachmentTexture(Texture *tex,
                                       GL::Attachment attachment,
                                       uint mipMapLevel)
{
    GL::Push(GL::BindTarget::FRAMEBUFFER);

    BeforeSetAttTex(attachment);
    GL::FramebufferTexture(GL::FramebufferTarget::READ_DRAW,
                           attachment,
                           tex->GetGLId(),
                           mipMapLevel);
    AfterSetAttTex(tex, attachment);

    GL::Pop(GL::BindTarget::FRAMEBUFFER);
}

void Framebuffer::SetAttachmentTexture(GLId texId,
                                       GL::Attachment attachment,
                                       uint mipMapLevel)
{
    GL::Push(GL::BindTarget::FRAMEBUFFER);

    BeforeSetAttTex(attachment);
    GL::FramebufferTexture(
        GL::FramebufferTarget::READ_DRAW, attachment, texId, mipMapLevel);

    ASSERT(GL::CheckFramebufferError());

    GL::Pop(GL::BindTarget::FRAMEBUFFER);
}

void Framebuffer::SetAttachmentTexture(Texture *tex,
                                       GL::Attachment attachment,
                                       GL::TextureTarget texTarget,
                                       uint mipMapLevel)
{
    GL::Push(GL::BindTarget::FRAMEBUFFER);

    BeforeSetAttTex(attachment);
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
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    Bind();
    PushDrawAttachments();
    SetReadBuffer(srcAttachment);
    SetDrawBuffers({dstAttachment});
    AARect rf(readNDCRect * 0.5f + 0.5f);
    AARecti r(Vector2i(Vector2::Floor(rf.GetMin())),
              Vector2i(Vector2::Ceil(rf.GetMax())) * GetSize());
    GL::BlitFramebuffer(r, r, GL::FilterMode::NEAREST, bufferBit);
    PopDrawAttachments();

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
}

GL::Attachment Framebuffer::GetCurrentReadAttachment() const
{
    return m_currentReadAttachment;
}

const Array<GL::Attachment> &Framebuffer::GetCurrentDrawAttachments() const
{
    return m_currentDrawAttachments;
}

Color Framebuffer::ReadColor(int x, int y, GL::Attachment attachment) const
{
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    Bind();
    Texture2D *t = GetAttachmentTex2D(attachment);
    SetReadBuffer(attachment);
    Byte color[4] = {0, 0, 0, 0};
    GL::ReadPixels(x,
                   y,
                   1,
                   1,
                   GL::GetColorCompFrom(t->GetFormat()),
                   GL::DataType::UNSIGNED_BYTE,
                   SCAST<void *>(&color));
    UnBind();

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    Color readColor = Color(color[0], color[1], color[2], color[3]) / 255.0f;
    return readColor;
}

bool Framebuffer::Resize(const Vector2i &size)
{
    m_size = Vector2i::Max(size, Vector2i::One());

    bool resized = false;
    for (const auto &it : m_attachments_To_Texture)
    {
        if (Texture2D *tex = DCAST<Texture2D *>(it.second.Get()))
        {
            resized |= tex->Resize(GetSize());
        }
        else if (TextureCubeMap *tex = DCAST<TextureCubeMap *>(it.second.Get()))
        {
            resized |= tex->Resize(GetSize().x);
        }
    }
    return resized;
}

bool Framebuffer::Resize(int width, int height)
{
    return Resize(Vector2i(width, height));
}

int Framebuffer::GetWidth() const
{
    return GetSize().x;
}

int Framebuffer::GetHeight() const
{
    return GetSize().y;
}

const Vector2i &Framebuffer::GetSize() const
{
    return m_size;
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

    GL::Flush();
    GL::Finish();

    Image img = GetAttachmentTex2D(attachment)->ToImage();
    if (invertY)
    {
        img = img.InvertedVertically();
    }
    img.Export(filepath);
}

template <class T>
void ExportDepthOrStencil(const Framebuffer *fb,
                          const Path &filepath,
                          bool depth,
                          int multiplier)
{
    ASSERT(GL::IsBound(fb));

    GL::Flush();
    GL::Finish();

    T *data = new T[fb->GetWidth() * fb->GetHeight()];
    GL::ReadPixels(
        0,
        0,
        fb->GetWidth(),
        fb->GetHeight(),
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

    Image img = Image::LoadFromData(fb->GetWidth(), fb->GetHeight(), bytes);
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
