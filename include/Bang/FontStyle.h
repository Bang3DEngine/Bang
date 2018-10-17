#ifndef FONT_STYLE_H
#define FONT_STYLE_H

#include "Bang/Bang.h"
#include "Bang/Flags.h"
#include BANG_SDL2_TTF_INCLUDE(SDL_ttf.h)

namespace Bang
{
enum class FontStyle
{
    REGULAR,
    BOLD,
    ITALIC,
    UNDERLINED,
    STROKE
};

using FontStyleFlags = Flags<FontStyle>;
}

#endif  // FONT_H
