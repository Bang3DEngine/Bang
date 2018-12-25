#include "Bang/Window.h"

#include <SDL.h>

#include "Bang/Application.h"
#include "Bang/Assets.h"
#include "Bang/Debug.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/Input.h"
#include "Bang/Scene.h"
#include "Bang/SceneManager.h"
#include "Bang/Texture2D.h"
#include "Bang/Thread.h"
#include "Bang/TimeSingleton.h"
#include "Bang/UILayoutManager.h"

using namespace Bang;

Window *Window::s_activeWindow = nullptr;

Window::Window()
{
    m_sdlGLContext = GL::GetSharedGLContext();
}

Window::~Window()
{
    SetParent(nullptr);

    delete m_sceneManager;
    delete m_input;

    SDL_DestroyWindow(m_sdlWindow);
}

SDL_Window *CreateSDLWindow(uint flags, const Vector2i &winSize)
{
    return SDL_CreateWindow("Bang",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            winSize.x,
                            winSize.y,
                            flags);
}

void Window::Create(uint flags)
{
    Vector2i winSize(512);

    // Share context
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    m_sdlWindow = CreateSDLWindow(flags, winSize);

    Window::SetActive(this);

    SetMinSize(1, 1);
    SetMaxSize(99999, 99999);

    m_input = new Input();
    m_sceneManager = CreateSceneManager();

    m_sceneManager->Init();

    SetSize(winSize.x, winSize.y);
}

void Window::SwapBuffers() const
{
    SDL_GL_SwapWindow(GetSDLWindow());
}

void Window::MakeCurrent()
{
    SDL_GL_MakeCurrent(GetSDLWindow(), GetGLContext());
    GL::SetViewport(0, 0, GetWidth(), GetHeight());
}

bool Window::MainLoopIteration()
{
    Window::SetActive(this);
    GetInput()->ProcessEnqueuedEvents();

    Update();
    Render();

    GetInput()->OnFrameFinished();
    if (!HasFocusRecursive())
    {
        if (GetSleepTimeOnBackground().GetNanos() > 0)
        {
            Thread::SleepCurrentThread(GetSleepTimeOnBackground().GetSeconds());
        }
    }
    SwapBuffers();

    return true;
}

void Window::Update()
{
    GetSceneManager()->Update();
}

void Window::Render()
{
    GetSceneManager()->Render();
}

bool Window::HandleEvent(const SDL_Event &sdlEvent)
{
    if (!IsBlockedByChildren())
    {
        GetInput()->EnqueueEvent(sdlEvent, this);
    }

    switch (sdlEvent.type)
    {
        case SDL_WINDOWEVENT:
            if (sdlEvent.window.windowID == GetSDLWindowID())
            {
                if (!IsBlockedByChildren())
                {
                    switch (sdlEvent.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE: return false;
                    }
                }

                switch (sdlEvent.window.event)
                {
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        EventEmitter<IEventsWindow>::PropagateToListeners(
                            &IEventsWindow::OnFocusGained, this);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        EventEmitter<IEventsWindow>::PropagateToListeners(
                            &IEventsWindow::OnFocusLost, this);
                        break;

                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    case SDL_WINDOWEVENT_RESIZED:
                        m_newSize = Vector2i(sdlEvent.window.data1,
                                             sdlEvent.window.data2);
                        break;
                }
            }
            else
            {
                // Other window (not this) events
                switch (sdlEvent.window.event)
                {
                    case SDL_WINDOWEVENT_EXPOSED:
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        if (IsParentWindow(sdlEvent.window.windowID))
                        {
                            MoveToFront();
                        }
                        break;
                }
            }
    }

    return true;
}

void Window::OnHandleEventsFinished()
{
    OnResize(m_newSize.x, m_newSize.y);
}

bool Window::OnClosed()
{
    return true;
}

void Window::Maximize()
{
    SDL_MaximizeWindow(GetSDLWindow());
}

void Window::Restore()
{
    SDL_RestoreWindow(GetSDLWindow());
}

void Window::Minimize()
{
    SDL_MinimizeWindow(GetSDLWindow());
}

void Window::MoveToFront()
{
    SDL_RaiseWindow(GetSDLWindow());
    for (Window *childWindow : p_children)
    {
        childWindow->MoveToFront();
    }
}

void SDL_PutPixel32(SDL_Surface *surface, int x, int y, Uint32 color)
{
    if (SDL_MUSTLOCK(surface))
    {
        SDL_LockSurface(surface);
    }

    Uint8 *pixel = (Uint8 *)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32 *)pixel) = color;

    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }
}

void Window::SetIcon(const Path &iconPath)
{
    constexpr Uint32 RM = 0xff, GM = 0xff00, BM = 0xff0000, AM = 0xff000000;
    AH<Texture2D> tex = Assets::Load<Texture2D>(iconPath);
    Image img = tex.Get()->ToImage();
    SDL_Surface *icon = SDL_CreateRGBSurface(
        SDL_SWSURFACE, img.GetWidth(), img.GetHeight(), 32, RM, GM, BM, AM);
    for (int y = 0; y < img.GetHeight(); ++y)
    {
        for (int x = 0; x < img.GetWidth(); ++x)
        {
            const Color color = img.GetPixel(x, y);
            const Vector4i ci(Vector4::Round(color.ToVector4() * 255.0f));
            SDL_PutPixel32(
                icon,
                x,
                y,
                SDL_MapRGBA(icon->format, ci[0], ci[1], ci[2], ci[3]));
        }
    }
    SDL_SetWindowIcon(GetSDLWindow(), icon);
    SDL_FreeSurface(icon);
}

void Window::SetBordered(bool bordered)
{
    SDL_SetWindowBordered(GetSDLWindow(), SCAST<SDL_bool>(bordered));
}

void Window::SetMinSize(int minSizeX, int minSizeY)
{
    m_minSize = Vector2i(minSizeX, minSizeY);
    _SetMinSize(minSizeX, minSizeY);
}

void Window::SetMaxSize(int maxSizeX, int maxSizeY)
{
    m_maxSize = Vector2i(maxSizeX, maxSizeY);
    _SetMaxSize(maxSizeX, maxSizeY);
}

void Window::SetResizable(bool resizable)
{
    if (m_isResizable != resizable)
    {
        m_isResizable = resizable;

        if (IsResizable())
        {
            _SetMaxSize(GetMaxSize().x, GetMaxSize().y);
            _SetMinSize(GetMinSize().x, GetMinSize().y);
            SetSize(GetSize().x, GetSize().y);
        }
        else
        {
            Vector2i size = GetSize();
            _SetMinSize(size.x, size.y);
            _SetMaxSize(size.x + 1, size.y + 1);
        }
    }
}

void Window::SetPosition(int newPosX, int newPosY)
{
    SDL_SetWindowPosition(GetSDLWindow(), newPosX, newPosY);
}

void Window::SetTitle(const String &title)
{
    SDL_SetWindowTitle(GetSDLWindow(), title.ToCString());
}

void Window::SetSize(int w, int h)
{
    SDL_SetWindowSize(m_sdlWindow, w, h);
    m_newSize = Vector2i(w, h);
    OnResize(w, h);
}

void Window::SetSleepTimeOnBackground(Time sleepTimeOnBackground)
{
    m_sleepTimeInBackground = sleepTimeOnBackground;
}

void Window::OnResize(int newWidth, int newHeight)
{
    if (m_newSize != m_prevSize)
    {
        m_prevSize = m_newSize;
        GL::SetViewport(0, 0, GetWidth(), GetHeight());
        GetSceneManager()->OnResize(newWidth, newHeight);
    }
}

int Window::GetWidth() const
{
    return GetSize().x;
}
int Window::GetHeight() const
{
    return GetSize().y;
}

bool Window::HasFocus() const
{
    return HasFlags(SDL_WINDOW_INPUT_FOCUS);
}

bool Window::HasFocusRecursive() const
{
    if (HasFocus())
    {
        return true;
    }

    for (Window *childWindow : GetChildren())
    {
        if (childWindow->HasFocusRecursive())
        {
            return true;
        }
    }

    return false;
}

bool Window::IsBordered() const
{
    return !HasFlags(SDL_WINDOW_BORDERLESS);
}

String Window::GetTitle() const
{
    return String(SDL_GetWindowTitle(GetSDLWindow()));
}

bool Window::IsMouseOver() const
{
    return HasFlags(SDL_WINDOW_MOUSE_FOCUS);
}

Vector2i Window::GetSize() const
{
    Vector2i size;
    SDL_GetWindowSize(m_sdlWindow, &size.x, &size.y);
    return size;
}

Vector2i Window::GetMinSize() const
{
    return m_minSize;
}

Vector2i Window::GetMaxSize() const
{
    return m_maxSize;
}

bool Window::IsResizable() const
{
    return m_isResizable;
}

Vector2i Window::GetPosition() const
{
    Vector2i wpos;
    SDL_GetWindowPosition(GetSDLWindow(), &wpos.x, &wpos.y);
    return wpos;
}

Vector2i Window::GetInsidePosition() const
{
    return GetPosition();
}

bool Window::IsBlockedByChildren() const
{
    return !p_children.IsEmpty();
}

bool Window::HasFlags(uint flags) const
{
    return (SDL_GetWindowFlags(GetSDLWindow()) & flags) > 0;
}

int Window::GetGLMajorVersion() const
{
    int v;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &v);
    return v;
}

int Window::GetGLMinorVersion() const
{
    int v;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &v);
    return v;
}

Time Window::GetSleepTimeOnBackground() const
{
    return m_sleepTimeInBackground;
}

int Window::GetHeightS()
{
    return Window::GetActive()->GetHeight();
}

int Window::GetWidthS()
{
    return Window::GetActive()->GetWidth();
}

SDL_GLContext Window::GetGLContext() const
{
    return m_sdlGLContext;
}

Input *Window::GetInput() const
{
    return m_input;
}

SceneManager *Window::GetSceneManager() const
{
    return m_sceneManager;
}

SDL_Window *Window::GetSDLWindow() const
{
    return m_sdlWindow;
}

uint Window::GetSDLWindowID() const
{
    return SDL_GetWindowID(m_sdlWindow);
}

Window *Window::GetParentWindow() const
{
    return p_parent;
}

const Array<Window *> &Window::GetChildren() const
{
    return p_children;
}

SceneManager *Window::CreateSceneManager() const
{
    return new SceneManager();
}

Window *Window::GetActive()
{
    return Window::s_activeWindow;
}

void Window::SetParent(Window *parentWindow)
{
    ASSERT(parentWindow != this);
    ASSERT(!p_children.Contains(parentWindow));

    if (p_parent != parentWindow)
    {
        if (p_parent)
        {
            p_parent->p_children.Remove(this);
            p_parent->SetResizable(true);
        }

        p_parent = parentWindow;

        if (p_parent)
        {
            p_parent->SetResizable(false);
            p_parent->p_children.PushBack(this);
        }
    }
}

void Window::_SetMinSize(int minSizeX, int minSizeY)
{
    SDL_SetWindowMinimumSize(GetSDLWindow(), minSizeX, minSizeY);
}

void Window::_SetMaxSize(int maxSizeX, int maxSizeY)
{
    SDL_SetWindowMaximumSize(GetSDLWindow(), maxSizeX, maxSizeY);
}

Vector2i Window::_GetMinSize() const
{
    Vector2i minSize;
    SDL_GetWindowMinimumSize(GetSDLWindow(), &minSize.x, &minSize.y);
    return minSize;
}

Vector2i Window::_GetMaxSize() const
{
    Vector2i maxSize;
    SDL_GetWindowMaximumSize(GetSDLWindow(), &maxSize.x, &maxSize.y);
    return maxSize;
}

bool Window::IsParentWindow(int sdlWindowId) const
{
    return p_parent ? (p_parent->GetSDLWindowID() == sdlWindowId ||
                       p_parent->IsParentWindow(sdlWindowId))
                    : false;
}

void Window::SetActive(Window *window)
{
    Window::s_activeWindow = window;
    if (window)
    {
        window->MakeCurrent();
    }
}
