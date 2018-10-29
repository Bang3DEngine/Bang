#ifndef MOUSEBUTTON_H
#define MOUSEBUTTON_H

#include "Bang/Bang.h"

#include <SDL_mouse.h>

namespace Bang
{
enum class MouseButton
{
    NONE = 0,
    LEFT = SDL_BUTTON_LEFT,
    RIGHT = SDL_BUTTON_RIGHT,
    MIDDLE = SDL_BUTTON_MIDDLE,
    XBUTTON1 = SDL_BUTTON_X1,
    XBUTTON2 = SDL_BUTTON_X2
};
}

#endif  // MOUSEBUTTON_H
