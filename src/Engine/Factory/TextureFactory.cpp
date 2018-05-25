#include "Bang/TextureFactory.h"

#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureCubeMap.h"

USING_NAMESPACE_BANG

RH<Texture2D> TextureFactory::GetBangB64Icon()
{ return GetTexture2D("LogoBang_B_64.png"); }
RH<Texture2D> TextureFactory::GetBangB512Icon()
{ return GetTexture2D("LogoBang_B_512.png"); }
RH<Texture2D> TextureFactory::GetBangB2048Icon()
{ return GetTexture2D("LogoBang_B_2048.png"); }
RH<Texture2D> TextureFactory::GetBang2048Icon()
{ return GetTexture2D("LogoBang_2048.png"); }
RH<Texture2D> TextureFactory::GetRightArrowIcon()
{ return GetTexture2D("RightArrow.png"); }
RH<Texture2D> TextureFactory::GetDownArrowIcon()
{ return GetTexture2D("DownArrow.png"); }
RH<Texture2D> TextureFactory::GetCheckIcon()
{ return GetTexture2D("Check.png"); }
RH<Texture2D> TextureFactory::GetAudioIcon()
{ return GetTexture2D("Audio.png"); }
RH<Texture2D> TextureFactory::GetCircleIcon()
{ return GetTexture2D("Circle.png"); }
RH<Texture2D> TextureFactory::GetSunIcon()
{ return GetTexture2D("Sun.png"); }
RH<Texture2D> TextureFactory::GetLightBulbIcon()
{ return GetTexture2D("LightBulb.png"); }
RH<Bang::Texture2D> Bang::TextureFactory::GetInfoIcon()
{ return GetTexture2D("Info.png"); }
RH<Texture2D> TextureFactory::GetWarningIcon()
{ return GetTexture2D("Warn.png"); }
RH<Bang::Texture2D> Bang::TextureFactory::GetErrorIcon()
{ return GetTexture2D("Error.png"); }
RH<Texture2D> TextureFactory::GetCheckerboard()
{
    RH<Texture2D> tex = GetTexture2D("Checkerboard.png");
    tex.Get()->SetFilterMode(GL::FilterMode::NEAREST);
    return TextureFactory::GetTexture2D("Checkerboard.png");
}
RH<Texture2D> TextureFactory::GetWhiteTexture()
{ return GetTexture2D("White.png"); }
RH<Texture2D> TextureFactory::GetBRDFLUTTexture()
{ return TextureFactory::GetTexture2D("BRDF_LUT.png"); }
RH<Texture2D> TextureFactory::Get9SliceRoundRectTexture()
{
    Texture2D *tex = TextureFactory::GetTexture2D("RRect_9s.png").Get();
    tex->SetFilterMode(GL::FilterMode::NEAREST);
    return TextureFactory::GetTexture2D("RRect_9s.png");
}
RH<Texture2D> TextureFactory::Get9SliceRoundRectBorderTexture()
{ return TextureFactory::GetTexture2D("RRectBorder_9s.png"); }
RH<TextureCubeMap> TextureFactory::GetWhiteTextureCubeMap()
{ return TextureFactory::GetTextureCubeMap("WhiteCM.texcm"); }
RH<TextureCubeMap> TextureFactory::GetDefaultTextureCubeMap()
{ return TextureFactory::GetTextureCubeMap("DefaultSkybox.texcm"); }

RH<Texture2D> TextureFactory::GetTexture2D(const String &filename)
{
    return TextureFactory::GetTexture2D(filename, EPATH("Textures"));
}

RH<Texture2D> TextureFactory::GetTexture2D(const String &filename, const Path &dir)
{
    Path path = dir.Append(filename);

    // Avoid removing of img resource when loading Texture2D[::Import()]
    // (this is why it must go before Load<Texture2D>(), and not after)
    Resources::SetPermanent(path, true);

    bool initialized = (Resources::GetCached<Texture2D>(path) != nullptr);
    RH<Texture2D> tex = Resources::Load<Texture2D>(path);
    if (tex && !initialized)
    {
        GLId prevTexID = GL::GetBoundId(tex.Get()->GetGLBindTarget());

        tex.Get()->Bind();
        tex.Get()->GenerateMipMaps();
        tex.Get()->SetAlphaCutoff(0.5f);
        tex.Get()->SetFilterMode(GL::FilterMode::TRILINEAR_LL);
        tex.Get()->SetWrapMode(GL::WrapMode::REPEAT);

        GL::Bind(tex.Get()->GetGLBindTarget(), prevTexID);

        Resources::SetPermanent(tex.Get(), true);
    }

    return tex;
}

RH<TextureCubeMap> TextureFactory::GetTextureCubeMap(const String &filename)
{
    return TextureFactory::GetTextureCubeMap(filename, EPATH("Textures"));
}

RH<TextureCubeMap> TextureFactory::GetTextureCubeMap(const String &filename, const Path &dir)
{
    Path path = dir.Append(filename);

    // Avoid removing of img resource when loading Texture2D[::Import()]
    // (this is why it must go before Load<Texture2D>(), and not after)
    Resources::SetPermanent(path, true);

    bool initialized = (Resources::GetCached<TextureCubeMap>(path) != nullptr);
    RH<TextureCubeMap> texCM = Resources::Load<TextureCubeMap>(path);
    if (texCM && !initialized)
    {
        Resources::SetPermanent(texCM.Get(), true);
    }

    return texCM;
}
