#ifndef CURSOR_H
#define CURSOR_H

#include "Bang/Bang.h"
#include BANG_SDL2_INCLUDE(SDL_mouse.h)

NAMESPACE_BANG_BEGIN

class Cursor
{
public:
    enum class Type
    {
        ARROW      = SDL_SYSTEM_CURSOR_ARROW,
        CROSSHAIR  = SDL_SYSTEM_CURSOR_CROSSHAIR,
        HAND       = SDL_SYSTEM_CURSOR_HAND,
        IBEAM      = SDL_SYSTEM_CURSOR_IBEAM,
        NO         = SDL_SYSTEM_CURSOR_NO,
        SIZE_NW_SE = SDL_SYSTEM_CURSOR_SIZENWSE,
        SIZE_NE_SW = SDL_SYSTEM_CURSOR_SIZENESW,
        SIZE_WE    = SDL_SYSTEM_CURSOR_SIZEWE,
        SIZE_NS    = SDL_SYSTEM_CURSOR_SIZENS,
        SIZE_ALL   = SDL_SYSTEM_CURSOR_SIZEALL,
        WAIT       = SDL_SYSTEM_CURSOR_WAIT,
        WAIT_ARROW = SDL_SYSTEM_CURSOR_WAITARROW
    };

    static void Set(Type cursorType);
    static Type Get();

    Cursor() = delete;
    virtual ~Cursor() = delete;

private:
    static Type s_type;

};

NAMESPACE_BANG_END

#endif // CURSOR_H

