#ifndef WINDOW_H
#define WINDOW_H

#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/String.h"
#include "Bang/Time.h"
#include "Bang/Vector2.h"

struct SDL_Window;
union SDL_Event;

using SDL_GLContext = void *;

namespace Bang
{
class Input;
class Path;
class SceneManager;
class Window;

class IEventsWindow
{
    IEVENTS(IEventsWindow);

public:
    virtual void OnFocusGained(Window *w)
    {
        BANG_UNUSED(w);
    }

    virtual void OnFocusLost(Window *w)
    {
        BANG_UNUSED(w);
    }
};

class Window : public EventEmitter<IEventsWindow>
{
public:
    Window();
    virtual ~Window() override;

    virtual void Create(uint flags);
    void SwapBuffers() const;
    bool MainLoopIteration();
    void MakeCurrent();

    virtual void Update();
    virtual void Render();
    virtual void OnResize(int newWidth, int newHeight);

    virtual bool HandleEvent(const SDL_Event &sdlEvent);
    void OnHandleEventsFinished();
    virtual bool OnClosed();

    void Maximize();
    void Restore();
    void Minimize();
    void MoveToFront();
    void SetIcon(const Path &iconPath);
    void SetBordered(bool bordered);
    void SetMinSize(int minSizeX, int minSizeY);
    void SetMaxSize(int maxSizeX, int maxSizeY);
    void SetResizable(bool resizable);
    void SetPosition(int newPosX, int newPosY);
    void SetTitle(const String &title);
    void SetSize(int newWidth, int newHeight);
    void SetSleepTimeOnBackground(Time sleepTimeOnBackground);
    static void SetActive(Window *window);

    int GetWidth() const;
    int GetHeight() const;
    bool HasFocus() const;
    bool HasFocusRecursive() const;
    bool IsBordered() const;
    String GetTitle() const;
    bool IsMouseOver() const;
    Vector2i GetSize() const;
    Vector2i GetMinSize() const;
    Vector2i GetMaxSize() const;
    bool IsResizable() const;
    Vector2i GetPosition() const;
    Vector2i GetInsidePosition() const;
    bool IsBlockedByChildren() const;
    bool HasFlags(uint flags) const;
    int GetGLMajorVersion() const;
    int GetGLMinorVersion() const;
    Time GetSleepTimeOnBackground() const;

    static float GetAspectRatioS();
    static int GetHeightS();
    static int GetWidthS();

    SDL_GLContext GetGLContext() const;
    Input *GetInput() const;
    SceneManager *GetSceneManager() const;

    SDL_Window *GetSDLWindow() const;
    uint GetSDLWindowID() const;
    Window *GetParentWindow() const;
    const Array<Window *> &GetChildren() const;

    static Window *GetActive();

protected:
    void SetParent(Window *parentWindow);

private:
    SDL_Window *m_sdlWindow = nullptr;

    Array<Window *> p_children;
    Window *p_parent = nullptr;

    SDL_GLContext m_sdlGLContext = nullptr;
    Input *m_input = nullptr;
    SceneManager *m_sceneManager = nullptr;

    Time m_sleepTimeInBackground;
    Vector2i m_minSize = Vector2i::Zero();
    Vector2i m_maxSize = Vector2i(4096);
    Vector2i m_prevSize = Vector2i::Zero();
    Vector2i m_newSize = Vector2i::Zero();
    bool m_isResizable = true, m_resizableChanged = false;

    static Window *s_activeWindow;

    void RetrieveTitleBarHeight();

    void _SetMinSize(int minSizeX, int minSizeY);
    void _SetMaxSize(int maxSizeX, int maxSizeY);
    Vector2i _GetMinSize() const;
    Vector2i _GetMaxSize() const;
    bool IsParentWindow(int sdlWindowId) const;

    virtual SceneManager *CreateSceneManager() const;

    friend class Application;
    friend int EventFilter(void *userData, SDL_Event *event);
};
}

#endif  // IWINDOW_H
