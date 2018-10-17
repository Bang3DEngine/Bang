#ifndef ILAYOUTCONTROLLER_H
#define ILAYOUTCONTROLLER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsTransform.h"
#include "Bang/IInvalidatable.h"

namespace Bang
{
class GameObject;
class IEventsChildren;
class IEventsTransform;

class ILayoutController : public IInvalidatable<ILayoutController>,
                          public EventListener<IEventsChildren>,
                          public EventListener<IEventsTransform>
{
public:
    virtual bool IsSelfController() const;

    // IInvalidatable
    virtual void Invalidate() override;
    virtual void OnInvalidated() override;

    // IEventsChildren
    void OnChildAdded(GameObject *addedChild, GameObject *parent) override;
    void OnChildRemoved(GameObject *removedChild, GameObject *parent) override;
    void OnParentChanged(GameObject *oldParent, GameObject *newParent) override;

    // IEventsTransform
    virtual void OnTransformChanged() override;
    virtual void OnParentTransformChanged() override;

protected:
    ILayoutController();
    virtual ~ILayoutController() override;

private:
    virtual void ApplyLayout(Axis axis) = 0;
    void _ApplyLayout(Axis axis);

    friend class UILayoutManager;
};
}

#endif  // ILAYOUTCONTROLLER_H
