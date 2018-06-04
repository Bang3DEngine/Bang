#ifndef TEXTUREFACTORY_H
#define TEXTUREFACTORY_H

#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture2D;
FORWARD class TextureCubeMap;

class TextureFactory
{
public:
    static RH<Texture2D> GetBangB64Icon();
    static RH<Texture2D> GetBangB512Icon();
    static RH<Texture2D> GetBangB2048Icon();
    static RH<Texture2D> GetBang2048Icon();
    static RH<Texture2D> GetRightArrowIcon();
    static RH<Texture2D> GetDownArrowIcon();
    static RH<Texture2D> GetCheckIcon();
    static RH<Texture2D> GetAudioIcon();
    static RH<Texture2D> GetCircleIcon();
    static RH<Texture2D> GetSunIcon();
    static RH<Texture2D> GetLightBulbIcon();
    static RH<Texture2D> GetInfoIcon();
    static RH<Texture2D> GetWarningIcon();
    static RH<Texture2D> GetErrorIcon();
    static RH<Texture2D> GetCheckerboard();
    static RH<Texture2D> GetWhiteTexture();
    static RH<Texture2D> Get9SliceRoundRectTexture();
    static RH<Texture2D> Get9SliceRoundRectBorderTexture();

    static RH<TextureCubeMap> GetWhiteTextureCubeMap();
    static RH<TextureCubeMap> GetDefaultTextureCubeMap();

    static RH<Texture2D> GetTexture(const String &filename, const Path &dir);

private:
    TextureFactory() = default;
    virtual ~TextureFactory() = default;

    static Path GetTexturesDir();
    static RH<Texture2D> GetTexture(const String &filename);
};

NAMESPACE_BANG_END

#endif // TEXTUREFACTORY_H

