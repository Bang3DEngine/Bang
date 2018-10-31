#ifndef TEXTURE_H
#define TEXTURE_H

#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/GL.h"
#include "Bang/GLObject.h"
#include "Bang/Image.h"
#include "Bang/String.h"

namespace Bang
{
class Texture : public GLObject, public Asset
{
    ASSET_ABSTRACT(Texture)

public:
    Texture();
    Texture(GL::TextureTarget texTarget);
    virtual ~Texture() override;

    virtual void CreateEmpty(const Vector2i &size) = 0;
    virtual bool Resize(const Vector2i &size) = 0;
    void CreateEmpty(int width, int height);
    bool Resize(int width, int height);
    void GenerateMipMaps() const;

    void SetFormat(GL::ColorFormat internalFormat);
    void SetTarget(GL::TextureTarget target);
    void SetFilterMode(GL::FilterMode filterMode);
    void SetWrapMode(GL::WrapMode wrapMode);

    int GetWidth() const;
    int GetHeight() const;
    const Vector2i &GetSize() const;
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

    Image ToImage(GL::TextureTarget texTarget) const;

    virtual void OnFormatChanged();

private:
    Vector2i m_size = Vector2i::Zero;

    GL::FilterMode m_filterMode = Undef<GL::FilterMode>();
    GL::WrapMode m_wrapMode = Undef<GL::WrapMode>();

    GL::ColorFormat m_glFormat = Undef<GL::ColorFormat>();
    GL::TextureTarget m_target = Undef<GL::TextureTarget>();
};
}  // namespace Bang

#endif  // TEXTURE_H
