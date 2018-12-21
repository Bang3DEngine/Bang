#ifndef UIRENDERER_H
#define UIRENDERER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsTransform.h"
#include "Bang/RenderPass.h"
#include "Bang/Renderer.h"
#include "Bang/String.h"

namespace Bang
{
class Camera;
class GameObject;
class IEventsChildren;
class IEventsTransform;
class Object;

class UIRenderer : public Renderer,
                   public EventListener<IEventsChildren>,
                   public EventListener<IEventsTransform>
{
    COMPONENT(UIRenderer)

public:
    virtual AARect GetBoundingRect(Camera *camera) const override;

    // Renderer
    virtual void OnRender() override;
    virtual void OnRender(RenderPass renderPass) override;

    void SetCullByRectTransform(bool cullByRectTransform);
    bool GetCullByRectTransform() const;

    // IEventsObject
    virtual void OnEnabled(Object *object) override;
    virtual void OnDisabled(Object *object) override;

    // IEventsTransform
    virtual void OnTransformChanged() override;

    // IEventsChildren
    virtual void OnChildAdded(GameObject *addedChild,
                              GameObject *parent) override;
    virtual void OnChildRemoved(GameObject *removedChild,
                                GameObject *parent) override;
    virtual void OnParentChanged(GameObject *oldParent,
                                 GameObject *newParent) override;

protected:
    UIRenderer();
    virtual ~UIRenderer() override;

private:
    bool m_cullByRectTransform = true;
};
}

#endif  // UIRENDERER_H
