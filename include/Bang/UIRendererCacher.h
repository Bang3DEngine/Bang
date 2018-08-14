#ifndef UIRENDERERCACHER_H
#define UIRENDERERCACHER_H

#include <unordered_map>

#include "Bang/Component.h"
#include "Bang/RenderPass.h"
#include "Bang/UIRenderer.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsRendererChanged.h"
#include "Bang/IEventsGameObjectVisibilityChanged.h"

NAMESPACE_BANG_BEGIN

FORWARD class Framebuffer;
FORWARD class ShaderProgram;
FORWARD class UIImageRenderer;

class UIRendererCacher : public Component,
                         public EventListener<IEventsChildren>,
                         public EventListener<IEventsTransform>,
                         public EventListener<IEventsRendererChanged>,
                         public EventListener<IEventsGameObjectVisibilityChanged>
{
    COMPONENT( UIRendererCacher )

public:
	UIRendererCacher();
	virtual ~UIRendererCacher();

    void OnStart() override;
    void OnRender(RenderPass renderPass) override;
    void OnAfterChildrenRender(RenderPass renderPass) override;

    void SetCachingEnabled(bool enabled);

    bool IsCachingEnabled() const;
    GameObject *GetContainer() const;

    void OnChanged();

    // IEventsChildren
    void OnChildAdded(GameObject *addedChild, GameObject *parent) override;
    void OnChildRemoved(GameObject *removedChild, GameObject *parent) override;

    // IEventsTransform
    void OnTransformChanged() override;

    // IEventsRendererChanged
    void OnRendererChanged(Renderer *changedRenderer) override;

    // IEventsObject
    void OnStarted(Object *object) override;
    void OnEnabled(Object *object) override;
    void OnDisabled(Object *object) override;

private:
    bool m_cachingEnabled = true;
    bool m_needNewImageToSnapshot = true;

    Framebuffer *p_cacheFramebuffer = nullptr;
    GameObject *p_uiRenderersContainer = nullptr;
    UIImageRenderer *p_cachedImageRenderer = nullptr;

    void SetContainerVisible(bool visible);
    static UIRendererCacher* CreateInto(GameObject *go);

    // IEventsGameObjectVisibilityChanged
    void OnVisibilityChanged(GameObject *go) override;

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UIRENDERERCACHER_H

