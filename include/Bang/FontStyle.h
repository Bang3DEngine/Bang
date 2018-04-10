#ifndef FONT_STYLE_H
#define FONT_STYLE_H

#include "Bang/Bang.h"
#include "Bang/Flags.h"
#include BANG_SDL2_TTF_INCLUDE(SDL_ttf.h)

NAMESPACE_BANG_BEGIN

enum class FontStyle
{
    REGULAR,
    BOLD,
    ITALIC,
    UNDERLINED,
    STROKE
};

using FontStyleFlags = Flags<FontStyle>;

NAMESPACE_BANG_END

#endif // FONT_H
