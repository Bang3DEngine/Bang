#include "Bang/DialogWindow.h"

#include <SDL.h>
#include "Bang/Input.h"

using namespace Bang;

DialogWindow::DialogWindow(Window *parentWindow, bool resizable)
{
    m_resizable = resizable;
    SetParent(parentWindow);
}

DialogWindow::~DialogWindow()
{
}

void DialogWindow::Create(uint flags)
{
    uint newFlags = flags;
    if (!m_resizable)
    {
        newFlags &= ~SDL_WINDOW_RESIZABLE;
    }
    Window::Create(newFlags);
}

void DialogWindow::CenterInParent()
{
    if (GetParentWindow())
    {
        Vector2i centeredPos = GetParentWindow()->GetPosition() +
                               GetParentWindow()->GetSize() / 2 - GetSize() / 2;
        SetPosition(centeredPos.x, centeredPos.y);
    }
}
