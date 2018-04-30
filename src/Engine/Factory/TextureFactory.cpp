#include "Bang/TextureFactory.h"

#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureCubeMap.h"

USING_NAMESPACE_BANG

RH<Texture2D> TextureFactory::GetBangB64Icon()
{ return GetIconTexture("LogoBang_B_64"); }
RH<Texture2D> TextureFactory::GetBangB512Icon()
{ return GetIconTexture("LogoBang_B_512"); }
RH<Texture2D> TextureFactory::GetBangB2048Icon()
{ return GetIconTexture("LogoBang_B_2048"); }
RH<Texture2D> TextureFactory::GetBang2048Icon()
{ return GetIconTexture("LogoBang_2048"); }
RH<Texture2D> TextureFactory::GetRightArrowIcon()
{ return GetIconTexture("RightArrow"); }
RH<Texture2D> TextureFactory::GetDownArrowIcon()
{ return GetIconTexture("DownArrow"); }
RH<Texture2D> TextureFactory::GetCheckIcon()
{ return GetIconTexture("Check"); }
RH<Texture2D> TextureFactory::GetAudioIcon()
{ return GetIconTexture("Audio"); }
RH<Texture2D> TextureFactory::GetCircleIcon()
{ return GetIconTexture("Circle"); }
RH<Texture2D> TextureFactory::GetSunIcon()
{ return GetIconTexture("Sun"); }
RH<Texture2D> TextureFactory::GetLightBulbIcon()
{ return GetIconTexture("LightBulb"); }
RH<Texture2D> TextureFactory::GetWarningIcon()
{ return GetIconTexture("Warn"); }
RH<Texture2D> TextureFactory::GetCheckerboard()
{
    RH<Texture2D> tex = GetIconTexture("Checkerboard");
    tex.Get()->SetFilterMode(GL::FilterMode::Nearest);
    return tex;
}
RH<Texture2D> TextureFactory::GetWhiteTexture()
{ return GetIconTexture("White"); }
RH<TextureCubeMap> TextureFactory::GetWhiteTextureCubeMap()
{
    Path path = EPATH("Icons/WhiteCM.texcm");
    RH<TextureCubeMap> tcm = Resources::Load<TextureCubeMap>(path);
    Resources::SetPermanent(path, true);
    Resources::SetPermanent(tcm.Get(), true);
    return tcm;
}

RH<TextureCubeMap> TextureFactory::GetDefaultTextureCubeMap()
{
    Path path = EPATH("Icons/DefaultSkybox.texcm");
    RH<TextureCubeMap> tcm = Resources::Load<TextureCubeMap>(path);
    Resources::SetPermanent(path, true);
    Resources::SetPermanent(tcm.Get(), true);
    return tcm;
}

RH<Texture2D> TextureFactory::GetIconTexture(const String &filename,
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

RH<Texture2D> TextureFactory::GetIconTexture(const String &filename)
{
    return TextureFactory::GetIconTexture(filename,
                                       Paths::GetEngineAssetsDir()
                                       .Append("Icons"));
}

