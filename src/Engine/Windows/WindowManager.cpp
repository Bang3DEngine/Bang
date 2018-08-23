#include "Bang/WindowManager.h"

#include BANG_SDL2_INCLUDE(SDL.h)
#include BANG_SDL2_TTF_INCLUDE(SDL_ttf.h)

#ifdef __linux__
#include <signal.h>
#include <unistd.h>
#endif

#include "Bang/Debug.h"
#include "Bang/Window.h"
#include "Bang/Application.h"
#include "Bang/DialogWindow.h"

USING_NAMESPACE_BANG

WindowManager::WindowManager()
{
}

WindowManager::~WindowManager()
{
    for (Window *w : GetCurrentWindows()) { delete w; }
    for (Window *w : p_windowsToBeDestroyed) { delete w; }

    TTF_Quit();
    SDL_Quit();
}

#ifdef __linux__
void SignalHandler(int signal)
{
    Debug_Log("Received SIGINT");
    Application::Exit(1, true);
}
#endif

void WindowManager::Init()
{
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0 )
    {
        Debug_Error("Failed to init SDL");
    }

    #ifdef __linux__
    signal(SIGINT,  SignalHandler);
    #endif

    if ( TTF_Init() )
    {
        Debug_Error("Could not init FreeType library: Error(" <<
                    TTF_GetError() <<  ")");
    }

    m_windowsStack.push( {} );
}

bool WindowManager::MainLoopIteration()
{
    bool exit = false;

    if (!HandleEvents())        { exit = true; }
    if (GetCurrentWindows().IsEmpty()) { exit = true; }
    DestroyQueuedWindows();

    for (Window *w : GetCurrentWindows())
    {
        Window::SetActive(w);
        w->MainLoopIteration();
        Window::SetActive(nullptr);
    }

    return exit;
}

void WindowManager::OnBlockingWaitBegin(Window *win)
{
    GetCurrentWindows().Remove(win);

    m_windowsStack.push( {win} );
    Window::SetActive(win);
}

void WindowManager::OnBlockingWaitEnd()
{
    m_windowsStack.pop();
    Window::SetActive( GetCurrentWindows().Front() );
}

void WindowManager::DestroyWindow(Window *window)
{
    p_windowsToBeDestroyed.PushBack(window);
}

DialogWindow *WindowManager::CreateDialogWindow(Window *parentWindow, bool resizable)
{
    DialogWindow *w = new DialogWindow(parentWindow, resizable);
    SetupWindow(w, 0);
    return w;
}

Window *WindowManager::GetTopWindow()
{
    WindowManager *wm = WindowManager::GetInstance();
    if (!wm) { return nullptr; }
    return !wm->GetCurrentWindows().IsEmpty() ? wm->GetCurrentWindows().Front() : nullptr;
}

void WindowManager::SetupWindow(Window *window, uint _flags)
{
    WindowManager::GetInstance()->GetCurrentWindows().PushBack(window);

    uint flags = (_flags > 0 ?_flags : (SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));
    window->Create(flags);
    window->OnResize(window->GetWidth(), window->GetHeight());
}

bool WindowManager::HandleEvents()
{
    SDL_Event sdlEvent;
    constexpr int AreThereMoreEvents = 1;
    while (SDL_PollEvent(&sdlEvent) == AreThereMoreEvents)
    {
        switch (sdlEvent.type)
        {
            default:
            {
                List<Window*> windowsToBeClosed;
                for (Window *w : GetCurrentWindows())
                {
                    Window::SetActive(w);
                    bool hasNotClosed = w->HandleEvent(sdlEvent);
                    if (!hasNotClosed) { windowsToBeClosed.PushBack(w); }
                    Window::SetActive(nullptr);
                }

                for (Window *w : windowsToBeClosed)
                {
                    Window::SetActive(w);
                    bool canCloseWindow = w->OnClosed();
                    if (canCloseWindow)
                    {
                        GetCurrentWindows().Remove(w);
                        delete w;
                        Window::SetActive(nullptr);
                    }
                }
            }
        }
    }

    for (Window *w : GetCurrentWindows())
    {
        Window::SetActive(w);
        w->OnHandleEventsFinished();
        Window::SetActive(nullptr);
    }
    return true;
}

void WindowManager::DestroyQueuedWindows()
{
    Window *latestWindow = Window::GetActive();
    for (Window *w : p_windowsToBeDestroyed)
    {
        Window::SetActive(w);
        GetCurrentWindows().Remove(w);
        delete w;
    }
    p_windowsToBeDestroyed.Clear();
    Window::SetActive(latestWindow);
}

List<Window*> &WindowManager::GetCurrentWindows()
{
    return m_windowsStack.top();
}
const List<Window*> &WindowManager::GetCurrentWindows() const
{
    return m_windowsStack.top();
}

WindowManager *WindowManager::GetInstance()
{
    return Application::GetInstance()->GetWindowManager();
}


