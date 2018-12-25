#include "Bang/TextureCubeMap.h"

#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/GUID.h"
#include "Bang/IEventsAsset.h"
#include "Bang/Image.h"
#include "Bang/Math.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Texture2D.h"

namespace Bang
{
class Asset;
}

using namespace Bang;

TextureCubeMap::TextureCubeMap() : Texture(GL::TextureTarget::TEXTURE_CUBE_MAP)
{
    SetFormat(GL::ColorFormat::RGBA8);
    CreateEmpty(1);

    SetFilterMode(GL::FilterMode::BILINEAR);
    SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);
}

TextureCubeMap::~TextureCubeMap()
{
}

void TextureCubeMap::CreateEmpty(uint size)
{
    for (GL::CubeMapDir cubeMapDir : GL::GetAllCubeMapDirs())
    {
        Fill(cubeMapDir, nullptr, size, GetColorComp(), GetDataType());
    }
}

bool TextureCubeMap::Resize(uint size)
{
    if (size != GetSize())
    {
        CreateEmpty(size);
        return true;
    }
    return false;
}

void TextureCubeMap::Fill(GL::CubeMapDir cubeMapDir,
                          const Byte *newData,
                          uint size,
                          GL::ColorComp inputDataColorComp,
                          GL::DataType inputDataType)
{
    m_size = size;

    GL::Push(GetGLBindTarget());

    Bind();
    GL::TexImage2D(SCAST<GL::TextureTarget>(cubeMapDir),
                   GetSize(),
                   GetSize(),
                   GetFormat(),
                   inputDataColorComp,
                   inputDataType,
                   newData);

    GL::Pop(GetGLBindTarget());

    PropagateAssetChanged();
}

uint TextureCubeMap::GetSize() const
{
    return m_size;
}

void TextureCubeMap::SetSideTexture(GL::CubeMapDir cubeMapDir, Texture2D *tex)
{
    if (GetSideTexture(cubeMapDir).Get() != tex)
    {
        if (GetSideTexture(cubeMapDir).Get())
        {
            GetSideTexture(cubeMapDir)
                .Get()
                ->EventEmitter<IEventsAsset>::UnRegisterListener(this);
        }

        FillCubeMapDir(cubeMapDir, tex ? &tex->GetImage() : nullptr);
        m_sideTextures[TextureCubeMap::GetDirIndex(cubeMapDir)].Set(tex);

        if (GetSideTexture(cubeMapDir).Get())
        {
            GetSideTexture(cubeMapDir)
                .Get()
                ->EventEmitter<IEventsAsset>::RegisterListener(this);
        }

        PropagateAssetChanged();
    }
}

void TextureCubeMap::FillCubeMapDir(GL::CubeMapDir dir, const Image *img)
{
    Fill(dir,
         (img ? img->GetData() : nullptr),
         (img ? Math::Min(img->GetWidth(), img->GetHeight()) : 0),
         GL::ColorComp::RGBA,
         GL::DataType::UNSIGNED_BYTE);
}

AH<Texture2D> TextureCubeMap::GetSideTexture(GL::CubeMapDir cubeMapDir) const
{
    return m_sideTextures[TextureCubeMap::GetDirIndex(cubeMapDir)];
}

void TextureCubeMap::Import(const Image &topImage,
                            const Image &botImage,
                            const Image &leftImage,
                            const Image &rightImage,
                            const Image &frontImage,
                            const Image &backImage)
{
    SetFormat(GL::ColorFormat::RGBA8);

    std::array<Image, 6> imgs = {
        {topImage, botImage, leftImage, rightImage, frontImage, backImage}};
    for (uint i = 0; i < GL::GetAllCubeMapDirs().size(); ++i)
    {
        const Image &img = imgs[i];
        if (img.GetData())
        {
            GL::CubeMapDir cubeMapDir = GL::GetAllCubeMapDirs()[i];

            Fill(cubeMapDir,
                 img.GetData(),
                 Math::Min(img.GetWidth(), img.GetHeight()),
                 GL::ColorComp::RGBA,
                 GL::DataType::UNSIGNED_BYTE);
        }
    }
}

void TextureCubeMap::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);

    if (metaNode.Contains("TopImage"))
    {
        SetSideTexture(
            GL::CubeMapDir::TOP,
            Assets::Load<Texture2D>(metaNode.Get<GUID>("TopImage")).Get());
    }
    if (metaNode.Contains("BotImage"))
    {
        SetSideTexture(
            GL::CubeMapDir::BOT,
            Assets::Load<Texture2D>(metaNode.Get<GUID>("BotImage")).Get());
    }
    if (metaNode.Contains("LeftImage"))
    {
        SetSideTexture(
            GL::CubeMapDir::LEFT,
            Assets::Load<Texture2D>(metaNode.Get<GUID>("LeftImage")).Get());
    }
    if (metaNode.Contains("RightImage"))
    {
        SetSideTexture(
            GL::CubeMapDir::RIGHT,
            Assets::Load<Texture2D>(metaNode.Get<GUID>("RightImage")).Get());
    }
    if (metaNode.Contains("FrontImage"))
    {
        SetSideTexture(
            GL::CubeMapDir::FRONT,
            Assets::Load<Texture2D>(metaNode.Get<GUID>("FrontImage")).Get());
    }
    if (metaNode.Contains("BackImage"))
    {
        SetSideTexture(
            GL::CubeMapDir::BACK,
            Assets::Load<Texture2D>(metaNode.Get<GUID>("BackImage")).Get());
    }
}

void TextureCubeMap::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);

    if (GetSideTexture(GL::CubeMapDir::TOP))
    {
        metaNode->Set("TopImage",
                      GetSideTexture(GL::CubeMapDir::TOP).Get()->GetGUID());
    }
    if (GetSideTexture(GL::CubeMapDir::BOT))
    {
        metaNode->Set("BotImage",
                      GetSideTexture(GL::CubeMapDir::BOT).Get()->GetGUID());
    }
    if (GetSideTexture(GL::CubeMapDir::LEFT))
    {
        metaNode->Set("LeftImage",
                      GetSideTexture(GL::CubeMapDir::LEFT).Get()->GetGUID());
    }
    if (GetSideTexture(GL::CubeMapDir::RIGHT))
    {
        metaNode->Set("RightImage",
                      GetSideTexture(GL::CubeMapDir::RIGHT).Get()->GetGUID());
    }
    if (GetSideTexture(GL::CubeMapDir::FRONT))
    {
        metaNode->Set("FrontImage",
                      GetSideTexture(GL::CubeMapDir::FRONT).Get()->GetGUID());
    }
    if (GetSideTexture(GL::CubeMapDir::BACK))
    {
        metaNode->Set("BackImage",
                      GetSideTexture(GL::CubeMapDir::BACK).Get()->GetGUID());
    }
}

void TextureCubeMap::Import(const Path &textureCubeMapFilepath)
{
    ImportMetaFromFile(textureCubeMapFilepath);
}

GL::BindTarget TextureCubeMap::GetGLBindTarget() const
{
    return GL::BindTarget::TEXTURE_CUBE_MAP;
}

void TextureCubeMap::OnImported(Asset *res)
{
    for (GL::CubeMapDir cubeMapDir : GL::GetAllCubeMapDirs())
    {
        if (res == GetSideTexture(cubeMapDir).Get())
        {
            Texture2D *tex = DCAST<Texture2D *>(res);
            ASSERT(tex);
            FillCubeMapDir(cubeMapDir, &(tex->GetImage()));
        }
    }
}

unsigned int TextureCubeMap::GetDirIndex(GL::CubeMapDir dir)
{
    switch (dir)
    {
        case GL::CubeMapDir::TOP: return 0; break;
        case GL::CubeMapDir::BOT: return 1; break;
        case GL::CubeMapDir::RIGHT: return 2; break;
        case GL::CubeMapDir::LEFT: return 3; break;
        case GL::CubeMapDir::BACK: return 4; break;
        case GL::CubeMapDir::FRONT: return 5; break;
    }
    ASSERT(false);
    return 0;
}
