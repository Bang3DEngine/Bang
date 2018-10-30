#include "Bang/WindowManager.h"

#include <SDL_ttf.h>

#ifdef __linux__
#include <signal.h>
#include <unistd.h>
#endif

#include "Bang/Application.h"
#include "Bang/Debug.h"
#include "Bang/DialogWindow.h"
#include "Bang/Window.h"

using namespace Bang;

WindowManager::WindowManager()
{
}

WindowManager::~WindowManager()
{
    for (Window *w : GetCurrentWindows())
    {
        delete w;
    }
    for (Window *w : p_windowsToBeDestroyed)
    {
        delete w;
    }

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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
    {
        Debug_Error("Failed to init SDL");
    }

#ifdef __linux__
    signal(SIGINT, SignalHandler);
#endif

    if (TTF_Init())
    {
        Debug_Error("Could not init FreeType library: Error(" << TTF_GetError()
                                                              << ")");
    }

    m_windowsStack.PushBack({});
}

bool WindowManager::MainLoopIteration()
{
    bool exit = false;
    exit |= (!HandleEvents());
    exit |= (GetCurrentWindows().IsEmpty());
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
    m_windowsStack.Back().Remove(win);

    m_windowsStack.PushBack({win});
    Window::SetActive(win);
}

void WindowManager::OnBlockingWaitEnd()
{
    m_windowsStack.PopBack();
    Window::SetActive(GetCurrentWindows().Front());
}

void WindowManager::DestroyWindow(Window *window)
{
    p_windowsToBeDestroyed.PushBack(window);
}

DialogWindow *WindowManager::CreateDialogWindow(Window *parentWindow,
                                                bool resizable)
{
    DialogWindow *w = new DialogWindow(parentWindow, resizable);
    SetupWindow(w, 0);
    return w;
}

Window *WindowManager::GetTopWindow()
{
    WindowManager *wm = WindowManager::GetInstance();
    if (!wm)
    {
        return nullptr;
    }
    return !wm->GetCurrentWindows().IsEmpty() ? wm->GetCurrentWindows().Front()
                                              : nullptr;
}

void WindowManager::SetupWindow(Window *window, uint _flags)
{
    WindowManager::GetInstance()->m_windowsStack.Back().PushBack(window);

    uint flags =
        (_flags > 0 ? _flags : (SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));
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
                List<Window *> windowsToBeClosed;
                for (Window *w : GetCurrentWindows())
                {
                    Window::SetActive(w);
                    bool hasNotClosed = w->HandleEvent(sdlEvent);
                    if (!hasNotClosed)
                    {
                        windowsToBeClosed.PushBack(w);
                    }
                    Window::SetActive(nullptr);
                }

                for (Window *w : windowsToBeClosed)
                {
                    Window::SetActive(w);
                    bool canCloseWindow = w->OnClosed();
                    if (canCloseWindow)
                    {
                        m_windowsStack.Back().Remove(w);
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
    while (!p_windowsToBeDestroyed.IsEmpty())
    {
        Window *w = p_windowsToBeDestroyed.Back();
        p_windowsToBeDestroyed.PopBack();

        for (Array<Window *> &wArr : m_windowsStack)
        {
            wArr.Remove(w);
        }

        if (w == Window::GetActive())
        {
            Window::SetActive(nullptr);
        }

        delete w;
    }
}

const Array<Window *> WindowManager::GetCurrentWindows() const
{
    if (m_windowsStack.Size() >= 1)
    {
        return m_windowsStack.Back();
    }
    return {};
}

WindowManager *WindowManager::GetInstance()
{
    return Application::GetInstance()->GetWindowManager();
}
