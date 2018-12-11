#include "Bang/TextFormatter.h"

#include <memory>
#include <vector>

#include "Bang/AARect.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Font.h"
#include "Bang/Math.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"

using namespace Bang;

Array<TextFormatter::CharRect> TextFormatter::GetFormattedTextPositions(
    const String &content,
    const Font *font,
    int fontSize,
    const AARecti &limitsRect,
    const Vector2 &spacingMultiplier,
    HorizontalAlignment hAlignment,
    VerticalAlignment vAlignment,
    bool wrapping,
    uint *numberOfLines)
{
    if (content.IsEmpty())
    {
        return Array<CharRect>();
    }

    // First create a list with all the character rects in the origin
    Array<CharRect> charRects;
    for (uint i = 0; i < content.Size(); ++i)
    {
        const char c = content[i];
        Vector2 size = Vector2(font->GetAtlasCharRectSize(fontSize, c));
        if (c == ' ')
        {
            size = Vector2(font->GetAtlasCharRectSize(fontSize, 'a'));
        }
        AARectf charRect = AARect(Vector2(0, -size.y), Vector2(size.x, 0)) +
                           Vector2(0, font->GetFontAscent(fontSize));
        charRects.PushBack(CharRect(c, charRect));
    }

    Array<Array<CharRect>> linedCharRects =
        SplitCharRectsInLines(content,
                              font,
                              fontSize,
                              limitsRect,
                              spacingMultiplier,
                              charRects,
                              wrapping);
    *numberOfLines = linedCharRects.Size();

    if (limitsRect.IsValid())
    {
        TextFormatter::ApplyAlignment(&linedCharRects,
                                      limitsRect,
                                      font,
                                      fontSize,
                                      hAlignment,
                                      vAlignment);
    }

    Array<CharRect> finalCharRects;  // Flattened result
    for (const Array<CharRect> &line : linedCharRects)
    {
        finalCharRects.PushBack(line);
    }
    return finalCharRects;
}

Array<Array<TextFormatter::CharRect>> TextFormatter::SplitCharRectsInLines(
    const String &content,
    const Font *font,
    int fontSize,
    const AARecti &limitsRect,
    const Vector2 &spacingMult,
    const Array<CharRect> &charRects,
    bool wrapping)
{
    Array<Array<CharRect>> linedCharRects(1);  // Result

    Vector2 penPosition(limitsRect.GetMinXMaxY());  // penPosition.y is baseline
    const float lineSkip = font->GetLineSkip(fontSize);
    for (uint i = 0; i < content.Size(); ++i)
    {
        const float charAdvX = GetCharAdvanceX(content, font, fontSize, i);
        bool lineBreak = (content[i] == '\n');
        bool addCharacterToLines = (!lineBreak);
        if (wrapping && !lineBreak)
        {
            // Do we have to break line here because of wrapping?
            bool breakLineBecauseOfWrapping = false;

            // Split the input char positions into the needed lines.
            // Each line will contain as many words as possible (split by
            // spaces).
            if (content[i] != ' ')
            {
                breakLineBecauseOfWrapping =
                    (penPosition.x + charAdvX > limitsRect.GetMax().x);
            }
            else
            {
                // We have arrived to a space.
                // Does the following word (after this space) still fits in
                // the current line?
                float tmpAdvX = penPosition.x + charAdvX;
                for (uint j = i + 1; j < content.Size(); ++j)
                {
                    if (content[j] == ' ')
                    {
                        break;
                    }
                    const float jCharAdvX =
                        GetCharAdvanceX(content, font, fontSize, j);
                    if (tmpAdvX + jCharAdvX > limitsRect.GetMax().x)
                    {
                        breakLineBecauseOfWrapping = true;
                        break;
                    }
                    tmpAdvX += jCharAdvX * spacingMult.x;
                }
            }

            lineBreak = lineBreak || breakLineBecauseOfWrapping;
        }

        if (lineBreak)
        {
            // Advance to next line! Add the current line to the result.
            penPosition.x = SCAST<float>(limitsRect.GetMin().x);
            penPosition.y -= lineSkip * spacingMult.y;
            linedCharRects.PushBack(Array<CharRect>());

            // Skip all next ' '
            if (content[i] == ' ')
            {
                while (i < content.Size() && content[i] == ' ')
                {
                    ++i;
                }
                --i;
            }
        }

        if (addCharacterToLines)
        {
            CharRect cr(content[i], penPosition + charRects[i].rectPx);
            linedCharRects.Back().PushBack(cr);
            penPosition.x += charAdvX * spacingMult.x;
        }
    }
    return linedCharRects;
}

Vector2i TextFormatter::GetMinimumHeightTextSize(
    const String &content,
    const Font *font,
    int fontSize,
    const Vector2 &spacingMultiplier)
{
    // Get the text size with as less height as possible
    if (!font || content.IsEmpty() || fontSize <= 0)
    {
        return Vector2i::Zero();
    }

    Vector2 textSize = Vector2::Zero();
    float currentLineWidth = 0.0f;
    for (uint i = 0; i < content.Size(); ++i)
    {
        char c = content[i];
        if (c == '\n')
        {
            textSize.y += font->GetLineSkip(fontSize);
            currentLineWidth = 0.0f;
        }
        else
        {
            int charAdvX =
                SCAST<int>(GetCharAdvanceX(content, font, fontSize, i));
            currentLineWidth += charAdvX * spacingMultiplier.x;
            textSize.x = Math::Max(textSize.x, currentLineWidth);
        }
    }
    textSize.y += font->GetLineSkip(fontSize);  // Last/first line
    return Vector2i(Vector2::Round(textSize));
}

Vector2 FindMinCoord(const Array<TextFormatter::CharRect> &);
Vector2 FindMaxCoord(const Array<TextFormatter::CharRect> &);

void TextFormatter::ApplyAlignment(Array<Array<CharRect>> *linesCharRects,
                                   const AARecti &limitsRect,
                                   const Font *font,
                                   int fontSize,
                                   HorizontalAlignment hAlignment,
                                   VerticalAlignment vAlignment)
{
    ASSERT(limitsRect.IsValid());

    // For each line apply the HorizontalAlign
    for (Array<CharRect> &line : *linesCharRects)
    {
        if (line.IsEmpty())
        {
            continue;
        }
        Vector2 lineMinCoord = FindMinCoord(line);
        Vector2 lineMaxCoord = FindMaxCoord(line);

        float lineHorizontalOffset = 0;
        if (hAlignment == HorizontalAlignment::LEFT)
        {
            lineHorizontalOffset = limitsRect.GetMin().x - lineMinCoord.x;
        }
        else if (hAlignment == HorizontalAlignment::CENTER)
        {
            lineHorizontalOffset = limitsRect.GetCenter().x -
                                   (lineMaxCoord.x + lineMinCoord.x) / 2;
        }
        else if (hAlignment == HorizontalAlignment::RIGHT)
        {
            lineHorizontalOffset = limitsRect.GetMax().x - lineMaxCoord.x;
        }

        for (CharRect &cr : line)
        {
            cr.rectPx += Vector2(lineHorizontalOffset, 0);
        }
    }

    // Vertical align all the lines at once
    float textVerticalOffset = 0;
    const float textHeight =
        (linesCharRects->Size() * font->GetLineSkip(fontSize));
    if (vAlignment == VerticalAlignment::TOP)
    {
        textVerticalOffset = -font->GetFontAscent(fontSize);
    }
    else if (vAlignment == VerticalAlignment::CENTER)
    {
        textVerticalOffset = -font->GetFontDescent(fontSize) -
                             (limitsRect.GetHeight() / 2) - (textHeight / 2);
    }
    else if (vAlignment == VerticalAlignment::BOT)
    {
        textVerticalOffset =
            -font->GetFontDescent(fontSize) - limitsRect.GetHeight();
    }

    // Apply offsets
    for (Array<CharRect> &line : *linesCharRects)
    {
        for (CharRect &cr : line)
        {
            cr.rectPx += Vector2(0, textVerticalOffset);
        }
    }
}

AARectf TextFormatter::GetCharRect(char c, const Font *font, int fontSize)
{
    if (!font)
    {
        return AARectf::Zero();
    }

    Font::GlyphMetrics charMetrics = font->GetCharMetrics(fontSize, c);
    const Font::GlyphMetrics &cm = charMetrics;

    Vector2 charMin(cm.bearing.x, -(cm.size.y - cm.bearing.y));
    Vector2 charMax(cm.bearing.x + cm.size.x, cm.bearing.y);
    return AARectf(charMin, charMax);
}

float TextFormatter::GetCharAdvanceX(const String &content,
                                     const Font *font,
                                     int fontSize,
                                     int currentCharIndex)
{
    float advance = 0;
    if (currentCharIndex < SCAST<int>(content.Size()) - 1)
    {
        advance = font->GetKerning(
            fontSize, content[currentCharIndex], content[currentCharIndex + 1]);
    }

    if (advance <= 0)
    {
        const char c = content[currentCharIndex];
        Font::GlyphMetrics charMetrics = font->GetCharMetrics(fontSize, c);
        advance = charMetrics.advance;
    }

    return advance;
}

Vector2 FindMinCoord(const Array<TextFormatter::CharRect> &rects)
{
    Vector2 result;
    bool first = true;
    for (const TextFormatter::CharRect &cr : rects)
    {
        if (first)
        {
            first = false;
            result = cr.rectPx.GetMin();
        }
        else
        {
            result = Vector2::Min(result, cr.rectPx.GetMin());
        }
    }
    return result;
}

Vector2 FindMaxCoord(const Array<TextFormatter::CharRect> &rects)
{
    Vector2 result;
    bool first = true;
    for (const TextFormatter::CharRect &cr : rects)
    {
        if (first)
        {
            first = false;
            result = cr.rectPx.GetMax();
        }
        else
        {
            result = Vector2::Max(result, cr.rectPx.GetMax());
        }
    }
    return result;
}
