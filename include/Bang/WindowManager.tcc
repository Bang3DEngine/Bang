#pragma once

#include "Bang.h"
#include "Bang/Window.h"
#include "Bang/WindowManager.h"

namespace Bang
{
template <class WinT>
WinT *WindowManager::CreateWindow(uint flags)
{
    WinT *win = new WinT();
    SetupWindow(win, flags);
    return win;
}
}
