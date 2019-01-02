#ifndef APPLICATION_H
#define APPLICATION_H

#include "Bang/BangDefines.h"
#include "Bang/Path.h"
#include "Bang/ProjectManager.h"
#include "Bang/String.h"

namespace Bang
{
class AudioManager;
class ClassDB;
class Debug;
class GEngine;
class MetaFilesManager;
class Paths;
class Physics;
class Assets;
class Settings;
class SystemUtils;
class TimeSingleton;
class Window;
class WindowManager;

class Application
{
public:
    Application();
    virtual ~Application();

    virtual void Init(const Path &engineRootPath = Path::Empty());

    int MainLoop();
    bool MainLoopIteration();
    void BlockingWait(Window *win);

    TimeSingleton *GetTime() const;
    ClassDB *GetClassDB() const;
    Paths *GetPaths() const;
    Debug *GetDebug() const;
    GEngine *GetGEngine() const;
    Physics *GetPhysics() const;
    Settings *GetSettings() const;
    Assets *GetAssets() const;
    SystemUtils *GetSystemUtils() const;
    AudioManager *GetAudioManager() const;
    WindowManager *GetWindowManager() const;
    ProjectManager *GetProjectManager() const;
    MetaFilesManager *GetMetaFilesManager() const;

    static String GetMainThreadId();

    static Application *GetInstance();
    static void SetApplicationSingleton(Application *app);

    static void Exit(int returnCode, bool immediate = false);

protected:
    TimeSingleton *m_time = nullptr;
    ClassDB *m_classDB = nullptr;
    Debug *m_debug = nullptr;
    Paths *m_paths = nullptr;
    Physics *m_physics = nullptr;
    GEngine *m_gEngine = nullptr;
    Settings *m_settings = nullptr;
    Assets *m_assets = nullptr;
    SystemUtils *m_systemUtils = nullptr;
    AudioManager *m_audioManager = nullptr;
    WindowManager *m_windowManager = nullptr;
    ProjectManager *m_projectManager = nullptr;
    MetaFilesManager *m_metaFilesManager = nullptr;

    String m_mainThreadId = "";

    virtual void Init_(const Path &engineRootPath = Path::Empty());
    virtual void InitAfterPathsInit_();

private:
    static Application *s_appSingleton;

    int m_exitCode = 0;
    bool m_forcedExit = false;

    void InitBeforeLoop();
    virtual Debug *CreateDebug() const;
    virtual Paths *CreatePaths() const;
    virtual Settings *CreateSettings() const;
    virtual Assets *CreateAssets() const;
    virtual ProjectManager *CreateProjectManager() const;

    friend class Window;
};
}  // namespace Bang

#endif  // APPLICATION_H
