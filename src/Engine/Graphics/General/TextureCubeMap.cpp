#include "Bang/TextureCubeMap.h"

#include "Bang/Path.h"
#include "Bang/XMLNode.h"
#include "Bang/Resources.h"

USING_NAMESPACE_BANG

const std::array<GL::CubeMapDir, 6> TextureCubeMap::AllCubeMapDirs =
    {GL::CubeMapDir::PositiveX, GL::CubeMapDir::PositiveX,
     GL::CubeMapDir::PositiveY, GL::CubeMapDir::PositiveY,
     GL::CubeMapDir::PositiveZ, GL::CubeMapDir::PositiveZ};

TextureCubeMap::TextureCubeMap() : Texture(GL::TextureTarget::TextureCubeMap)
{
    CreateEmpty(1,1);

    SetFilterMode(GL::FilterMode::Bilinear);
    SetWrapMode(GL::WrapMode::ClampToEdge);
    SetFormat( GetFormat() );
}

TextureCubeMap::~TextureCubeMap()
{
}

void TextureCubeMap::CreateEmpty(int width, int height)
{
    SetWidth(width);
    SetHeight(height);

    for (GL::CubeMapDir cubeMapDir : TextureCubeMap::AllCubeMapDirs)
    {
        Fill(cubeMapDir, nullptr,
             Math::Min(width, height),
             GL::ColorComp::RGBA,
             GL::DataType::UnsignedByte);
    }
}

void TextureCubeMap::Resize(int width, int height)
{
    if (width != GetWidth() || height != GetHeight())
    {
        CreateEmpty(width, height);
    }
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

void TextureCubeMap::SetDirTexture(GL::CubeMapDir cubeMapDir, Texture2D *tex)
{
    // Debug_Log("SetDirTexture " << cubeMapDir << ": " << (tex ? tex->GetSize() : Vector2i(-1)));
    Fill(cubeMapDir,
         (tex ? tex->ToImage<Byte>().GetData() : SCAST<Byte*>(nullptr)),
         Math::Min(tex->GetWidth(), tex->GetHeight()),
         GL::ColorComp::RGBA,
         GL::DataType::UnsignedByte);
    m_dirTextures[ GetDirIndex(cubeMapDir) ].Set(tex);
}

Texture2D *TextureCubeMap::GetDirTexture(GL::CubeMapDir cubeMapDir) const
{
    return m_dirTextures[ GetDirIndex(cubeMapDir) ].Get();
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
        SetDirTexture(GL::CubeMapDir::Top,
            Resources::Load<Texture2D>(xmlInfo.Get<GUID>("TopImage") ).Get() );
    }
    if (xmlInfo.Contains("BotImage"))
    {
        SetDirTexture(GL::CubeMapDir::Bot,
            Resources::Load<Texture2D>( xmlInfo.Get<GUID>("BotImage") ).Get() );
    }
    if (xmlInfo.Contains("LeftImage"))
    {
        SetDirTexture(GL::CubeMapDir::Left,
            Resources::Load<Texture2D>( xmlInfo.Get<GUID>("LeftImage") ).Get() );
    }
    if (xmlInfo.Contains("RightImage"))
    {
        SetDirTexture(GL::CubeMapDir::Right,
            Resources::Load<Texture2D>( xmlInfo.Get<GUID>("RightImage") ).Get() );
    }
    if (xmlInfo.Contains("FrontImage"))
    {
        SetDirTexture(GL::CubeMapDir::Front,
            Resources::Load<Texture2D>( xmlInfo.Get<GUID>("FrontImage") ).Get() );
    }
    if (xmlInfo.Contains("BackImage"))
    {
        SetDirTexture(GL::CubeMapDir::Back,
            Resources::Load<Texture2D>( xmlInfo.Get<GUID>("BackImage") ).Get() );
    }

}

void TextureCubeMap::ExportXML(XMLNode *xmlInfo) const
{
    Asset::ExportXML(xmlInfo);

    if (GetDirTexture(GL::CubeMapDir::Top))
    { xmlInfo->Set("TopImage", GetDirTexture(GL::CubeMapDir::Top)->GetGUID()); }
    if (GetDirTexture(GL::CubeMapDir::Bot))
    { xmlInfo->Set("BotImage", GetDirTexture(GL::CubeMapDir::Bot)->GetGUID()); }
    if (GetDirTexture(GL::CubeMapDir::Left))
    { xmlInfo->Set("LeftImage", GetDirTexture(GL::CubeMapDir::Left)->GetGUID()); }
    if (GetDirTexture(GL::CubeMapDir::Right))
    { xmlInfo->Set("RightImage", GetDirTexture(GL::CubeMapDir::Right)->GetGUID()); }
    if (GetDirTexture(GL::CubeMapDir::Front))
    { xmlInfo->Set("FrontImage", GetDirTexture(GL::CubeMapDir::Front)->GetGUID()); }
    if (GetDirTexture(GL::CubeMapDir::Back))
    { xmlInfo->Set("BackImage", GetDirTexture(GL::CubeMapDir::Back)->GetGUID()); }
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

