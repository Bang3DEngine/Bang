#include "Bang/ILayoutController.h"

#include "Bang/ILayoutElement.h"
#include "Bang/UILayoutManager.h"

USING_NAMESPACE_BANG

ILayoutController::ILayoutController()
{
}

ILayoutController::~ILayoutController()
{
}

bool ILayoutController::IsSelfController() const
{
    return false;
}

void ILayoutController::Invalidate()
{
    IInvalidatable<ILayoutController>::Invalidate();
}

void ILayoutController::OnInvalidated()
{
    IInvalidatable<ILayoutController>::OnInvalidated();
    if (UILayoutManager *uilm = UILayoutManager::GetActive(this))
    {
        uilm->PropagateInvalidation(this);
    }
}

void ILayoutController::_ApplyLayout(Axis axis)
{
    if (IInvalidatable<ILayoutController>::IsInvalid())
    {
        ApplyLayout(axis);
        if (axis == Axis::VERTICAL) { Validate(); }
    }
}

void ILayoutController::OnChildAdded(GameObject *addedChild,
                                     GameObject *parent)
{
    (void) addedChild;
    (void) parent;
    Invalidate();
}

void ILayoutController::OnChildRemoved(GameObject *removedChild,
                                       GameObject *parent)
{
    (void) removedChild;
    (void) parent;
    Invalidate();
}

void ILayoutController::OnParentChanged(GameObject*, GameObject*)
{
    Invalidate();
}

void ILayoutController::OnTransformChanged()
{
    Invalidate();
}

void ILayoutController::OnParentTransformChanged()
{
    Invalidate();
}
