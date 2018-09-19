#ifndef UIRENDERER_H
#define UIRENDERER_H

#include "Bang/Color.h"
#include "Bang/Renderer.h"
#include "Bang/Component.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"
#include "Bang/IEventsObject.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsTransform.h"

NAMESPACE_BANG_BEGIN

class UIRenderer : public Renderer,
                   public EventListener<IEventsChildren>,
                   public EventListener<IEventsTransform>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIRenderer)

public:
    virtual AARect GetBoundingRect(Camera *camera) const override;

    // Renderer
    virtual void OnRender() override;
    virtual void OnRender(RenderPass renderPass) override;

    void SetCullByRectTransform(bool cullByRectTransform);
    bool GetCullByRectTransform() const;

    // IEventsObject
    virtual void OnEnabled(Object *object)  override;
    virtual void OnDisabled(Object *object) override;

    // IEventsTransform
    virtual void OnTransformChanged() override;

    // IEventsChildren
    virtual void OnChildAdded(GameObject *addedChild, GameObject *parent) override;
    virtual void OnChildRemoved(GameObject *removedChild, GameObject *parent) override;
    virtual void OnParentChanged(GameObject *oldParent, GameObject *newParent) override;

protected:
    UIRenderer();
    virtual ~UIRenderer();

private:
    bool m_cullByRectTransform = true;

};

NAMESPACE_BANG_END

#endif // UIRENDERER_H
