#include "Bang/TextureFactory.h"

#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureCubeMap.h"

USING_NAMESPACE_BANG

RH<Texture2D> TextureFactory::GetBangB64Icon()
{ return GetTexture("LogoBang_B_64"); }
RH<Texture2D> TextureFactory::GetBangB512Icon()
{ return GetTexture("LogoBang_B_512"); }
RH<Texture2D> TextureFactory::GetBangB2048Icon()
{ return GetTexture("LogoBang_B_2048"); }
RH<Texture2D> TextureFactory::GetBang2048Icon()
{ return GetTexture("LogoBang_2048"); }
RH<Texture2D> TextureFactory::GetRightArrowIcon()
{ return GetTexture("RightArrow"); }
RH<Texture2D> TextureFactory::GetDownArrowIcon()
{ return GetTexture("DownArrow"); }
RH<Texture2D> TextureFactory::GetCheckIcon()
{ return GetTexture("Check"); }
RH<Texture2D> TextureFactory::GetAudioIcon()
{ return GetTexture("Audio"); }
RH<Texture2D> TextureFactory::GetCircleIcon()
{ return GetTexture("Circle"); }
RH<Texture2D> TextureFactory::GetSunIcon()
{ return GetTexture("Sun"); }
RH<Texture2D> TextureFactory::GetLightBulbIcon()
{ return GetTexture("LightBulb"); }
RH<Bang::Texture2D> Bang::TextureFactory::GetInfoIcon()
{ return GetTexture("Info"); }
RH<Texture2D> TextureFactory::GetWarningIcon()
{ return GetTexture("Warn"); }
RH<Bang::Texture2D> Bang::TextureFactory::GetErrorIcon()
{ return GetTexture("Error"); }
RH<Texture2D> TextureFactory::GetCheckerboard()
{
    RH<Texture2D> tex = GetTexture("Checkerboard");
    tex.Get()->SetFilterMode(GL::FilterMode::Nearest);
    return tex;
}
RH<Texture2D> TextureFactory::GetWhiteTexture()
{ 
    return GetTexture("White"); 
}

RH<Texture2D> TextureFactory::Get9SliceRoundRectTexture()
{
    return GetTexture("RRect_9s.png");
}
RH<Texture2D> TextureFactory::Get9SliceRoundRectBorderTexture()
{
    return GetTexture("RRectBorder_9s.png");
}

RH<TextureCubeMap> TextureFactory::GetWhiteTextureCubeMap()
{
    Path path = GetTexturesDir().Append("WhiteCM.texcm");
    RH<TextureCubeMap> tcm = Resources::Load<TextureCubeMap>(path);
    Resources::SetPermanent(path, true);
    Resources::SetPermanent(tcm.Get(), true);
    return tcm;
}

RH<TextureCubeMap> TextureFactory::GetDefaultTextureCubeMap()
{
    Path path = GetTexturesDir().Append("DefaultSkybox.texcm");
    RH<TextureCubeMap> tcm = Resources::Load<TextureCubeMap>(path);
    Resources::SetPermanent(path, true);
    Resources::SetPermanent(tcm.Get(), true);
    return tcm;
}

RH<Texture2D> TextureFactory::GetTexture(const String &filename,
                                          const Path &dir)
{
    Path path = dir.Append(filename).AppendExtension("png");

    // Avoid removing of img resource when loading Texture2D[::Import()]
    // (this is why it must go before Load<Texture2D>(), and not after)
    Resources::SetPermanent(path, true);

    bool initialized = (Resources::GetCached<Texture2D>(path) != nullptr);
    RH<Texture2D> iconTex = Resources::Load<Texture2D>(path);
    if (iconTex && !initialized)
    {
        GLId prevTexID = GL::GetBoundId(iconTex.Get()->GetGLBindTarget());

        iconTex.Get()->Bind();
        iconTex.Get()->GenerateMipMaps();
        iconTex.Get()->SetAlphaCutoff(0.5f);
        iconTex.Get()->SetFilterMode(GL::FilterMode::Trilinear_LL);
        iconTex.Get()->SetWrapMode(GL::WrapMode::Repeat);

        GL::Bind(iconTex.Get()->GetGLBindTarget(), prevTexID);

        Resources::SetPermanent(iconTex.Get(), true);
    }

    return iconTex;
}

Path TextureFactory::GetTexturesDir()
{
    return Paths::GetEngineAssetsDir().Append("Textures");
}

RH<Texture2D> TextureFactory::GetTexture(const String &filename)
{
    return TextureFactory::GetTexture(filename, GetTexturesDir());
}

