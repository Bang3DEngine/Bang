#ifndef IUITREELISTENER_H
#define IUITREELISTENER_H

#include "Bang/Bang.h"
#include "Bang/IEventListener.h"

NAMESPACE_BANG_BEGIN

class IUITreeListener : public virtual IEventListener
{
    EVENTLISTENER(IUITreeListener);

public:
    virtual void OnItemAdded(GameObject *item, GameObject *parentItem,
                             int indexInsideParent)
    {
        (void)item; (void)parentItem; (void)indexInsideParent;
    }

    virtual void OnItemMoved(GameObject *item,
                             GameObject *oldParentItem, int oldIndexInsideParent,
                             GameObject *newParentItem, int newIndexInsideParent)
    {
        (void)item;
        (void)oldParentItem; (void)oldIndexInsideParent;
        (void)newParentItem; (void)newIndexInsideParent;
    }

    virtual void OnItemRemoved(GameObject *item) { (void)item; }
};

NAMESPACE_BANG_END

#endif // IUITREELISTENER_H

