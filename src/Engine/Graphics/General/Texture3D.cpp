#include "Bang/Texture3D.h"

#include <fstream>

#include "Bang/Debug.h"
#include "Bang/ImageIODDS.h"

using namespace Bang;

Texture3D::Texture3D() : Texture(GL::TextureTarget::TEXTURE_3D)
{
    SetFormat(GL::ColorFormat::R8);
    CreateEmpty(1, 1, 1);

    SetFilterMode(GL::FilterMode::BILINEAR);
    SetWrapMode(GL::WrapMode::REPEAT);
}

Texture3D::~Texture3D()
{
}

void Texture3D::CreateEmpty(const Vector3i &size)
{
    Resize(size);
}

void Texture3D::CreateEmpty(uint width, uint height, uint depth)
{
    CreateEmpty(Vector3i(width, height, depth));
}

bool Texture3D::Resize(const Vector3i &size)
{
    if (size != GetSize())
    {
        Fill(nullptr, size, GetColorComp(), GetDataType());
        return true;
    }
    return false;
}

void Texture3D::Fill(const Byte *newData,
                     const Vector3i &size,
                     GL::ColorComp inputDataColorComp,
                     GL::DataType inputDataType,
                     Byte paddingFilling)
{
    m_size = size;

    GL::Push(GetGLBindTarget());

    Bind();

    GL::TexImage3D(GetTextureTarget(),
                   GetSizePOT().x,
                   GetSizePOT().y,
                   GetSizePOT().z,
                   GetFormat(),
                   inputDataColorComp,
                   inputDataType,
                   newData);

    if (newData && GetWidth() > 0 && GetHeight() > 0)
    {
        // GenerateMipMaps();
    }

    GL::Pop(GetGLBindTarget());

    PropagateResourceChanged();
}

uint Texture3D::GetWidth() const
{
    return GetSize().x;
}

uint Texture3D::GetHeight() const
{
    return GetSize().y;
}

uint Texture3D::GetDepth() const
{
    return GetSize().z;
}

const Vector3i &Texture3D::GetSize() const
{
    return m_size;
}

Vector3i Texture3D::GetSizePOT() const
{
    return Vector3i(
        GetPOT(GetWidth()), GetPOT(GetHeight()), GetPOT(GetDepth()));
}

uint Texture3D::GetBytesSize() const
{
    return GetSizePOT().x * GetSizePOT().y * GetSizePOT().z *
           GL::GetPixelBytesSize(GetFormat());
}

GL::BindTarget Texture3D::GetGLBindTarget() const
{
    return GL::BindTarget::TEXTURE_3D;
}

void Texture3D::Reflect()
{
    Texture::Reflect();
}

void Texture3D::Import(const Path &volumeTextureFilepath)
{
    if (!volumeTextureFilepath.IsFile())
    {
        Debug_Error(volumeTextureFilepath << " not found!");
        return;
    }

    if (volumeTextureFilepath.HasExtension("txt"))
    {
        SetFormat(GL::ColorFormat::R8);

        std::ifstream ifs(volumeTextureFilepath.GetAbsolute().ToCString());
        if (ifs)
        {
            uint width, height, depth;
            ifs >> width >> height >> depth;

            const uint widthPOT = GetPOT(width);
            const uint heightPOT = GetPOT(height);
            const uint depthPOT = GetPOT(depth);

            const uint sizePOT = (widthPOT * heightPOT * depthPOT);
            Array<uint8_t> data(sizePOT, 255);
            for (uint z = 0; z < depthPOT; ++z)
            {
                for (uint y = 0; y < heightPOT; ++y)
                {
                    for (uint x = 0; x < widthPOT; ++x)
                    {
                        if (z < depth && y < height && x < width)
                        {
                            uint datai;
                            ifs >> datai;

                            const uint idx =
                                (z * widthPOT * heightPOT + y * widthPOT + x);
                            data[idx] = datai;
                        }
                    }
                }
            }

            Fill(data.Data(),
                 Vector3i(width, height, depth),
                 GL::ColorComp::RED,
                 GL::DataType::UNSIGNED_BYTE);
        }
    }
    else if (volumeTextureFilepath.HasExtension("pvm"))
    {
        ImageIODDS::ImportDDS3D(volumeTextureFilepath, this, nullptr);
    }
    else
    {
        Debug_Error(volumeTextureFilepath << " extension not supported!");
    }
}

uint Texture3D::GetPOT(float x)
{
    return SCAST<uint>(
        Math::Pow(2.0f, Math::Ceil(Math::Log(x) / Math::Log(2.0f))));
}
