#include "Bang/Cursor.h"

#include "Bang/UMap.h"
#include "Bang/UMap.tcc"

using namespace Bang;

Cursor::Type Cursor::s_type = Cursor::Type::ARROW;

void Cursor::Set(Cursor::Type cursorType)
{
    if (cursorType != Cursor::s_type)
    {
        Cursor::s_type = cursorType;

        static UMap<Cursor::Type, SDL_Cursor *, EnumClassHash> createdCursors;
        if (!createdCursors.ContainsKey(Cursor::Get()))
        {
            SDL_Cursor *cursor =
                SDL_CreateSystemCursor(SCAST<SDL_SystemCursor>(Cursor::Get()));
            createdCursors.Add(Cursor::Get(), cursor);
        }

        SDL_SetCursor(createdCursors.Get(Cursor::Get()));
    }
}

Cursor::Type Cursor::Get()
{
    return Cursor::s_type;
}
