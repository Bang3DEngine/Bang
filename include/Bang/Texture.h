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

    void GenerateMipMaps() const;

    void SetFormat(GL::ColorFormat internalFormat);
    void SetTarget(GL::TextureTarget target);
    void SetFilterMode(GL::FilterMode filterMode);
    void SetWrapMode(GL::WrapMode wrapMode);

    GL::DataType GetDataType() const;
    GL::ColorComp GetColorComp() const;
    GL::ColorFormat GetFormat() const;
    GL::FilterMode GetFilterMode() const;
    int GetNumComponents() const;
    uint GetBytesSize() const;
    GL::WrapMode GetWrapMode(
        GL::WrapCoord wrapCoord = GL::WrapCoord::WRAP_R) const;

    GL::TextureTarget GetTextureTarget() const;

protected:
    void SetWrapMode(GL::WrapMode wrapMode, GL::WrapCoord wrapCoord);

    GL::WrapMode GetWrapMode(Axis3D dim) const;
    static Color GetColorFromFloatArray(const float *pixels, int i);
    static Color GetColorFromByteArray(const Byte *pixels, int i);

    virtual void OnFormatChanged();

private:
    GL::ColorFormat m_glFormat = Undef<GL::ColorFormat>();
    GL::TextureTarget m_target = Undef<GL::TextureTarget>();

    GL::FilterMode m_filterMode = Undef<GL::FilterMode>();
    std::array<GL::WrapMode, 3> m_wrapMode = {
        {Undef<GL::WrapMode>(), Undef<GL::WrapMode>(), Undef<GL::WrapMode>()}};
};
}  // namespace Bang

#endif  // TEXTURE_H
