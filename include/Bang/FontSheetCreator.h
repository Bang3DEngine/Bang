#ifndef FONTSHEETCREATOR_H
#define FONTSHEETCREATOR_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/Font.h"
#include "Bang/ImageIO.h"
#include "Bang/String.h"

namespace Bang
{
class Texture2D;

class FontSheetCreator
{
public:
    static bool LoadAtlasTexture(TTF_Font *fontFace,
                                 Texture2D *resultTexture,
                                 const String &charsToLoad,
                                 Array<AARecti> *imagesOutputRects = nullptr,
                                 int extraMargin = 0);

    static Imageb PackImages(const Array<Imageb> &images,
                             int margin,
                             Array<AARecti> *imagesOutputRects = nullptr,
                             const Color &bgColor = Color::Zero);

    FontSheetCreator() = delete;
};
}

#endif  // FONTSHEETCREATOR_H
