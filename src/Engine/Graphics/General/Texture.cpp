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

void Texture::CreateEmpty(int width, int height)
{
    CreateEmpty(Vector2i(width, height));
}

bool Texture::Resize(int width, int height)
{
    return Resize(Vector2i(width, height));
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

void Texture::SetWrapMode(GL::WrapMode wrapMode)
{
    if (wrapMode != GetWrapMode())
    {
        m_wrapMode = wrapMode;

        GL::Push(GetGLBindTarget());

        Bind();
        GL::TexParameterWrap(
            GetTextureTarget(), GL::WrapCoord::WRAP_S, GetWrapMode());
        GL::TexParameterWrap(
            GetTextureTarget(), GL::WrapCoord::WRAP_T, GetWrapMode());
        GL::TexParameterWrap(
            GetTextureTarget(), GL::WrapCoord::WRAP_R, GetWrapMode());

        GL::Pop(GetGLBindTarget());

        PropagateResourceChanged();
    }
}

int Texture::GetWidth() const
{
    return m_size.x;
}

int Texture::GetHeight() const
{
    return m_size.y;
}

const Vector2i &Texture::GetSize() const
{
    return m_size;
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

uint Texture::GetBytesSize() const
{
    return GetWidth() * GetHeight() * GL::GetPixelBytesSize(m_glFormat);
}

GL::TextureTarget Texture::GetTextureTarget() const
{
    return m_target;
}

GL::FilterMode Texture::GetFilterMode() const
{
    return m_filterMode;
}

GL::WrapMode Texture::GetWrapMode() const
{
    return m_wrapMode;
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

void Texture::SetWidth(int width)
{
    if (width != GetWidth())
    {
        m_size.x = width;
        PropagateResourceChanged();
    }
}
void Texture::SetHeight(int height)
{
    if (height != GetHeight())
    {
        m_size.y = height;
        PropagateResourceChanged();
    }
}
