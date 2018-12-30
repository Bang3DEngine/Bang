#ifndef SCENE_H
#define SCENE_H

#include "Bang/BangDefines.h"
#include "Bang/GameObject.h"
#include "Bang/MetaNode.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"
#include "Bang/Time.h"

namespace Bang
{
template <class>
class EventEmitter;
class Camera;
class DebugRenderer;
class ICloneable;
class IEventsDestroy;

class Scene : public GameObject, public EventListener<IEventsDestroy>
{
    GAMEOBJECT(Scene);

public:
    Scene();

    virtual void Start() override;
    virtual void Update() override;
    virtual void Render(RenderPass rp, bool renderChildren = true) override;
    virtual void OnResize(int newWidth, int newHeight);

    void SetCamera(Camera *cam);

    Time GetDeltaTime() const;
    Camera *GetCamera() const;

    void InvalidateCanvas();

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

protected:
    virtual ~Scene() override;

    DebugRenderer *GetDebugRenderer() const;

private:
    Time m_deltaTime;
    Time m_lastUpdateTime;

    Camera *p_camera = nullptr;
    DebugRenderer *p_debugRenderer = nullptr;

    friend class Window;
    friend class GEngine;
    friend class Application;
    friend class SceneManager;
    friend class DebugRenderer;
};
}

#endif  // SCENE_H
