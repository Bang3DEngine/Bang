#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/Image.h"
#include "Bang/ImageIO.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "Bang/Texture.h"

NAMESPACE_BANG_BEGIN

FORWARD class Color;
FORWARD class Path;

class Texture2D : public Texture
{
    ASSET(Texture2D)

public:
    Texture2D(const Texture2D &tex) = delete;

    // Texture
    using Texture::Resize;
    using Texture::CreateEmpty;
    void CreateEmpty(const Vector2i &size) override;
    bool Resize(const Vector2i &size) override;
    void Fill(const Color &fillColor, int width, int height);
    void Fill(const Byte *newData,
              int width, int height,
              GL::ColorComp inputDataColorComp,
              GL::DataType inputDataType);

    template<class T = Byte>
    Image<T> ToImage();

    void SetAlphaCutoff(float alphaCutoff);

    float GetAlphaCutoff() const;
    const Imageb& GetImage() const;

    void Import(const Image<Byte> &image);

    // GLObject
    GL::BindTarget GetGLBindTarget() const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // Resource
    virtual void Import(const Path &imageFilepath) override;

protected:
    Imageb m_image;
    float m_alphaCutoff = 0.0f;

    Texture2D();
    virtual ~Texture2D();

    void OnFormatChanged() override;
};

template<class T>
Image<T> Texture2D::ToImage()
{
    return Texture::ToImage<T>(GL::TextureTarget::TEXTURE_2D);
}

NAMESPACE_BANG_END

#endif // TEXTURE2D_H
