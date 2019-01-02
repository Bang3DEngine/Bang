#include "Bang/Application.h"

#include <cstdlib>

#include "SDL_timer.h"

#include "Bang/Assets.h"
#include "Bang/AudioManager.h"
#include "Bang/ClassDB.h"
#include "Bang/Debug.h"
#include "Bang/GEngine.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/Paths.h"
#include "Bang/Physics.h"
#include "Bang/Settings.h"
#include "Bang/SystemUtils.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureFactory.h"
#include "Bang/Thread.h"
#include "Bang/Time.h"
#include "Bang/TimeSingleton.h"
#include "Bang/Window.h"
#include "Bang/WindowManager.h"

using namespace Bang;

Application *Application::s_appSingleton = nullptr;

Application::Application()
{
}

void Application::Init(const Path &engineRootPath)
{
    Init_(engineRootPath);
    InitAfterPathsInit_();
}

void Application::Init_(const Path &engineRootPath)
{
    srand(1234);

    Application::s_appSingleton = this;
    m_mainThreadId = Thread::GetCurrentThreadId();

    m_classDB = new ClassDB();
    GetClassDB()->RegisterClasses();

    m_time = new TimeSingleton();

    m_systemUtils = new SystemUtils();
    m_debug = CreateDebug();

    m_paths = CreatePaths();
    m_paths->InitPaths(engineRootPath);
}

void Application::InitAfterPathsInit_()
{
    m_settings = CreateSettings();
    m_settings->Init();

    m_paths->InitPathsAfterInitingSettings();

    m_projectManager = CreateProjectManager();

    m_physics = new Physics();
    m_physics->Init();

    m_audioManager = new AudioManager();
    m_audioManager->Init();

    m_windowManager = new WindowManager();
    GetWindowManager()->Init();

    m_time->SetInitTime(Time::GetNow() - Time::Millis(SDL_GetTicks()));

    m_metaFilesManager = new MetaFilesManager();
    MetaFilesManager::CreateMissingMetaFiles(Paths::GetEngineAssetsDir());
    MetaFilesManager::LoadMetaFilepathGUIDs(Paths::GetEngineAssetsDir());

    m_assets = CreateAssets();
    m_assets->Init();

    m_gEngine = new GEngine();
    m_gEngine->Init();

    m_assets->InitAfterGL();
}

Application::~Application()
{
    delete m_classDB;
    delete m_time;
    delete m_debug;
    delete m_paths;
    delete m_systemUtils;
    delete m_projectManager;

    delete m_physics;
    m_physics = nullptr;

    m_assets->Destroy();
    delete m_assets;
    m_assets = nullptr;

    delete m_settings;
    delete m_audioManager;
    delete m_windowManager;
    delete m_metaFilesManager;

    // delete m_gEngine; m_gEngine = nullptr;
}

#ifdef GPROF
#include <gperftools/profiler.h>
#endif
void Application::InitBeforeLoop()
{
    GetWindowManager()->GetTopWindow()->SetIcon(
        TextureFactory::GetBangB512Icon()->GetAssetFilepath());

#ifdef GPROF
    Path profileOutFile =
        Paths::GetExecutablePath().GetDirectory().Append("profiling_info.out");
    Debug_Log("Writing profiling information to: '" << profileOutFile << "'");
    ProfilerStart(profileOutFile.GetAbsolute().ToCString());
#endif
}

void FlushProfiling()
{
#ifdef GPROF
    ProfilerFlush();
#endif
}
void StopProfiling()
{
#ifdef GPROF
    ProfilerStop();
#endif
}

int Application::MainLoop()
{
    InitBeforeLoop();

    bool exit = false;
    while (!exit && !m_forcedExit)
    {
        exit = MainLoopIteration();
        FlushProfiling();
    }

    StopProfiling();
    return m_exitCode;
}

bool Application::MainLoopIteration()
{
    bool exit = GetWindowManager()->MainLoopIteration();
    return exit;
}

void Application::BlockingWait(Window *win)
{
    GetWindowManager()->OnBlockingWaitBegin(win);
    MainLoop();
    GetWindowManager()->OnBlockingWaitEnd();
}

TimeSingleton *Application::GetTime() const
{
    return m_time;
}

ClassDB *Application::GetClassDB() const
{
    return m_classDB;
}

Paths *Application::GetPaths() const
{
    return m_paths;
}

Debug *Application::GetDebug() const
{
    return m_debug;
}

GEngine *Application::GetGEngine() const
{
    return m_gEngine;
}

Physics *Application::GetPhysics() const
{
    return m_physics;
}

Settings *Application::GetSettings() const
{
    return m_settings;
}

Assets *Application::GetAssets() const
{
    return m_assets;
}

SystemUtils *Application::GetSystemUtils() const
{
    return m_systemUtils;
}

AudioManager *Application::GetAudioManager() const
{
    return m_audioManager;
}

WindowManager *Application::GetWindowManager() const
{
    return m_windowManager;
}

ProjectManager *Application::GetProjectManager() const
{
    return m_projectManager;
}

MetaFilesManager *Application::GetMetaFilesManager() const
{
    return m_metaFilesManager;
}

String Application::GetMainThreadId()
{
    Application *app = Application::GetInstance();
    return app ? app->m_mainThreadId : "";
}

Application *Application::GetInstance()
{
    return Application::s_appSingleton;
}

void Application::SetApplicationSingleton(Application *app)
{
    Application::s_appSingleton = app;
}

void Application::Exit(int returnCode, bool immediate)
{
    if (immediate)
    {
        std::exit(returnCode);
    }
    else
    {
        Application *app = Application::GetInstance();
        app->m_forcedExit = true;
        app->m_exitCode = returnCode;
    }
}

Debug *Application::CreateDebug() const
{
    return new Debug();
}

Paths *Application::CreatePaths() const
{
    return new Paths();
}

Settings *Application::CreateSettings() const
{
    return new Settings();
}

Assets *Application::CreateAssets() const
{
    return new Assets();
}

ProjectManager *Application::CreateProjectManager() const
{
    return new ProjectManager();
}
