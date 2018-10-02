#ifndef SCENE_H
#define SCENE_H

#include <queue>

#include "Bang/List.h"
#include "Bang/GameObject.h"
#include "Bang/IEventsDestroy.h"

NAMESPACE_BANG_BEGIN

FORWARD class Camera;
FORWARD class RenderFactory;
FORWARD class DebugRenderer;

class Scene : public GameObject,
              public EventListener<IEventsDestroy>
{
    GAMEOBJECT(Scene);

public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void PostUpdate() override;
    virtual void Render(RenderPass rp, bool renderChildren = true) override;
    virtual void OnResize(int newWidth, int newHeight);
    void DestroyDelayedElements();

    void SetCamera(Camera *cam);

    void AddGameObjectToDestroyDelayed(GameObject *go);
    void AddComponentToDestroyDelayed(Component *comp);

    Time GetDeltaTime() const;
    Camera *GetCamera() const;

    void InvalidateCanvas();

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

protected:
    Scene();
    virtual ~Scene();

    DebugRenderer *GetDebugRenderer() const;

private:
    Time m_lastUpdateTime;

    Camera *p_camera = nullptr;
    DebugRenderer *p_debugRenderer = nullptr;

    Array<GameObject*> m_gameObjectsToDestroyDelayed;
    Array<Component*> m_componentsToDestroyDelayed;

    friend class Window;
    friend class GEngine;
    friend class Application;
    friend class SceneManager;
    friend class DebugRenderer;
};

NAMESPACE_BANG_END

#endif // SCENE_H
