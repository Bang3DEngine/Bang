#include "Bang/TextureFactory.h"

#include "Bang/Paths.h"
#include "Bang/Resources.h"

USING_NAMESPACE_BANG

Texture2D* TextureFactory::GetBangB64Icon()
{
    return GetTexture2D("LogoBang_B_64.png");
}

Texture2D* TextureFactory::GetBangB512Icon()
{
    return GetTexture2D("LogoBang_B_512.png");
}

Texture2D* TextureFactory::GetBangB2048Icon()
{
    return GetTexture2D("LogoBang_B_2048.png");
}

Texture2D* TextureFactory::GetBang2048Icon()
{
    return GetTexture2D("LogoBang_2048.png");
}

Texture2D* TextureFactory::GetRightArrowIcon()
{
    return GetTexture2D("RightArrow.png");
}

Texture2D* TextureFactory::GetDownArrowIcon()
{
    return GetTexture2D("DownArrow.png");
}

Texture2D* TextureFactory::GetCheckIcon()
{
    return GetTexture2D("Check.png");
}

Texture2D* TextureFactory::GetAudioIcon()
{
    return GetTexture2D("Audio.png");
}

Texture2D* TextureFactory::GetCircleIcon()
{
    return GetTexture2D("Circle.png");
}

Texture2D* TextureFactory::GetSunIcon()
{
    return GetTexture2D("Sun.png");
}

Texture2D* TextureFactory::GetLightBulbIcon()
{
    return GetTexture2D("LightBulb.png");
}

Texture2D* Bang::TextureFactory::GetInfoIcon()
{
    return GetTexture2D("Info.png");
}

Texture2D* TextureFactory::GetWarningIcon()
{
    return GetTexture2D("Warn.png");
}

Texture2D* Bang::TextureFactory::GetErrorIcon()
{
    return GetTexture2D("Error.png");

}

Texture2D* TextureFactory::GetCheckerboard()
{
    Texture2D *tex = GetTexture2D("Checkerboard.png");
    tex->SetFilterMode(GL::FilterMode::NEAREST);
    return TextureFactory::GetTexture2D("Checkerboard.png");
}
Texture2D* TextureFactory::GetWhiteTexture()
{
    return GetTexture2D("White.png");
}

Texture2D* TextureFactory::GetBRDFLUTTexture()
{
    return TextureFactory::GetTexture2D("BRDF_LUT.png");
}

Texture2D* TextureFactory::Get9SliceRoundRectTexture()
{
    Texture2D *tex = TextureFactory::GetTexture2D("RRect_9s.png");
    tex->SetFilterMode(GL::FilterMode::NEAREST);
    return TextureFactory::GetTexture2D("RRect_9s.png");
}

Texture2D* TextureFactory::Get9SliceRoundRectBorderTexture()
{
    return TextureFactory::GetTexture2D("RRectBorder_9s.png");
}

TextureCubeMap* TextureFactory::GetWhiteTextureCubeMap()
{
    return TextureFactory::GetTextureCubeMap("WhiteCM.texcm");
}

TextureCubeMap* TextureFactory::GetDefaultTextureCubeMap()
{
    return TextureFactory::GetTextureCubeMap("DefaultSkybox.texcm");
}

Texture2D* TextureFactory::GetTexture2D(const String &filename)
{
    return TextureFactory::GetTexture2D(filename, EPATH("Textures"));
}

Texture2D* TextureFactory::GetTexture2D(const String &filename, const Path &dir)
{
    TextureFactory *tf = TextureFactory::GetInstance();
    Path path = dir.Append(filename);
    Texture2D *tex = nullptr;

    if (tf->m_texture2DCache.ContainsKey(path))
    {
        tex = tf->m_texture2DCache.Get(path).Get();
    }
    else
    {
        bool initialized = (Resources::GetCached<Texture2D>(path) != nullptr);
        RH<Texture2D> texRH = Resources::Load<Texture2D>(path);
        if (texRH && !initialized)
        {
            GL::Push(GL::BindTarget::TEXTURE_2D);

            texRH.Get()->Bind();
            texRH.Get()->GenerateMipMaps();
            texRH.Get()->SetAlphaCutoff(0.5f);
            texRH.Get()->SetFilterMode(GL::FilterMode::TRILINEAR_LL);
            texRH.Get()->SetWrapMode(GL::WrapMode::REPEAT);

            GL::Pop(GL::BindTarget::TEXTURE_2D);
        }

        tex = texRH.Get();
        tf->m_texture2DCache.Add(path, texRH);
    }

    return tex;
}

TextureCubeMap* TextureFactory::GetTextureCubeMap(const String &filename)
{
    return TextureFactory::GetTextureCubeMap(filename, EPATH("Textures"));
}

TextureCubeMap* TextureFactory::GetTextureCubeMap(const String &filename, const Path &dir)
{
    TextureFactory *tf = TextureFactory::GetInstance();
    Path path = dir.Append(filename);
    TextureCubeMap *texCM = nullptr;

    if (tf->m_textureCubeMapsCache.ContainsKey(path))
    {
        texCM = tf->m_textureCubeMapsCache.Get(path).Get();
    }
    else
    {
        RH<TextureCubeMap> texCMRH = Resources::Load<TextureCubeMap>(path);
        texCM = texCMRH.Get();
        tf->m_textureCubeMapsCache.Add(path, texCMRH);
    }

    return texCM;
}

TextureFactory *TextureFactory::GetInstance()
{
    return Resources::GetInstance()->GetTextureFactory();
}
