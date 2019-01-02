#include "Bang/Texture2D.h"

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Color.h"
#include "Bang/GL.h"
#include "Bang/Image.h"
#include "Bang/ImageIO.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Path.h"
#include "Bang/StreamOperators.h"

using namespace Bang;

Texture2D::Texture2D() : Texture(GL::TextureTarget::TEXTURE_2D)
{
    SetFormat(GL::ColorFormat::RGBA8);
    CreateEmpty(1, 1);

    SetFilterMode(GL::FilterMode::BILINEAR);
    SetWrapMode(GL::WrapMode::REPEAT);
}

Texture2D::~Texture2D()
{
}

void Texture2D::OnFormatChanged()
{
    Texture::OnFormatChanged();

    if (GetWidth() >= 1 && GetHeight() >= 1)
    {
        if (GetAssetFilepath().IsFile())
        {
            Image img;
            ImageIO::Import(GetAssetFilepath(), &img);
            Import(img);
        }
    }
}

void Texture2D::CreateEmpty(const Vector2i &size)
{
    Fill(nullptr, size.x, size.y, GetColorComp(), GetDataType());
}

bool Texture2D::Resize(const Vector2i &size)
{
    if (size != GetSize())
    {
        CreateEmpty(size.x, size.y);
        return true;
    }
    return false;
}

void Texture2D::Fill(const Color &fillColor, int width, int height)
{
    Array<Color> inputData = Array<Color>(width * height, fillColor);
    Fill(RCAST<const Byte *>(inputData.Data()),
         width,
         height,
         GL::ColorComp::RGBA,
         GL::DataType::FLOAT);
}

void Texture2D::Fill(const Byte *newData,
                     int width,
                     int height,
                     GL::ColorComp inputDataColorComp,
                     GL::DataType inputDataType)
{
    SetWidth(width);
    SetHeight(height);

    GL::Push(GetGLBindTarget());

    Bind();
    GL::TexImage2D(GetTextureTarget(),
                   GetWidth(),
                   GetHeight(),
                   GetFormat(),
                   inputDataColorComp,
                   inputDataType,
                   newData);

    if (newData && GetWidth() > 0 && GetHeight() > 0)
    {
        GenerateMipMaps();
    }

    GL::Pop(GetGLBindTarget());

    PropagateAssetChanged();
}

void Texture2D::SetAlphaCutoff(float alphaCutoff)
{
    if (alphaCutoff != GetAlphaCutoff())
    {
        m_alphaCutoff = alphaCutoff;
        PropagateAssetChanged();
    }
}

float Texture2D::GetAlphaCutoff() const
{
    return m_alphaCutoff;
}

const Image &Texture2D::GetImage() const
{
    return m_image;
}

void Texture2D::CreateEmpty(int width, int height)
{
    CreateEmpty(Vector2i(width, height));
}

bool Texture2D::Resize(int width, int height)
{
    return Resize(Vector2i(width, height));
}

int Texture2D::GetWidth() const
{
    return m_size.x;
}

int Texture2D::GetHeight() const
{
    return m_size.y;
}

const Vector2i &Texture2D::GetSize() const
{
    return m_size;
}

void Texture2D::SetWidth(int width)
{
    if (width != GetWidth())
    {
        m_size.x = width;
        PropagateAssetChanged();
    }
}

void Texture2D::SetHeight(int height)
{
    if (height != GetHeight())
    {
        m_size.y = height;
        PropagateAssetChanged();
    }
}

uint Texture2D::GetBytesSize() const
{
    return GetWidth() * GetHeight() * GL::GetPixelBytesSize(GetFormat());
}

Image Texture2D::ToImage() const
{
    const int width = GetWidth();
    const int height = GetHeight();
    Image img(width, height);

    const int numComps = GL::GetNumComponents(GL::ColorComp::RGBA);
    Byte *pixels = new Byte[width * height * numComps];

    GL::Push(GL::BindTarget::TEXTURE_2D);
    Bind();
    GL::GetTexImage(GetTextureTarget(),
                    GL::ColorComp::RGBA,
                    GL::DataType::UNSIGNED_BYTE,
                    pixels);
    GL::Pop(GL::BindTarget::TEXTURE_2D);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const int i = (y * width + x) * numComps;
            Color pixelColor = GetColorFromByteArray(pixels, i);
            if (GetColorComp() == GL::ColorComp::RED)
            {
                pixelColor = Color(pixelColor.r, pixelColor.r, pixelColor.r, 1);
            }
            img.SetPixel(x, y, pixelColor);
        }
    }

    delete[] pixels;

    return img;
}

void Texture2D::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);

    if (metaNode.Contains("Format"))
    {
        SetFormat(metaNode.Get<GL::ColorFormat>("Format"));
    }

    if (metaNode.Contains("FilterMode"))
    {
        SetFilterMode(metaNode.Get<GL::FilterMode>("FilterMode"));
    }

    if (metaNode.Contains("WrapModeS"))
    {
        SetWrapMode(metaNode.Get<GL::WrapMode>("WrapModeS"),
                    GL::WrapCoord::WRAP_S);
    }
    if (metaNode.Contains("WrapModeT"))
    {
        SetWrapMode(metaNode.Get<GL::WrapMode>("WrapModeT"),
                    GL::WrapCoord::WRAP_T);
    }
    if (metaNode.Contains("WrapModeR"))
    {
        SetWrapMode(metaNode.Get<GL::WrapMode>("WrapModeR"),
                    GL::WrapCoord::WRAP_R);
    }

    if (metaNode.Contains("AlphaCutoff"))
    {
        SetAlphaCutoff(metaNode.Get<float>("AlphaCutoff"));
    }
}

void Texture2D::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);

    metaNode->Set("Format", GetFormat());
    metaNode->Set("FilterMode", GetFilterMode());
    metaNode->Set("WrapModeS", GetWrapMode(GL::WrapCoord::WRAP_S));
    metaNode->Set("WrapModeT", GetWrapMode(GL::WrapCoord::WRAP_T));
    metaNode->Set("WrapModeR", GetWrapMode(GL::WrapCoord::WRAP_R));
    metaNode->Set("AlphaCutoff", GetAlphaCutoff());
}

void Texture2D::Import(const Path &imageFilepath)
{
    ImageIO::Import(imageFilepath, &m_image, this, nullptr);

    Path importFilepath = MetaFilesManager::GetMetaFilepath(imageFilepath);
    ImportMetaFromFile(importFilepath);
}

void Texture2D::Import(const Image &image)
{
    if (image.GetData())
    {
        m_image = image;

        SetWidth(image.GetWidth());
        SetHeight(image.GetHeight());

        Fill(image.GetData(),
             GetWidth(),
             GetHeight(),
             GL::ColorComp::RGBA,
             GL::DataType::UNSIGNED_BYTE);
    }
}

GL::BindTarget Texture2D::GetGLBindTarget() const
{
    return GL::BindTarget::TEXTURE_2D;
}
