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
    virtual void OnPreStart() override;
    virtual void Update() override;
    virtual void Render(RenderPass rp, bool renderChildren = true) override;
    virtual void OnResize(int newWidth, int newHeight);

    void SetCamera(Camera *cam);
    void SetFirstFoundCamera();

    virtual Camera *GetCamera() const;

    void InvalidateCanvas();

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

protected:
    Camera *p_camera = nullptr;
    RenderFactory *m_gizmos = nullptr;
    DebugRenderer *p_debugRenderer = nullptr;

    Scene();
    virtual ~Scene();

    RenderFactory *GetGizmos() const;
    DebugRenderer *GetDebugRenderer() const;

private:

    friend class RenderFactory;
    friend class Window;
    friend class GEngine;
    friend class Application;
    friend class SceneManager;
    friend class DebugRenderer;
};

NAMESPACE_BANG_END

#endif // SCENE_H
