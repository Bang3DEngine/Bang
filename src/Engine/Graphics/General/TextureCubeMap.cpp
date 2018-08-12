#include "Bang/TextureCubeMap.h"

#include "Bang/Path.h"
#include "Bang/XMLNode.h"
#include "Bang/Resources.h"

USING_NAMESPACE_BANG

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
void TextureCubeMap::CreateEmpty(const Vector2i &size)
{
    ASSERT_MSG(size.x == size.y, "CubeMaps must have the same width and height.");
    SetWidth(size.x);
    SetHeight(size.y);

    for (GL::CubeMapDir cubeMapDir : GL::GetAllCubeMapDirs())
    {
        Fill(cubeMapDir, nullptr, size.x, GetColorComp(), GetDataType());
    }
}

void TextureCubeMap::Resize(const Vector2i &size)
{
    ASSERT_MSG(size.x == size.y, "CubeMaps must have the same width and height.");
    if (size != GetSize())
    {
        CreateEmpty(size.x, size.y);
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

    PropagateResourceChanged();
}

void TextureCubeMap::SetSideTexture(GL::CubeMapDir cubeMapDir, Texture2D *tex)
{
    if (GetSideTexture(cubeMapDir).Get() != tex)
    {
        if (GetSideTexture(cubeMapDir).Get())
        {
            GetSideTexture(cubeMapDir).Get()->
                    EventEmitter<IEventsResource>::UnRegisterListener(this);
        }

        FillCubeMapDir(cubeMapDir, tex ? &tex->GetImage() : nullptr);
        m_sideTextures[ TextureCubeMap::GetDirIndex(cubeMapDir) ].Set(tex);

        if (GetSideTexture(cubeMapDir).Get())
        {
            GetSideTexture(cubeMapDir).Get()->
                    EventEmitter<IEventsResource>::RegisterListener(this);
        }

        PropagateResourceChanged();
    }
}

void TextureCubeMap::FillCubeMapDir(GL::CubeMapDir dir, const Imageb *img)
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

RH<Texture2D> TextureCubeMap::GetSideTexture(GL::CubeMapDir cubeMapDir) const
{
    return m_sideTextures[ TextureCubeMap::GetDirIndex(cubeMapDir) ];
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
    for (int i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
    {
        const Image<Byte> &img = imgs[i];
        if (img.GetData())
        {
            GL::CubeMapDir cubeMapDir = GL::GetAllCubeMapDirs()[i];

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
        SetSideTexture(GL::CubeMapDir::TOP,
                 Resources::Load<Texture2D>( xmlInfo.Get<GUID>("TopImage") ).Get() );
    }
    if (xmlInfo.Contains("BotImage"))
    {
        SetSideTexture(GL::CubeMapDir::BOT,
                 Resources::Load<Texture2D>( xmlInfo.Get<GUID>("BotImage") ).Get() );
    }
    if (xmlInfo.Contains("LeftImage"))
    {
        SetSideTexture(GL::CubeMapDir::LEFT,
                 Resources::Load<Texture2D>( xmlInfo.Get<GUID>("LeftImage") ).Get() );
    }
    if (xmlInfo.Contains("RightImage"))
    {
        SetSideTexture(GL::CubeMapDir::RIGHT,
                 Resources::Load<Texture2D>( xmlInfo.Get<GUID>("RightImage") ).Get() );
    }
    if (xmlInfo.Contains("FrontImage"))
    {
        SetSideTexture(GL::CubeMapDir::FRONT,
                 Resources::Load<Texture2D>( xmlInfo.Get<GUID>("FrontImage") ).Get() );
    }
    if (xmlInfo.Contains("BackImage"))
    {
        SetSideTexture(GL::CubeMapDir::BACK,
                 Resources::Load<Texture2D>( xmlInfo.Get<GUID>("BackImage") ).Get() );
    }
}

void TextureCubeMap::ExportXML(XMLNode *xmlInfo) const
{
    Asset::ExportXML(xmlInfo);

    if (GetSideTexture(GL::CubeMapDir::TOP))
    { xmlInfo->Set("TopImage", GetSideTexture(GL::CubeMapDir::TOP).Get()->GetGUID()); }
    if (GetSideTexture(GL::CubeMapDir::BOT))
    { xmlInfo->Set("BotImage", GetSideTexture(GL::CubeMapDir::BOT).Get()->GetGUID()); }
    if (GetSideTexture(GL::CubeMapDir::LEFT))
    { xmlInfo->Set("LeftImage", GetSideTexture(GL::CubeMapDir::LEFT).Get()->GetGUID()); }
    if (GetSideTexture(GL::CubeMapDir::RIGHT))
    { xmlInfo->Set("RightImage", GetSideTexture(GL::CubeMapDir::RIGHT).Get()->GetGUID()); }
    if (GetSideTexture(GL::CubeMapDir::FRONT))
    { xmlInfo->Set("FrontImage", GetSideTexture(GL::CubeMapDir::FRONT).Get()->GetGUID()); }
    if (GetSideTexture(GL::CubeMapDir::BACK))
    { xmlInfo->Set("BackImage", GetSideTexture(GL::CubeMapDir::BACK).Get()->GetGUID()); }
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
    for (GL::CubeMapDir cubeMapDir : GL::GetAllCubeMapDirs())
    {
        if (res == GetSideTexture(cubeMapDir).Get())
        {
            Texture2D *tex = DCAST<Texture2D*>(res);
            ASSERT(tex);
            FillCubeMapDir(cubeMapDir, &(tex->GetImage()));
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

