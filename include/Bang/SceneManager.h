#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsSceneManager.h"
#include "Bang/List.h"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
class BehaviourManager;
class GameObject;
class Scene;

class SceneManager : public EventEmitter<IEventsSceneManager>,
                     public EventListener<IEventsDestroy>
{
public:
    static void LoadScene(Scene *scene, bool destroyActive = true);
    static void LoadScene(const Path &sceneFilepath, bool destroyActive = true);
    static void LoadScene(const String &sceneFilepath,
                          bool destroyActive = true);

    static void LoadSceneInstantly(Scene *scene, bool destroyActive = true);
    static void LoadSceneInstantly(const Path &sceneFilepath,
                                   bool destroyActive = true);

    void Update();
    void Render();
    void OnResize(int width, int height);
    static void OnNewFrame(Scene *scene);

    static Scene *GetActiveScene();
    static SceneManager *GetActive();
    static Scene *GetObjectPtrLookupScene();
    BehaviourManager *GetBehaviourManager() const;

protected:
    SceneManager();
    virtual ~SceneManager() override;

    void LoadSceneInstantly_();

    void SetLoadedScene(Scene *loadedScene);
    void SetActiveScene_(Scene *activeScene);

    Scene *GetLoadedScene() const;
    Scene *GetActiveScene_() const;
    bool GetNextLoadNeeded() const;
    Scene *GetNextLoadScene() const;
    const Path &GetNextLoadScenePath() const;
    bool GetNextLoadDestroyPrevious() const;
    virtual Scene *GetObjectPtrLookupScene_() const;
    void ClearNextLoad();

private:
    Scene *p_activeScene = nullptr;
    Scene *p_loadedScene = nullptr;
    BehaviourManager *m_behaviourManager = nullptr;

    bool m_nextLoadNeeded = false;
    Scene *p_nextLoadScene = nullptr;
    bool m_nextLoadDestroyPrevious = false;
    Path m_nextLoadScenePath = Path::Empty();

    void Init();
    void SetSceneVariable(Scene **sceneVariable, Scene *sceneValue);
    virtual BehaviourManager *CreateBehaviourManager() const;

    void PrepareNextLoad(Scene *scene,
                         const Path &scenePath,
                         bool destroyActive);
    static List<GameObject *> FindDontDestroyOnLoadGameObjects(GameObject *go);

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    friend class Window;
    friend class Application;
};
}  // namespace Bang

#endif  // SCENEMANAGER_H
