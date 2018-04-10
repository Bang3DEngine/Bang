#include "Bang/IconManager.h"

#include "Bang/Paths.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"

USING_NAMESPACE_BANG

RH<Texture2D> IconManager::GetBangB64Icon()
{ return GetIconTexture("LogoBang_B_64"); }
RH<Texture2D> IconManager::GetBangB512Icon()
{ return GetIconTexture("LogoBang_B_512"); }
RH<Texture2D> IconManager::GetBangB2048Icon()
{ return GetIconTexture("LogoBang_B_2048"); }
RH<Texture2D> IconManager::GetBang2048Icon()
{ return GetIconTexture("LogoBang_2048"); }
RH<Texture2D> IconManager::GetRightArrowIcon()
{ return GetIconTexture("RightArrow"); }
RH<Texture2D> IconManager::GetDownArrowIcon()
{ return GetIconTexture("DownArrow"); }
RH<Texture2D> IconManager::GetCheckIcon()
{ return GetIconTexture("Check"); }
RH<Texture2D> IconManager::GetAudioIcon()
{ return GetIconTexture("Audio"); }
RH<Texture2D> IconManager::GetCircleIcon()
{ return GetIconTexture("Circle"); }
RH<Texture2D> IconManager::GetSunIcon()
{ return GetIconTexture("Sun"); }
RH<Texture2D> IconManager::GetLightBulbIcon()
{ return GetIconTexture("LightBulb"); }
RH<Texture2D> IconManager::GetWarningIcon()
{ return GetIconTexture("Warn"); }

RH<Texture2D> IconManager::GetIconTexture(const String &filename,
                                          const Path &dir)
{
    Path path = dir.Append(filename).AppendExtension("png");

    // Avoid removing of img resource when loading Texture2D[::Import()]
    // (this is why it must go before Load<Texture2D>(), and not after)
    Resources::SetPermanent(path, true);

    RH<Texture2D> iconTex = Resources::Load<Texture2D>(path);
    if (iconTex)
    {
        GLId prevId = GL::GetBoundId(iconTex.Get()->GetGLBindTarget());
        iconTex.Get()->Bind();
        iconTex.Get()->GenerateMipMaps();
        iconTex.Get()->SetAlphaCutoff(0.5f);
        iconTex.Get()->SetFilterMode(GL::FilterMode::Trilinear_LL);
        iconTex.Get()->SetWrapMode(GL::WrapMode::Repeat);
        GL::Bind(iconTex.Get()->GetGLBindTarget(), prevId);

        Resources::SetPermanent(iconTex.Get(), true);
    }

    return iconTex;
}

RH<Texture2D> IconManager::GetIconTexture(const String &filename)
{
    return IconManager::GetIconTexture(filename,
                                       Paths::GetEngineAssetsDir()
                                       .Append("Icons"));
}

