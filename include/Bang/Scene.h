#ifndef SCENE_H
#define SCENE_H

#include <queue>

#include "Bang/BangDefines.h"
#include "Bang/GameObject.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/List.h"
#include "Bang/MetaNode.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"
#include "Bang/Time.h"

NAMESPACE_BANG_BEGIN

FORWARD   class Camera;
FORWARD   class DebugRenderer;
FORWARD   class ICloneable;
FORWARD   class IEventsDestroy;
FORWARD_T class EventEmitter;
FORWARD   class RenderFactory;

class Scene : public GameObject
{
    GAMEOBJECT(Scene);

public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void Render(RenderPass rp, bool renderChildren = true) override;
    virtual void OnResize(int newWidth, int newHeight);

    void SetCamera(Camera *cam);

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

NAMESPACE_BANG_END

#endif // SCENE_H
