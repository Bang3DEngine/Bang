#include "Bang/TextureCubeMap.h"

#include "Bang/Path.h"
#include "Bang/XMLNode.h"
#include "Bang/Resources.h"

USING_NAMESPACE_BANG

const std::array<GL::CubeMapDir, 6> TextureCubeMap::AllCubeMapDirs =
    {GL::CubeMapDir::PositiveX, GL::CubeMapDir::NegativeX,
     GL::CubeMapDir::PositiveY, GL::CubeMapDir::NegativeY,
     GL::CubeMapDir::PositiveZ, GL::CubeMapDir::NegativeZ};

TextureCubeMap::TextureCubeMap() : Texture(GL::TextureTarget::TextureCubeMap)
{
    SetFormat(GL::ColorFormat::RGBA_UByte8);
    CreateEmpty(1, 1);

    SetFilterMode(GL::FilterMode::Bilinear);
    SetWrapMode(GL::WrapMode::ClampToEdge);
}

TextureCubeMap::~TextureCubeMap()
{
}

void TextureCubeMap::CreateEmpty(int size)
{
    CreateEmpty(size, size);
}
void TextureCubeMap::CreateEmpty(int width, int height)
{
    ASSERT_MSG(width == height, "CubeMaps must have the same width and height.");
    SetWidth(width);
    SetHeight(height);

    for (GL::CubeMapDir cubeMapDir : TextureCubeMap::AllCubeMapDirs)
    {
        Fill(cubeMapDir, nullptr, Math::Min(width, height),
             GetColorComp(), GetDataType());
    }
}

void TextureCubeMap::Resize(int width, int height)
{
    ASSERT_MSG(width == height, "CubeMaps must have the same width and height.");
    if (width != GetWidth() || height != GetHeight())
    {
        CreateEmpty(width, height);
    }
}
void TextureCubeMap::Resize(int size)
{
    Resize(size, size);
}

void TextureCubeMap::Fill(GL::CubeMapDir cubeMapDir,
                          const Byte *newData,
                          int size,
                          GL::ColorComp inputDataColorComp,
                          GL::DataType inputDataType)
{
    SetWidth(size);
    SetHeight(size);

    GLId prevBoundId = GL::GetBoundId( GetGLBindTarget() ); // Save state

    Bind();
    GL::TexImage2D(SCAST<GL::TextureTarget>(cubeMapDir),
                   GetWidth(),
                   GetHeight(),
                   GetFormat(),
                   inputDataColorComp,
                   inputDataType,
                   newData);

    GL::Bind(GetGLBindTarget(), prevBoundId); // Restore

    PropagateTextureChanged();
}

void TextureCubeMap::SetImageResource(GL::CubeMapDir cubeMapDir, Imageb *img)
{
    Imageb imgForTexture;
    if (img) { imgForTexture = *img; }

    Fill(cubeMapDir,
         imgForTexture.GetData(),
         Math::Min(imgForTexture.GetWidth(), imgForTexture.GetHeight()),
         GL::ColorComp::RGBA,
         GL::DataType::UnsignedByte);

    m_imageResources[ TextureCubeMap::GetDirIndex(cubeMapDir) ].Set(img);
}

Imageb TextureCubeMap::ToImage(GL::CubeMapDir cubeMapDir) const
{
    return Texture::ToImage<Byte>( SCAST<GL::TextureTarget>(cubeMapDir) );
}

RH<Imageb> TextureCubeMap::GetImageResource(GL::CubeMapDir cubeMapDir) const
{
    return m_imageResources[ TextureCubeMap::GetDirIndex(cubeMapDir) ];
}

void TextureCubeMap::Import(const Image<Byte> &topImage,
                            const Image<Byte> &botImage,
                            const Image<Byte> &leftImage,
                            const Image<Byte> &rightImage,
                            const Image<Byte> &frontImage,
                            const Image<Byte> &backImage)
{
    SetFormat(GL::ColorFormat::RGBA_UByte8);

    std::array<Image<Byte>, 6> imgs = {topImage, botImage, leftImage,
                                       rightImage, frontImage, backImage};
    for (int i = 0; i < TextureCubeMap::AllCubeMapDirs.size(); ++i)
    {
        const Image<Byte> &img = imgs[i];
        if (img.GetData())
        {
            GL::CubeMapDir cubeMapDir = TextureCubeMap::AllCubeMapDirs[i];

            SetWidth(img.GetWidth());
            SetHeight(img.GetHeight());

            Fill(cubeMapDir,
                 img.GetData(),
                 Math::Min(GetWidth(), GetHeight()),
                 GL::ColorComp::RGBA,
                 GL::DataType::UnsignedByte);
        }
    }
}

void TextureCubeMap::ImportXML(const XMLNode &xmlInfo)
{
    Asset::ImportXML(xmlInfo);

    if (xmlInfo.Contains("TopImage"))
    {
        SetImageResource(GL::CubeMapDir::Top,
                 Resources::Load<Imageb>(xmlInfo.Get<GUID>("TopImage") ).Get() );
    }
    if (xmlInfo.Contains("BotImage"))
    {
        SetImageResource(GL::CubeMapDir::Bot,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("BotImage") ).Get() );
    }
    if (xmlInfo.Contains("LeftImage"))
    {
        SetImageResource(GL::CubeMapDir::Left,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("LeftImage") ).Get() );
    }
    if (xmlInfo.Contains("RightImage"))
    {
        SetImageResource(GL::CubeMapDir::Right,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("RightImage") ).Get() );
    }
    if (xmlInfo.Contains("FrontImage"))
    {
        SetImageResource(GL::CubeMapDir::Front,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("FrontImage") ).Get() );
    }
    if (xmlInfo.Contains("BackImage"))
    {
        SetImageResource(GL::CubeMapDir::Back,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("BackImage") ).Get() );
    }

}

void TextureCubeMap::ExportXML(XMLNode *xmlInfo) const
{
    Asset::ExportXML(xmlInfo);

    if (GetImageResource(GL::CubeMapDir::Top))
    { xmlInfo->Set("TopImage", GetImageResource(GL::CubeMapDir::Top).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::Bot))
    { xmlInfo->Set("BotImage", GetImageResource(GL::CubeMapDir::Bot).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::Left))
    { xmlInfo->Set("LeftImage", GetImageResource(GL::CubeMapDir::Left).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::Right))
    { xmlInfo->Set("RightImage", GetImageResource(GL::CubeMapDir::Right).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::Front))
    { xmlInfo->Set("FrontImage", GetImageResource(GL::CubeMapDir::Front).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::Back))
    { xmlInfo->Set("BackImage", GetImageResource(GL::CubeMapDir::Back).Get()->GetGUID()); }
}

void TextureCubeMap::Import(const Path &textureCubeMapFilepath)
{
    ImportXMLFromFile(textureCubeMapFilepath);
}

GL::BindTarget TextureCubeMap::GetGLBindTarget() const
{
    return GL::BindTarget::TextureCubeMap;
}

unsigned int TextureCubeMap::GetDirIndex(GL::CubeMapDir dir)
{
    switch (dir)
    {
        case GL::CubeMapDir::Top:   return 0; break;
        case GL::CubeMapDir::Bot:   return 1; break;
        case GL::CubeMapDir::Right: return 2; break;
        case GL::CubeMapDir::Left:  return 3; break;
        case GL::CubeMapDir::Back:  return 4; break;
        case GL::CubeMapDir::Front: return 5; break;
    }
    ASSERT(false);
    return 0;
}

