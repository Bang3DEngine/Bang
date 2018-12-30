#include "Bang/SceneManager.h"

#include <ostream>

#include "Bang/Assert.h"
#include "Bang/AudioManager.h"
#include "Bang/BehaviourManager.h"
#include "Bang/Camera.h"
#include "Bang/Color.h"
#include "Bang/Debug.h"
#include "Bang/Extensions.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsSceneManager.h"
#include "Bang/List.tcc"
#include "Bang/Paths.h"
#include "Bang/Physics.h"
#include "Bang/Scene.h"
#include "Bang/StreamOperators.h"
#include "Bang/Window.h"

using namespace Bang;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
    if (GetLoadedScene())
    {
        GameObject::DestroyImmediate(GetLoadedScene());
    }
    delete m_behaviourManager;
}

void SceneManager::Init()
{
    m_behaviourManager = CreateBehaviourManager();
}

BehaviourManager *SceneManager::CreateBehaviourManager() const
{
    return new BehaviourManager();
}

SceneManager *SceneManager::GetActive()
{
    Window *win = Window::GetActive();
    return win ? win->GetSceneManager() : nullptr;
}

Scene *SceneManager::GetObjectPtrLookupScene()
{
    SceneManager *sm = SceneManager::GetActive();
    return sm ? sm->GetObjectPtrLookupScene_() : nullptr;
}

void SceneManager::OnNewFrame(Scene *scene)
{
    if (scene)
    {
        scene->PreStart();
        scene->Start();

        scene->Update();
        Physics::GetInstance()->UpdatePxSceneFromTransforms(scene);
        Physics::GetInstance()->StepIfNeeded(scene);
        scene->PostUpdate();

        scene->DestroyDelayedGameObjects();
        scene->DestroyDelayedComponents();
    }
}

void SceneManager::Update()
{
    GetBehaviourManager()->Update();
    if (GetNextLoadNeeded())
    {
        LoadSceneInstantly_();
    }
    SceneManager::OnNewFrame(GetActiveScene_());
}

void SceneManager::Render()
{
    Scene *activeScene = GetActiveScene_();
    if (activeScene)
    {
        Camera *camera = activeScene->GetCamera();
        GEngine *ge = GEngine::GetInstance();
        if (camera && ge)
        {
            camera->SetRenderSize(Window::GetActive()->GetSize());
            ge->Render(activeScene, camera);
            ge->RenderTexture(camera->GetGBuffer()->GetDrawColorTexture());
        }
        else
        {
            GL::ClearColorBuffer(Color::Black());
        }
    }
}

void SceneManager::OnResize(int width, int height)
{
    Scene *scene = GetActiveScene_();
    if (scene)
    {
        scene->OnResize(width, height);
    }
}

Scene *SceneManager::GetActiveScene()
{
    SceneManager *sm = SceneManager::GetActive();
    return sm ? sm->GetActiveScene_() : nullptr;
}
Scene *SceneManager::GetActiveScene_() const
{
    return p_activeScene;
}
Scene *SceneManager::GetLoadedScene() const
{
    return p_loadedScene;
}

void SceneManager::SetSceneVariable(Scene **sceneVar, Scene *sceneValue)
{
    if (*sceneVar != sceneValue)
    {
        if (*sceneVar)
        {
            if (GetLoadedScene() != GetActiveScene_())
            {
                (*sceneVar)->EventEmitter<IEventsDestroy>::UnRegisterListener(
                    this);
            }
        }

        *sceneVar = sceneValue;
        if (*sceneVar)
        {
            if (GetLoadedScene() != GetActiveScene_())
            {
                (*sceneVar)->EventEmitter<IEventsDestroy>::RegisterListener(
                    this);
            }
        }
    }
}

void SceneManager::SetActiveScene_(Scene *activeScene)
{
    SetSceneVariable(&p_activeScene, activeScene);
}
void SceneManager::SetLoadedScene(Scene *loadedScene)
{
    SetSceneVariable(&p_loadedScene, loadedScene);
}

void SceneManager::LoadScene(Scene *scene, bool destroyActive)
{
    SceneManager *sm = SceneManager::GetActive();
    sm->PrepareNextLoad(scene, Path::Empty(), destroyActive);
}
void SceneManager::LoadScene(const Path &sceneFilepath, bool destroyActive)
{
    Path basePath(sceneFilepath);
    Path scenePath(basePath);
    if (!scenePath.IsFile())
    {
        if (!basePath.HasExtension(Extensions::GetSceneExtension()))
        {
            basePath =
                basePath.AppendExtension(Extensions::GetSceneExtension());
        }
        scenePath = basePath;

        if (!scenePath.IsFile())
        {
            scenePath = EPATH(basePath.GetAbsolute());
            if (!scenePath.IsFile())
            {
                scenePath = PPATH(basePath.GetAbsolute());
            }
        }
    }

    SceneManager *sm = SceneManager::GetActive();
    if (scenePath.IsFile())
    {
        Scene *scene = GameObjectFactory::CreateScene(false);
        scene->ImportMetaFromFile(sceneFilepath);
        sm->PrepareNextLoad(scene, sceneFilepath, destroyActive);
    }
    else
    {
        Debug_Error("Scene '" << sceneFilepath << "' could not be loaded.");
        sm->ClearNextLoad();
    }
}

void SceneManager::LoadSceneInstantly(Scene *scene, bool destroyActive)
{
    SceneManager *sm = SceneManager::GetActive();
    SceneManager::LoadScene(scene, destroyActive);
    sm->LoadSceneInstantly_();
}
void SceneManager::LoadSceneInstantly(const Path &sceneFilepath,
                                      bool destroyActive)
{
    SceneManager *sm = SceneManager::GetActive();
    SceneManager::LoadScene(sceneFilepath, destroyActive);
    sm->LoadSceneInstantly_();
}

void SceneManager::LoadSceneInstantly_()
{
    ASSERT(GetNextLoadNeeded());

    // Destroy previous loaded scene
    Scene *prevLoadedScene = GetLoadedScene();
    if (GetNextLoadDestroyPrevious() && prevLoadedScene)
    {
        GameObject::DestroyImmediate(prevLoadedScene);
    }
    AudioManager::StopAllSounds();

    // Load scene
    SetLoadedScene(GetNextLoadScene());
    Scene *loadedScene = GetLoadedScene();
    if (loadedScene)
    {
        loadedScene->InvalidateCanvas();
    }

    if (!GetActiveScene_())
    {
        SetActiveScene_(loadedScene);
    }

    // Propagate loaded event and clear next load variables
    EventEmitter<IEventsSceneManager>::PropagateToListeners(
        &IEventsSceneManager::OnSceneLoaded,
        GetNextLoadScene(),
        GetNextLoadScenePath());
    ClearNextLoad();
}

List<GameObject *> SceneManager::FindDontDestroyOnLoadGameObjects(
    GameObject *go)
{
    List<GameObject *> result;
    for (GameObject *child : go->GetChildren())
    {
        if (child->IsDontDestroyOnLoad())
        {
            result.PushBack(child);
        }
        else
        {
            result.PushBack(FindDontDestroyOnLoadGameObjects(child));
        }
    }
    return result;
}

void SceneManager::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (object == GetNextLoadScene())
    {
        ClearNextLoad();
    }
    if (object == GetLoadedScene())
    {
        p_loadedScene = nullptr;
    }
    if (object == GetActiveScene_())
    {
        p_activeScene = nullptr;
    }
}

bool SceneManager::GetNextLoadNeeded() const
{
    return m_nextLoadNeeded;
}
Scene *SceneManager::GetNextLoadScene() const
{
    return p_nextLoadScene;
}
const Path &SceneManager::GetNextLoadScenePath() const
{
    return m_nextLoadScenePath;
}
bool SceneManager::GetNextLoadDestroyPrevious() const
{
    return m_nextLoadDestroyPrevious;
}

Scene *SceneManager::GetObjectPtrLookupScene_() const
{
    return GetActiveScene_();
}

BehaviourManager *SceneManager::GetBehaviourManager() const
{
    return m_behaviourManager;
}

void SceneManager::ClearNextLoad()
{
    m_nextLoadNeeded = false;
    p_nextLoadScene = nullptr;
    m_nextLoadScenePath = Path::Empty();
    m_nextLoadDestroyPrevious = false;
}

void SceneManager::PrepareNextLoad(Scene *scene,
                                   const Path &scenePath,
                                   bool destroyActive)
{
    if (GetNextLoadScene() && GetNextLoadScene() != GetActiveScene_())
    {
        GetNextLoadScene()->EventEmitter<IEventsDestroy>::UnRegisterListener(
            this);
    }

    m_nextLoadNeeded = true;
    p_nextLoadScene = scene;
    m_nextLoadScenePath = scenePath;
    m_nextLoadDestroyPrevious = destroyActive;

    if (GetNextLoadScene())
    {
        GetNextLoadScene()->EventEmitter<IEventsDestroy>::RegisterListener(
            this);
    }
}
