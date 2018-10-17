#include "Bang/ILayoutController.h"

#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsTransform.h"
#include "Bang/UILayoutManager.h"

using namespace Bang;

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
        if (axis == Axis::VERTICAL)
        {
            Validate();
        }
    }
}

void ILayoutController::OnChildAdded(GameObject *addedChild, GameObject *parent)
{
    Invalidate();
    addedChild->EventEmitter<IEventsChildren>::RegisterListener(this);
}

void ILayoutController::OnChildRemoved(GameObject *removedChild,
                                       GameObject *parent)
{
    Invalidate();
    removedChild->EventEmitter<IEventsChildren>::UnRegisterListener(this);
}

void ILayoutController::OnParentChanged(GameObject *, GameObject *)
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
