#ifndef TEXTUREFACTORY_H
#define TEXTUREFACTORY_H

#include "Bang/Map.h"
#include "Bang/Path.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureCubeMap.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture2D;
FORWARD class TextureCubeMap;

class TextureFactory
{
public:
    static Texture2D* GetBangB64Icon();
    static Texture2D* GetBangB512Icon();
    static Texture2D* GetBangB2048Icon();
    static Texture2D* GetBang2048Icon();
    static Texture2D* GetButtonIdle();
    static Texture2D* GetButtonDown();
    static Texture2D* GetInnerShadow();
    static Texture2D* GetOuterShadow();
    static Texture2D* GetRightArrowIcon();
    static Texture2D* GetDownArrowIcon();
    static Texture2D* GetCheckIcon();
    static Texture2D* GetAudioIcon();
    static Texture2D* GetCircleIcon();
    static Texture2D* GetSunIcon();
    static Texture2D* GetLightBulbIcon();
    static Texture2D* GetInfoIcon();
    static Texture2D* GetWarningIcon();
    static Texture2D* GetErrorIcon();
    static Texture2D* GetCheckerboard();
    static Texture2D* GetWhiteTexture();
    static Texture2D* GetBRDFLUTTexture();
    static Texture2D* Get9SliceBorder();
    static Texture2D* Get9SliceRoundRectTexture();
    static Texture2D* Get9SliceRoundRectBorderTexture();

    static TextureCubeMap* GetWhiteTextureCubeMap();
    static TextureCubeMap* GetDefaultTextureCubeMap();

    static Texture2D* GetTexture2D(const String &filename);
    static Texture2D* GetTexture2D(const String &filename, const Path &dir);
    static Texture2D* GetTexture2D(const Path &path);
    static TextureCubeMap* GetTextureCubeMap(const String &filename);
    static TextureCubeMap* GetTextureCubeMap(const String &filename, const Path &dir);
    static TextureCubeMap* GetTextureCubeMap(const Path &path);

protected:
    TextureFactory();
    virtual ~TextureFactory() = default;

    static TextureFactory* GetInstance();

private:
    Map<Path, RH<Texture2D>> m_texture2DCache;
    Map<Path, RH<TextureCubeMap>> m_textureCubeMapsCache;

    // Cache most used paths
    Path m_whiteTexturePath   = Path::Empty;
    Path m_whiteTextureCMPath = Path::Empty;
    Path m_brdfLutTexturePath = Path::Empty;

    friend class Resources;
};

NAMESPACE_BANG_END

#endif // TEXTUREFACTORY_H

