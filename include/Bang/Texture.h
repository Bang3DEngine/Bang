#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include "Bang/Asset.h"
#include "Bang/Image.h"
#include "Bang/Vector2.h"
#include "Bang/GLObject.h"
#include "Bang/EventEmitter.h"

NAMESPACE_BANG_BEGIN

class Texture : public GLObject,
                public Asset
{
    ASSET(Texture)

public:
    Texture();
    Texture(GL::TextureTarget texTarget);
    Texture(const Texture &t);
    virtual ~Texture();

    virtual void CreateEmpty(const Vector2i &size) = 0;
    virtual void Resize(const Vector2i &size) = 0;
    void CreateEmpty(int width, int height);
    void Resize(int width, int height);
    void GenerateMipMaps() const;

    void SetFormat(GL::ColorFormat internalFormat);
    void SetTarget(GL::TextureTarget target);
    void SetFilterMode(GL::FilterMode filterMode);
    void SetWrapMode(GL::WrapMode wrapMode);

    int GetWidth() const;
    int GetHeight() const;
    const Vector2i& GetSize() const;
    GL::FilterMode GetFilterMode() const;
    GL::WrapMode GetWrapMode() const;

    GL::DataType GetDataType() const;
    GL::ColorComp GetColorComp() const;
    GL::ColorFormat GetFormat() const;
    int GetNumComponents() const;
    uint GetBytesSize() const;

    GL::TextureTarget GetTextureTarget() const;

protected:
    void SetWidth(int width);
    void SetHeight(int height);

    static Color GetColorFromFloatArray(const float *pixels, int i);
    static Color GetColorFromByteArray(const Byte *pixels, int i);

    template<class T>
    Image<T> ToImage(GL::TextureTarget texTarget) const;

    virtual void OnFormatChanged();

private:
    Vector2i m_size = Vector2i::Zero;

    GL::FilterMode m_filterMode = Undef<GL::FilterMode>();
    GL::WrapMode m_wrapMode     = Undef<GL::WrapMode>();

    GL::ColorFormat m_glFormat = Undef<GL::ColorFormat>();
    GL::TextureTarget m_target = Undef<GL::TextureTarget>();
};

template<class T>
Image<T> Texture::ToImage(GL::TextureTarget texTarget) const
{
    const int width  = GetWidth();
    const int height = GetHeight();
    const int numComps = GL::GetNumComponents( GL::ColorComp::RGBA );
    Byte *pixels = new Byte[width * height * numComps];


    GL::Push(GL::BindTarget::TEXTURE_2D);
    Bind();
    GL::GetTexImage(texTarget,
                    GL::ColorComp::RGBA,
                    GL::DataType::UNSIGNED_BYTE,
                    pixels);
    GL::Pop(GL::BindTarget::TEXTURE_2D);

    Image<T> img(width, height);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const int coords = (y * width + x) * numComps;
            Color pixelColor = GetColorFromByteArray(pixels, coords);
            img.SetPixel(x, y, pixelColor);
        }
    }

    delete[] pixels;

    return img;
}

NAMESPACE_BANG_END

#endif // TEXTURE_H
