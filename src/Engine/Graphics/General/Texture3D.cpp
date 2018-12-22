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
                     uint paddingFilling)
{
    m_size = size;

    GL::Push(GetGLBindTarget());

    Bind();

    const Vector3i &sizePOT = GetSizePOT();
    const uint totalDataBytes = GetBytesSize();
    Array<uint8_t> paddedPOTData(totalDataBytes, paddingFilling);
    if (newData)
    {
        for (uint z = 0; z < sizePOT.z; ++z)
        {
            for (uint y = 0; y < sizePOT.y; ++y)
            {
                for (uint x = 0; x < sizePOT.x; ++x)
                {
                    if (z < size.z && y < size.y && x < size.x)
                    {
                        const uint idx = (z * size.x * size.y + y * size.x + x);
                        const uint idxPOT =
                            (z * sizePOT.x * sizePOT.y + y * sizePOT.x + x);
                        paddedPOTData[idxPOT] = newData[idx];
                    }
                }
            }
        }
    }

    GL::TexImage3D(GetTextureTarget(),
                   GetSizePOT().x,
                   GetSizePOT().y,
                   GetSizePOT().z,
                   GetFormat(),
                   inputDataColorComp,
                   inputDataType,
                   paddedPOTData.Data());

    if (newData && GetWidth() > 0 && GetHeight() > 0)
    {
        GenerateMipMaps();
    }

    GL::Pop(GetGLBindTarget());

    PropagateAssetChanged();
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

            const uint size = (width * height * depth);
            Array<uint8_t> data(size);
            for (uint z = 0; z < depth; ++z)
            {
                for (uint y = 0; y < height; ++y)
                {
                    for (uint x = 0; x < width; ++x)
                    {
                        uint datai;
                        ifs >> datai;

                        const uint idx = (z * width * height + y * width + x);
                        data[idx] = datai;
                    }
                }
            }

            Fill(data.Data(),
                 Vector3i(width, height, depth),
                 GL::ColorComp::RED,
                 GL::DataType::UNSIGNED_BYTE,
                 255);
        }
    }
    else if (volumeTextureFilepath.HasExtension("dat"))
    {
        FILE *fp = fopen(volumeTextureFilepath.GetAbsolute().ToCString(), "rb");

        unsigned short size[3];
        fread((void *)size, 3, sizeof(unsigned short), fp);
        const uint width = size[0];
        const uint height = size[1];
        const uint depth = size[2];

        uint totalSize = width * height * depth;
        unsigned short *dataPtr = new unsigned short[totalSize];
        fread((void *)dataPtr, totalSize, sizeof(unsigned short), fp);

        fclose(fp);

        int downsample = 4;
        const uint widthDS = width / downsample;
        const uint heightDS = height / downsample;
        const uint depthDS = depth / downsample;
        uint totalSizeDownsampled = (widthDS * heightDS * depthDS);
        Array<uint8_t> dataDS(totalSizeDownsampled);
        for (uint z = 0; z < depthDS; ++z)
        {
            for (uint y = 0; y < heightDS; ++y)
            {
                for (uint x = 0; x < widthDS; ++x)
                {
                    const uint idx = (z * downsample * width * height +
                                      y * downsample * width + x * downsample);
                    const uint idxDS =
                        (z * widthDS * heightDS + y * widthDS + x);
                    dataDS[idxDS] =
                        SCAST<uint8_t>(255 * (float(dataPtr[idx]) / 4095.0f));
                }
            }
        }

        delete[] dataPtr;

        Fill(dataDS.Data(),
             Vector3i(widthDS, heightDS, depthDS),
             GL::ColorComp::RED,
             GL::DataType::UNSIGNED_BYTE,
             255);
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
