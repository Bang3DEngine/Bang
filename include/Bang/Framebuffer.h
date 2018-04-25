#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Bang/Map.h"
#include "Bang/Array.h"
#include "Bang/Color.h"
#include "Bang/GLObject.h"
#include "Bang/Texture2D.h"

NAMESPACE_BANG_BEGIN

FORWARD class TextureCubeMap;

class Framebuffer : public GLObject
{
public:
    Framebuffer();
    Framebuffer(int width, int height);
    virtual ~Framebuffer();

    void CreateAttachmentTex2D(GL::Attachment attachment,
                               GL::ColorFormat texFormat);
    void CreateAttachmentTexCubeMap(GL::Attachment attachment,
                                    GL::ColorFormat texFormat);

    virtual void SetAllDrawBuffers() const;
    void SetDrawBuffers(const Array<GL::Attachment> &attachments) const;
    void SetReadBuffer(GL::Attachment attachment) const;
    void SetAttachmentTexture(Texture *tex,
                              GL::Attachment attachment,
                              uint mipMapLevel = 0);
    void SetAttachmentTexture2D(Texture *tex,
                                GL::TextureTarget texTarget,
                                GL::Attachment attachment,
                                uint mipMapLevel = 0);


    void Blit(GL::Attachment srcAttachment, GL::Attachment dstAttachment,
              const AARect &ndcRect = AARect::NDCRect,
              GL::BufferBit bufferBit = GL::BufferBit::Color);
    Color ReadColor(int x, int y, GL::Attachment attachment) const;
    void Resize(int width, int height);

    int GetWidth() const;
    int GetHeight() const;
    Vector2 GetSize() const;
    GL::Attachment GetCurrentReadAttachment() const;
    const Array<GL::Attachment>& GetCurrentDrawAttachments() const;
    Texture2D* GetAttachmentTex2D(GL::Attachment attachment) const;
    TextureCubeMap* GetAttachmentTexCubeMap(GL::Attachment attachment) const;
    GL::BindTarget GetGLBindTarget() const override;

    void Clear();
    virtual void ClearDepth(float clearDepth = 1.0f);
    void ClearColor(const Color &clearColor = Color::Zero);

    virtual void Bind() const override;
    virtual void UnBind() const override;

    void Export(GL::Attachment attachmentId, const Path &filepath,
                bool invertY = true) const;
    void ExportDepth(const Path &filepath) const;
    void ExportStencil(const Path &filepath,
                       int stencilValueMultiplier = 1) const;

protected:
    int m_width = 0;
    int m_height = 0;
    Array<GL::Attachment> m_attachments;
    Map<GL::Attachment, RH<Texture>> m_attachments_To_Texture;

    void PushDrawAttachments();
    void PopDrawAttachments();

private:
    mutable Array<GL::Attachment> m_currentDrawAttachments;
    mutable Array<GL::Attachment> m_latestDrawAttachments;
    mutable GL::Attachment m_currentReadAttachment;

    void BeforeSetAttTex(Texture* tex, GL::Attachment attachment);
    void AfterSetAttTex(Texture* tex, GL::Attachment attachment);
};

NAMESPACE_BANG_END

#endif // FRAMEBUFFER_H
