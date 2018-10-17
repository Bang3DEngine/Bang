#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <ostream>

#include "Bang/AARect.h"
#include "Bang/Alignment.h"
#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"

namespace Bang
{
class Font;

class TextFormatter
{
public:
    struct CharRect
    {
        AARectf rectPx;
        char character;
        CharRect(char _c, const AARectf &_rect) : rectPx(_rect), character(_c)
        {
        }
        friend std::ostream &operator<<(std::ostream &os,
                                        const TextFormatter::CharRect &cr);
    };

    static Array<CharRect> GetFormattedTextPositions(
        const String &content,
        const Font *font,
        int fontSize,
        const AARecti &limitsRect,
        const Vector2 &spacingMultiplier,
        HorizontalAlignment hAlignment,
        VerticalAlignment vAlignment,
        bool wrapping,
        uint *numberOfLines);

    static Vector2i GetMinimumHeightTextSize(const String &content,
                                             const Font *font,
                                             int fontSize,
                                             const Vector2 &spacingMultiplier);

    TextFormatter() = delete;

private:
    static Array<Array<CharRect>> SplitCharRectsInLines(
        const String &content,
        const Font *font,
        int fontSize,
        const AARecti &limitsRect,
        const Vector2 &spacingMultiplier,
        const Array<CharRect> &charRects,
        bool wrapping);

    static void ApplyAlignment(Array<Array<CharRect>> *linedCharRects,
                               const AARecti &limitsRect,
                               const Font *font,
                               int fontSize,
                               HorizontalAlignment hAlignment,
                               VerticalAlignment vAlignment);

    static AARectf GetCharRect(char c, const Font *font, int fontSize);
    static float GetCharAdvanceX(const String &content,
                                 const Font *font,
                                 int fontSize,
                                 int currentCharIndex);
};

inline std::ostream &operator<<(std::ostream &os,
                                const TextFormatter::CharRect &cr)
{
    os << "(" << cr.character << ", " << cr.rectPx << ")";
    return os;
}
}

#endif  // TEXTFORMATTER_H
