#include "Bang/TextureFactory.h"

#include "Bang/GL.h"
#include "Bang/Map.tcc"
#include "Bang/Paths.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Texture2D.h"
#include "Bang/Texture3D.h"
#include "Bang/TextureCubeMap.h"

using namespace Bang;

TextureFactory::TextureFactory()
{
    m_whiteTexturePath = EPATH("Textures").Append("White.png");
    m_whiteTextureCMPath = EPATH("Textures").Append("WhiteCM.texcm");
    m_brdfLutTexturePath = EPATH("Textures").Append("BRDF_LUT.png");
}

Texture2D *TextureFactory::GetBangB64Icon()
{
    return TextureFactory::GetTexture2D("LogoBang_B_64.png");
}

Texture2D *TextureFactory::GetBangB512Icon()
{
    return TextureFactory::GetTexture2D("LogoBang_B_512.png");
}

Texture2D *TextureFactory::GetBangB2048Icon()
{
    return TextureFactory::GetTexture2D("LogoBang_B_2048.png");
}

Texture2D *TextureFactory::GetBang2048Icon()
{
    return TextureFactory::GetTexture2D("LogoBang_2048.png");
}

Texture2D *TextureFactory::GetButtonIdle()
{
    return TextureFactory::GetTexture2D("ButtonIdle.png");
}

Texture2D *TextureFactory::GetButtonDown()
{
    return TextureFactory::GetTexture2D("ButtonDown.png");
}

Texture2D *TextureFactory::GetInnerShadow()
{
    return TextureFactory::GetTexture2D("InnerShadow.png");
}

Texture2D *TextureFactory::GetOuterShadow()
{
    return TextureFactory::GetTexture2D("OuterShadow.png");
}

Texture2D *TextureFactory::GetRightArrowIcon()
{
    return TextureFactory::GetTexture2D("RightArrow.png");
}

Texture2D *TextureFactory::GetUpArrowIcon()
{
    return TextureFactory::GetTexture2D("UpArrow.png");
}

Texture2D *TextureFactory::GetDownArrowIcon()
{
    return TextureFactory::GetTexture2D("DownArrow.png");
}

Texture2D *TextureFactory::GetCheckIcon()
{
    return TextureFactory::GetTexture2D("Check.png");
}

Texture2D *TextureFactory::GetAudioIcon()
{
    return TextureFactory::GetTexture2D("Audio.png");
}

Texture2D *TextureFactory::GetCircleIcon()
{
    return TextureFactory::GetTexture2D("Circle.png");
}

Texture2D *TextureFactory::GetSunIcon()
{
    return TextureFactory::GetTexture2D("Sun.png");
}

Texture2D *TextureFactory::GetLightBulbIcon()
{
    return TextureFactory::GetTexture2D("LightBulb.png");
}

Texture2D *Bang::TextureFactory::GetInfoIcon()
{
    return TextureFactory::GetTexture2D("Info.png");
}

Texture2D *TextureFactory::GetWarningIcon()
{
    return TextureFactory::GetTexture2D("Warn.png");
}

Texture2D *Bang::TextureFactory::GetErrorIcon()
{
    return TextureFactory::GetTexture2D("Error.png");
}

Texture2D *TextureFactory::GetCheckerboard()
{
    Texture2D *tex = GetTexture2D("Checkerboard.png");
    tex->SetFilterMode(GL::FilterMode::NEAREST);
    return TextureFactory::GetTexture2D("Checkerboard.png");
}
Texture2D *TextureFactory::GetWhiteTexture()
{
    return GetTexture2D(TextureFactory::GetInstance()->m_whiteTexturePath);
}

Texture3D *TextureFactory::GetWhiteTexture3D()
{
    TextureFactory *tf = TextureFactory::GetInstance();
    if (!tf->m_whiteTexture3D)
    {
        tf->m_whiteTexture3D = Assets::Create<Texture3D>();
        tf->m_whiteTexture3D.Get()->CreateEmpty(Vector3i(1));
    }
    return tf->m_whiteTexture3D.Get();
}

Texture2D *TextureFactory::GetBRDFLUTTexture()
{
    return TextureFactory::GetTexture2D(
        TextureFactory::GetInstance()->m_brdfLutTexturePath);
}

Texture2D *TextureFactory::Get9SliceBorder()
{
    Texture2D *tex = TextureFactory::GetTexture2D("Border_9s.png");
    tex->SetFilterMode(GL::FilterMode::NEAREST);
    return tex;
}

Texture2D *TextureFactory::Get9SliceRoundRectTexture()
{
    Texture2D *tex = TextureFactory::GetTexture2D("RRect_9s.png");
    tex->SetFilterMode(GL::FilterMode::NEAREST);
    return tex;
}

Texture2D *TextureFactory::Get9SliceRoundRectBorderTexture()
{
    return TextureFactory::GetTexture2D("RRectBorder_9s.png");
}

TextureCubeMap *TextureFactory::GetWhiteTextureCubeMap()
{
    return TextureFactory::GetTextureCubeMap(
        TextureFactory::GetInstance()->m_whiteTextureCMPath);
}

TextureCubeMap *TextureFactory::GetDefaultSkybox()
{
    return TextureFactory::GetTextureCubeMap("DefaultSkybox.texcm");
}

Texture2D *TextureFactory::GetTexture2D(const String &filename)
{
    return TextureFactory::GetTexture2D(filename, EPATH("Textures"));
}

Texture2D *TextureFactory::GetTexture2D(const String &filename, const Path &dir)
{
    Path path = dir.Append(filename);
    return GetTexture2D(path);
}

Texture2D *TextureFactory::GetTexture2D(const Path &path)
{
    TextureFactory *tf = TextureFactory::GetInstance();
    Texture2D *tex = nullptr;

    if (tf->m_texture2DCache.ContainsKey(path))
    {
        tex = tf->m_texture2DCache.Get(path).Get();
    }
    else
    {
        bool initialized = (Assets::GetCached<Texture2D>(path) != nullptr);
        AH<Texture2D> texAH = Assets::Load<Texture2D>(path);
        if (texAH && !initialized)
        {
            GL::Push(GL::BindTarget::TEXTURE_2D);

            texAH.Get()->Bind();
            texAH.Get()->GenerateMipMaps();
            texAH.Get()->SetFilterMode(GL::FilterMode::TRILINEAR_LL);
            texAH.Get()->SetWrapMode(GL::WrapMode::REPEAT);

            GL::Pop(GL::BindTarget::TEXTURE_2D);
        }

        tex = texAH.Get();
        tf->m_texture2DCache.Add(path, texAH);
    }

    return tex;
}

TextureCubeMap *TextureFactory::GetTextureCubeMap(const String &filename)
{
    return TextureFactory::GetTextureCubeMap(filename, EPATH("Textures"));
}

TextureCubeMap *TextureFactory::GetTextureCubeMap(const String &filename,
                                                  const Path &dir)
{
    Path path = dir.Append(filename);
    return GetTextureCubeMap(path);
}

TextureCubeMap *TextureFactory::GetTextureCubeMap(const Path &path)
{
    TextureCubeMap *texCM = nullptr;
    if (TextureFactory *tf = TextureFactory::GetInstance())
    {
        if (tf->m_textureCubeMapsCache.ContainsKey(path))
        {
            texCM = tf->m_textureCubeMapsCache.Get(path).Get();
        }
        else
        {
            AH<TextureCubeMap> texCMAH = Assets::Load<TextureCubeMap>(path);
            texCM = texCMAH.Get();
            tf->m_textureCubeMapsCache.Add(path, texCMAH);
        }
    }
    return texCM;
}

TextureFactory *TextureFactory::GetInstance()
{
    return Assets::GetInstance()->GetTextureFactory();
}
