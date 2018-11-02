#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <stack>

#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/Window.h"

namespace Bang
{
class DialogWindow;

class WindowManager
{
public:
    WindowManager();
    virtual ~WindowManager();

    void Init();
    int MainLoop();
    bool MainLoopIteration();
    void OnBlockingWaitBegin(Window *win);
    void OnBlockingWaitEnd();
    void DestroyWindow(Window *window);

    template <class WinT = Window>
    static WinT *CreateWindow(uint flags = 0);
    static DialogWindow *CreateDialogWindow(Window *parentWindow,
                                            bool resizable);
    static Window *GetTopWindow();

    static WindowManager *GetInstance();

private:
    Array<Array<Window *>> m_windowsStack;
    Array<Window *> p_windowsToBeDestroyed;

    static void SetupWindow(Window *window, uint flags);

    bool HandleEvents();
    void DestroyQueuedWindows();

    const Array<Window *> GetCurrentWindows() const;
};

}

#include "Bang/WindowManager.tcc"

#endif  // WINDOWMANAGER_H
