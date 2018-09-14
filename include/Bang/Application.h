#ifndef APPLICATION_H
#define APPLICATION_H

#include <chrono>

#include "Bang/List.h"
#include "Bang/Path.h"
#include "Bang/SystemUtils.h"

NAMESPACE_BANG_BEGIN

FORWARD class Time;
FORWARD class Debug;
FORWARD class Scene;
FORWARD class Paths;
FORWARD class Window;
FORWARD class GEngine;
FORWARD class Physics;
FORWARD class Settings;
FORWARD class Resources;
FORWARD class AudioManager;
FORWARD class DialogWindow;
FORWARD class SceneManager;
FORWARD class WindowManager;
FORWARD class MetaFilesManager;

class Application
{
public:
    Application();
    virtual ~Application();

    virtual void Init(const Path &engineRootPath = Path::Empty);

    int MainLoop();
    bool MainLoopIteration();
    void BlockingWait(Window *win);

    Time             *GetTime() const;
    Paths            *GetPaths() const;
    Debug            *GetDebug() const;
    GEngine          *GetGEngine() const;
    Physics          *GetPhysics() const;
    Settings         *GetSettings() const;
    Resources        *GetResources() const;
    SystemUtils      *GetSystemUtils() const;
    AudioManager     *GetAudioManager() const;
    WindowManager    *GetWindowManager() const;
    MetaFilesManager *GetMetaFilesManager() const;

    static String GetMainThreadId();

    static Application *GetInstance();
    static void SetApplicationSingleton(Application *app);

    static void Exit(int returnCode, bool immediate = false);

protected:
    Time             *m_time               = nullptr;
    Debug            *m_debug              = nullptr;
    Paths            *m_paths              = nullptr;
    Physics          *m_physics            = nullptr;
    GEngine          *m_gEngine            = nullptr;
    Settings         *m_settings           = nullptr;
    Resources        *m_resources          = nullptr;
    SystemUtils      *m_systemUtils        = nullptr;
    AudioManager     *m_audioManager       = nullptr;
    WindowManager    *m_windowManager      = nullptr;
    MetaFilesManager *m_metaFilesManager   = nullptr;

    String m_mainThreadId = "";

private:
    static Application *s_appSingleton;

    int m_exitCode = 0;
    bool m_forcedExit = false;

    void InitBeforeLoop();
    virtual Debug* CreateDebug() const;
    virtual Paths* CreatePaths() const;
    virtual Settings* CreateSettings() const;
    virtual Resources *CreateResources() const;

    friend class Window;
};

NAMESPACE_BANG_END

#endif // APPLICATION_H
