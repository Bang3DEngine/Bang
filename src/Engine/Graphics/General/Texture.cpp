#include "Bang/Texture.h"

#include <sys/types.h>

#include "Bang/Assert.h"
#include "Bang/GL.h"

using namespace Bang;

Texture::Texture()
{
    GL::GenTextures(1, &m_idGL);
}

Texture::Texture(GL::TextureTarget texTarget) : Texture()
{
    m_target = texTarget;
}

Texture::~Texture()
{
    GL::DeleteTextures(1, &m_idGL);
}

void Texture::GenerateMipMaps() const
{
    ASSERT(GL::IsBound(this));
    GL::GenerateMipMap(GetTextureTarget());
}

void Texture::SetFormat(GL::ColorFormat glFormat)
{
    if (glFormat != GetFormat())
    {
        m_glFormat = glFormat;
        OnFormatChanged();
        PropagateResourceChanged();
    }
}

void Texture::SetTarget(GL::TextureTarget target)
{
    if (target != GetTextureTarget())
    {
        m_target = target;
        PropagateResourceChanged();
    }
}

void Texture::SetWrapMode(GL::WrapMode wrapMode)
{
    SetWrapMode(wrapMode, GL::WrapCoord::WRAP_R);
    SetWrapMode(wrapMode, GL::WrapCoord::WRAP_S);
    SetWrapMode(wrapMode, GL::WrapCoord::WRAP_T);
}

void Texture::SetFilterMode(GL::FilterMode filterMode)
{
    if (filterMode != GetFilterMode())
    {
        m_filterMode = filterMode;

        GL::Push(GetGLBindTarget());

        Bind();

        if (GetFilterMode() == GL::FilterMode::NEAREST ||
            GetFilterMode() == GL::FilterMode::BILINEAR)
        {
            GL::TexParameterFilter(
                GetTextureTarget(), GL::FilterMagMin::MAG, GetFilterMode());
        }
        GL::TexParameterFilter(
            GetTextureTarget(), GL::FilterMagMin::MIN, GetFilterMode());

        GL::Pop(GetGLBindTarget());

        PropagateResourceChanged();
    }
}

GL::DataType Texture::GetDataType() const
{
    return GL::GetDataTypeFrom(GetFormat());
}

GL::ColorComp Texture::GetColorComp() const
{
    return GL::GetColorCompFrom(GetFormat());
}

GL::ColorFormat Texture::GetFormat() const
{
    return m_glFormat;
}

GL::FilterMode Texture::GetFilterMode() const
{
    return m_filterMode;
}

GL::TextureTarget Texture::GetTextureTarget() const
{
    return m_target;
}

void Texture::SetWrapMode(GL::WrapMode wrapMode, GL::WrapCoord wrapCoord)
{
    if (wrapMode != GetWrapMode())
    {
        uint idx;
        switch (wrapCoord)
        {
            case GL::WrapCoord::WRAP_R: idx = 0; break;
            case GL::WrapCoord::WRAP_S: idx = 1; break;
            case GL::WrapCoord::WRAP_T: idx = 2; break;
            default: ASSERT(false); break;
        }
        m_wrapMode[idx] = wrapMode;

        GL::Push(GetGLBindTarget());

        Bind();
        GL::TexParameterWrap(GetTextureTarget(), wrapCoord, GetWrapMode());

        GL::Pop(GetGLBindTarget());

        PropagateResourceChanged();
    }
}

GL::WrapMode Texture::GetWrapMode(Axis3D dim) const
{
    return m_wrapMode[SCAST<uint>(dim)];
}

Color Texture::GetColorFromFloatArray(const float *pixels, int i)
{
    return Color(pixels[i + 0], pixels[i + 1], pixels[i + 2], pixels[i + 3]);
}

Color Texture::GetColorFromByteArray(const Byte *pixels, int i)
{
    return Color(pixels[i + 0] / 255.0f,
                 pixels[i + 1] / 255.0f,
                 pixels[i + 2] / 255.0f,
                 pixels[i + 3] / 255.0f);
}

void Texture::OnFormatChanged()
{
}

int Texture::GetNumComponents() const
{
    return GL::GetNumComponents(GetFormat());
}

GL::WrapMode Texture::GetWrapMode() const
{
    return m_wrapMode[0];
}
