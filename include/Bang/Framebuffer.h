#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "Bang/AARect.h"
#include "Bang/Array.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"
#include "Bang/UMap.h"
#include "Bang/Vector2.h"

namespace Bang
{
template <class>
class AssetHandle;
class Path;
class Texture2D;
class Texture;
class TextureCubeMap;

class Framebuffer : public GLObject
{
public:
    Framebuffer();
    Framebuffer(int width, int height);
    virtual ~Framebuffer() override;

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
    void SetAttachmentTexture(GLId texId,
                              GL::Attachment attachment,
                              uint mipMapLevel = 0);
    void SetAttachmentTexture(Texture *tex,
                              GL::Attachment attachment,
                              GL::TextureTarget texTarget,
                              uint mipMapLevel = 0);

    void Blit(GL::Attachment srcAttachment,
              GL::Attachment dstAttachment,
              const AARect &ndcRect = AARect::NDCRect(),
              GL::BufferBit bufferBit = GL::BufferBit::COLOR);
    Color ReadColor(int x, int y, GL::Attachment attachment) const;
    virtual bool Resize(const Vector2i &size);
    bool Resize(int width, int height);

    int GetWidth() const;
    int GetHeight() const;
    const Vector2i &GetSize() const;
    GL::Attachment GetCurrentReadAttachment() const;
    const Array<GL::Attachment> &GetCurrentDrawAttachments() const;
    Texture2D *GetAttachmentTex2D(GL::Attachment attachment) const;
    TextureCubeMap *GetAttachmentTexCubeMap(GL::Attachment attachment) const;
    GL::BindTarget GetGLBindTarget() const override;

    virtual void Bind() const override;
    virtual void UnBind() const override;

    void PushDrawAttachments();
    void PopDrawAttachments();

    void Export(GL::Attachment attachmentId,
                const Path &filepath,
                bool invertY = true) const;
    void ExportDepth(const Path &filepath) const;
    void ExportStencil(const Path &filepath,
                       int stencilValueMultiplier = 1) const;

protected:
    Vector2i m_size = Vector2i::Zero();
    Array<GL::Attachment> m_attachments;
    UMap<GL::Attachment, AH<Texture>, EnumClassHash> m_attachments_To_Texture;

private:
    mutable Array<GL::Attachment> m_currentDrawAttachments;
    mutable Array<GL::Attachment> m_latestDrawAttachments;
    mutable GL::Attachment m_currentReadAttachment;

    void BeforeSetAttTex(GL::Attachment attachment);
    void AfterSetAttTex(Texture *tex, GL::Attachment attachment);
};
}

#endif  // FRAMEBUFFER_H
