#include "Bang/SystemClipboard.h"

#include <SDL_clipboard.h>

using namespace Bang;

void SystemClipboard::Set(const String &str)
{
    SDL_SetClipboardText(str.ToCString());
}

String SystemClipboard::Get()
{
    return String(SDL_GetClipboardText());
}
