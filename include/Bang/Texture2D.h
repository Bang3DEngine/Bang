#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Bang/Asset.h"
#include "Bang/Image.h"
#include "Bang/Texture.h"

NAMESPACE_BANG_BEGIN

class Texture2D : public Texture
{
    ASSET(Texture2D)

public:
    Texture2D(const Texture2D &tex) = delete;

    // Texture
    using Texture::Resize;
    using Texture::CreateEmpty;
    void CreateEmpty(const Vector2i &size) override;
    void Resize(const Vector2i &size) override;
    void Fill(const Color &fillColor, int width, int height);
    void Fill(const Byte *newData,
              int width, int height,
              GL::ColorComp inputDataColorComp,
              GL::DataType inputDataType);

    template<class T = Byte>
    Image<T> ToImage() const;

    void SetAlphaCutoff(float alphaCutoff);
    float GetAlphaCutoff() const;

    void Import(const Image<Byte> &image);

    // GLObject
    GL::BindTarget GetGLBindTarget() const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

    // Resource
    virtual void Import(const Path &imageFilepath) override;

protected:
    float m_alphaCutoff = 0.1f;

    Texture2D();
    virtual ~Texture2D();

    void OnFormatChanged() override;
};

template<class T>
Image<T> Texture2D::ToImage() const
{
    return Texture::ToImage<T>(GL::TextureTarget::TEXTURE_2D);
}

NAMESPACE_BANG_END

#endif // TEXTURE2D_H
