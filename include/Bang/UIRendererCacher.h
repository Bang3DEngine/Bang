#ifndef UIRENDERERCACHER_H
#define UIRENDERERCACHER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsComponent.h"
#include "Bang/IEventsGameObjectVisibilityChanged.h"
#include "Bang/IEventsRendererChanged.h"
#include "Bang/IEventsTransform.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"

namespace Bang
{
class Framebuffer;
class GameObject;
class IEventsChildren;
class IEventsComponent;
class IEventsGameObjectVisibilityChanged;
class IEventsRendererChanged;
class IEventsTransform;
class Object;
class Renderer;
class UIImageRenderer;

class UIRendererCacher
    : public Component,
      public EventListener<IEventsChildren>,
      public EventListener<IEventsTransform>,
      public EventListener<IEventsComponent>,
      public EventListener<IEventsRendererChanged>,
      public EventListener<IEventsGameObjectVisibilityChanged>
{
    COMPONENT(UIRendererCacher)

public:
    UIRendererCacher();
    virtual ~UIRendererCacher() override;

    void OnStart() override;
    void OnRender(RenderPass renderPass) override;
    void OnAfterChildrenRender(RenderPass renderPass) override;

    void SetCachingEnabled(bool enabled);

    bool IsCachingEnabled() const;
    GameObject *GetContainer() const;

    void OnChanged();

    // IEventsComponent
    virtual void OnComponentAdded(Component *addedComponent, int index);
    virtual void OnComponentRemoved(Component *removedComponent,
                                    GameObject *previousGameObject);

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
    static UIRendererCacher *CreateInto(GameObject *go);

    // IEventsGameObjectVisibilityChanged
    void OnVisibilityChanged(GameObject *go) override;

    friend class GameObjectFactory;
};
}

#endif  // UIRENDERERCACHER_H
