#include "Bang/Texture3D.h"

#include <fstream>

#include "Bang/Debug.h"
#include "Bang/ImageIODDS.h"

using namespace Bang;

Texture3D::Texture3D() : Texture(GL::TextureTarget::TEXTURE_3D)
{
    SetFormat(GL::ColorFormat::RGBA8);
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
                     GL::DataType inputDataType)
{
    m_size = size;

    GL::Push(GetGLBindTarget());

    Bind();
    GL::TexImage3D(GetTextureTarget(),
                   GetWidth(),
                   GetHeight(),
                   GetDepth(),
                   GetFormat(),
                   inputDataColorComp,
                   inputDataType,
                   newData);

    if (newData && GetWidth() > 0 && GetHeight() > 0)
    {
        GenerateMipMaps();
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

uint Texture3D::GetBytesSize() const
{
    return GetWidth() * GetHeight() * GetDepth() *
           GL::GetPixelBytesSize(GetFormat());
}

GL::BindTarget Texture3D::GetGLBindTarget() const
{
    return GL::BindTarget::TEXTURE_3D;
}

void Texture3D::Reflect()
{
    ReflectVar<Vector3>("Size",
                        [this](const Vector3 &size) { Resize(Vector3i(size)); },
                        [this]() { return Vector3(GetSize()); });
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

            uint size = (width * height * depth);
            Array<uint8_t> data(size, 0);
            for (uint i = 0; i < size; ++i)
            {
                uint datai;
                if (!(ifs >> datai))
                {
                    break;
                }
                data[i] = datai;
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
