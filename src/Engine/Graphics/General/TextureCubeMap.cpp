#include "Bang/TextureCubeMap.h"

#include "Bang/Path.h"
#include "Bang/XMLNode.h"
#include "Bang/Resources.h"

USING_NAMESPACE_BANG

const std::array<GL::CubeMapDir, 6> TextureCubeMap::AllCubeMapDirs =
    {{GL::CubeMapDir::POSITIVE_X, GL::CubeMapDir::NEGATIVE_X,
      GL::CubeMapDir::POSITIVE_Y, GL::CubeMapDir::NEGATIVE_Y,
      GL::CubeMapDir::POSITIVE_Z, GL::CubeMapDir::NEGATIVE_Z}};

TextureCubeMap::TextureCubeMap() : Texture(GL::TextureTarget::TEXTURE_CUBE_MAP)
{
    SetFormat(GL::ColorFormat::RGBA8);
    CreateEmpty(1, 1);

    SetFilterMode(GL::FilterMode::BILINEAR);
    SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);
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

    GL::Push( GetGLBindTarget() );

    Bind();
    GL::TexImage2D(SCAST<GL::TextureTarget>(cubeMapDir),
                   GetWidth(),
                   GetHeight(),
                   GetFormat(),
                   inputDataColorComp,
                   inputDataType,
                   newData);

    GL::Pop( GetGLBindTarget() );

    PropagateTextureChanged();
}

void TextureCubeMap::SetImageResource(GL::CubeMapDir cubeMapDir, Imageb *img)
{
    if (GetImageResource(cubeMapDir).Get() != img)
    {
        if (GetImageResource(cubeMapDir).Get())
        {
            GetImageResource(cubeMapDir).Get()->
                    EventEmitter<IEventsResource>::UnRegisterListener(this);
        }

        FillCubeMapDir(cubeMapDir, img);
        m_imageResources[ TextureCubeMap::GetDirIndex(cubeMapDir) ].Set(img);

        if (GetImageResource(cubeMapDir).Get())
        {
            GetImageResource(cubeMapDir).Get()->
                    EventEmitter<IEventsResource>::RegisterListener(this);
        }
    }
}

void TextureCubeMap::FillCubeMapDir(GL::CubeMapDir dir, Imageb *img)
{
    Fill(dir,
         (img ? img->GetData() : nullptr),
         (img ? Math::Min(img->GetWidth(), img->GetHeight()) : 0),
         GL::ColorComp::RGBA,
         GL::DataType::UNSIGNED_BYTE);
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
    SetFormat(GL::ColorFormat::RGBA8);

    std::array<Image<Byte>, 6> imgs = {{topImage, botImage, leftImage,
                                        rightImage, frontImage, backImage}};
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
                 GL::DataType::UNSIGNED_BYTE);
        }
    }
}

void TextureCubeMap::ImportXML(const XMLNode &xmlInfo)
{
    Asset::ImportXML(xmlInfo);

    if (xmlInfo.Contains("TopImage"))
    {
        SetImageResource(GL::CubeMapDir::TOP,
                 Resources::Load<Imageb>(xmlInfo.Get<GUID>("TopImage") ).Get() );
    }
    if (xmlInfo.Contains("BotImage"))
    {
        SetImageResource(GL::CubeMapDir::BOT,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("BotImage") ).Get() );
    }
    if (xmlInfo.Contains("LeftImage"))
    {
        SetImageResource(GL::CubeMapDir::LEFT,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("LeftImage") ).Get() );
    }
    if (xmlInfo.Contains("RightImage"))
    {
        SetImageResource(GL::CubeMapDir::RIGHT,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("RightImage") ).Get() );
    }
    if (xmlInfo.Contains("FrontImage"))
    {
        SetImageResource(GL::CubeMapDir::FRONT,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("FrontImage") ).Get() );
    }
    if (xmlInfo.Contains("BackImage"))
    {
        SetImageResource(GL::CubeMapDir::BACK,
                 Resources::Load<Imageb>( xmlInfo.Get<GUID>("BackImage") ).Get() );
    }
}

void TextureCubeMap::ExportXML(XMLNode *xmlInfo) const
{
    Asset::ExportXML(xmlInfo);

    if (GetImageResource(GL::CubeMapDir::TOP))
    { xmlInfo->Set("TopImage", GetImageResource(GL::CubeMapDir::TOP).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::BOT))
    { xmlInfo->Set("BotImage", GetImageResource(GL::CubeMapDir::BOT).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::LEFT))
    { xmlInfo->Set("LeftImage", GetImageResource(GL::CubeMapDir::LEFT).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::RIGHT))
    { xmlInfo->Set("RightImage", GetImageResource(GL::CubeMapDir::RIGHT).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::FRONT))
    { xmlInfo->Set("FrontImage", GetImageResource(GL::CubeMapDir::FRONT).Get()->GetGUID()); }
    if (GetImageResource(GL::CubeMapDir::BACK))
    { xmlInfo->Set("BackImage", GetImageResource(GL::CubeMapDir::BACK).Get()->GetGUID()); }
}

void TextureCubeMap::Import(const Path &textureCubeMapFilepath)
{
    ImportXMLFromFile(textureCubeMapFilepath);
}

GL::BindTarget TextureCubeMap::GetGLBindTarget() const
{
    return GL::BindTarget::TEXTURE_CUBE_MAP;
}

void TextureCubeMap::OnImported(Resource *res)
{
    for (GL::CubeMapDir cubeMapDir : TextureCubeMap::AllCubeMapDirs)
    {
        if (res == GetImageResource(cubeMapDir).Get())
        {
            FillCubeMapDir(cubeMapDir, DCAST<Imageb*>(res));
        }
    }
}

unsigned int TextureCubeMap::GetDirIndex(GL::CubeMapDir dir)
{
    switch (dir)
    {
        case GL::CubeMapDir::TOP:   return 0; break;
        case GL::CubeMapDir::BOT:   return 1; break;
        case GL::CubeMapDir::RIGHT: return 2; break;
        case GL::CubeMapDir::LEFT:  return 3; break;
        case GL::CubeMapDir::BACK:  return 4; break;
        case GL::CubeMapDir::FRONT: return 5; break;
    }
    ASSERT(false);
    return 0;
}

