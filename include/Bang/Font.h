#ifndef FONT_H
#define FONT_H

#include <SDL_ttf.h>

#include "Bang/AARect.h"
#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/MetaNode.h"
#include "Bang/Path.h"
#include "Bang/String.h"
#include "Bang/UMap.h"
#include "Bang/Vector2.h"

using TTF_Font = _TTF_Font;

namespace Bang
{
template <class>
class AssetHandle;
class Texture2D;

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
        Vector2f size = Vector2f::Zero();
        Vector2f bearing = Vector2f::Zero();
        float advance = 0;
    };

    Texture2D *GetFontAtlas(int size) const;

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

    // Asset
    void Import(const Path &ttfFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    Path m_ttfFilepath = Path::Empty();
    TTF_Font *m_referenceFont = nullptr;

    struct FontDataCache
    {
        float height, ascent, descent, lineSkip;
        UMap<char, GlyphMetrics> charMetrics;
    };

    // For each font style
    FontDataCache m_referenceFontDataCache;
    mutable UMap<int, TTF_Font *> m_openFonts;
    mutable UMap<int, AH<Texture2D>> m_cachedAtlas;
    mutable UMap<int, UMap<char, AARecti>> m_cachedAtlasCharRects;
    mutable UMap<int, String> m_cachedAtlasChars;

    Font();
    virtual ~Font() override;

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
}  // namespace Bang

#endif  // FONT_H
