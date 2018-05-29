#ifndef FONT_H
#define FONT_H

#include "Bang/Map.h"
#include "Bang/UMap.h"
#include "Bang/Asset.h"
#include "Bang/Mutex.h"
#include "Bang/AARect.h"
#include "Bang/Vector2.h"
#include "Bang/Resource.h"
#include "Bang/FontStyle.h"

FORWARD struct _TTF_Font;
using TTF_Font = _TTF_Font;

NAMESPACE_BANG_BEGIN

FORWARD class Texture2D;

class Font : public Asset
{
    ASSET(Font)

public:
    /**
     * @brief Structure to hold metrics for a character glyph.
     * They are all relative to the baseline.
     */
    struct GlyphMetrics
    {
        Vector2f size    = Vector2f::Zero;
        Vector2f bearing = Vector2f::Zero;
        float    advance = 0;
    };

    Texture2D* GetFontAtlas(int size) const;

    Font::GlyphMetrics GetCharMetrics(int fontSize, char c) const;
    bool HasCharacter(char c) const;
    float GetKerning(int fontSize, char leftChar, char rightChar) const;
    float GetLineSkip(int fontSize) const;
    float GetFontAscent(int fontSize) const;
    float GetFontDescent(int fontSize) const;
    float GetFontHeight(int fontSize) const;
    Vector2i GetAtlasCharRectSize(int fontSize, char c) const;
    Vector2 GetCharMinUv(int fontSize, char c) const;
    Vector2 GetCharMaxUv(int fontSize, char c) const;

    // Resource
    void Import(const Path &ttfFilepath) override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    Path m_ttfFilepath = Path::Empty;
    TTF_Font *m_referenceFont = nullptr;

    struct FontDataCache
    {
        float height, ascent, descent, lineSkip;
        UMap<char, GlyphMetrics> charMetrics;
    };

    // For each font style
    FontDataCache m_referenceFontDataCache;
    mutable UMap<int, TTF_Font*> m_openFonts;
    mutable UMap<int, RH<Texture2D>> m_cachedAtlas;
    mutable UMap<int, UMap<char, AARecti>> m_cachedAtlasCharRects;
    mutable UMap<int, String> m_cachedAtlasChars;

    Font();
    virtual ~Font();

    TTF_Font *GetReferenceFont() const;
    TTF_Font *GetTTFFont(int fontSize) const;
    bool HasFontSizeLoaded(int fontSize) const;
    static float ScaleMagnitude(int fontSize, float magnitude);
    static Vector2 ScaleMagnitude(int fontSize, const Vector2 &magnitude);
    static float GetScaleProportion(int fontSize);

    static void ClearTTFError();
    static bool CatchTTFError();

    void Free();
};

NAMESPACE_BANG_END

#endif // FONT_H
