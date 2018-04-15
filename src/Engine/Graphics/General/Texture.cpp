#include "Bang/Texture.h"

#include "Bang/GL.h"

USING_NAMESPACE_BANG

Texture::Texture()
{
    GL::GenTextures(1, &m_idGL);
}

Texture::~Texture()
{
    GL::DeleteTextures(1, &m_idGL);
}

Texture::Texture(GL::TextureTarget texTarget) : Texture()
{
    m_target = texTarget;
}

Texture::Texture(const Texture &t) : GLObject(t)
{
    m_size = Vector2i(t.GetWidth(), t.GetHeight());
    m_filterMode = t.m_filterMode;
    m_wrapMode = t.m_wrapMode;
    m_glFormat = t.m_glFormat;
    m_target = t.m_target;
}

void Texture::GenerateMipMaps() const
{
    ASSERT( GL::IsBound(this) );
    GL::GenerateMipMap( GetTextureTarget() );
}

void Texture::SetFormat(GL::ColorFormat internalFormat)
{
    m_glFormat = internalFormat;
}

void Texture::SetTarget(GL::TextureTarget target)
{
    m_target = target;
}

void Texture::SetFilterMode(GL::FilterMode filterMode)
{
    if (filterMode != GetFilterMode())
    {
        m_filterMode = filterMode;

        GLId prevBoundTex = GL::GetBoundId(GetGLBindTarget());
        Bind();

        if (GetFilterMode() == GL::FilterMode::Nearest ||
            GetFilterMode() == GL::FilterMode::Bilinear)
        {
            GL::TexParameterFilter(GetTextureTarget(), GL::FilterMagMin::Mag,
                                   GetFilterMode());
        }
        GL::TexParameterFilter(GetTextureTarget(), GL::FilterMagMin::Min,
                               GetFilterMode());

        GL::Bind(GetGLBindTarget(), prevBoundTex);

        PropagateTextureChanged();
    }
}

void Texture::SetWrapMode(GL::WrapMode wrapMode)
{
    if (wrapMode != GetWrapMode())
    {
        m_wrapMode = wrapMode;

        GLId prevBoundTex = GL::GetBoundId(GetGLBindTarget());
        Bind();
        GL::TexParameterWrap(GetTextureTarget(), GL::WrapCoord::WrapS, GetWrapMode());
        GL::TexParameterWrap(GetTextureTarget(), GL::WrapCoord::WrapT, GetWrapMode());
        GL::TexParameterWrap(GetTextureTarget(), GL::WrapCoord::WrapR, GetWrapMode());
        GL::Bind(GetGLBindTarget(), prevBoundTex);

        PropagateTextureChanged();
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
    return GL::GetDataTypeFrom( GetFormat() );
}

GL::ColorComp Texture::GetColorComp() const
{
    return GL::GetColorCompFrom( GetFormat() );
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
    return Color(pixels[i+0], pixels[i+1], pixels[i+2], pixels[i+3]);
}

Color Texture::GetColorFromByteArray(const Byte *pixels, int i)
{
    return Color(pixels[i+0] / 255.0f, pixels[i+1] / 255.0f,
                 pixels[i+2] / 255.0f, pixels[i+3] / 255.0f);
}

int Texture::GetNumComponents() const
{
    return GL::GetNumComponents( GetFormat() );
}


void Texture::SetWidth(int width)
{
    if (width != GetWidth())
    {
        m_size.x = width;
        PropagateTextureChanged();
    }
}
void Texture::SetHeight(int height)
{
    if (height != GetHeight())
    {
        m_size.y = height;
        PropagateTextureChanged();
    }
}

void Texture::PropagateTextureChanged()
{
    EventEmitter<ITextureChangedListener>::PropagateToListeners(
                &ITextureChangedListener::OnTextureChanged, this);
}
