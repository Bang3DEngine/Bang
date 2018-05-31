#ifndef IUILISTLISTENER_H
#define IUILISTLISTENER_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

class IUIListListener
{
public:
    virtual void OnItemAdded(GameObject *item, int index) { (void)item; (void)index; }

    virtual void OnItemMoved(GameObject *item, int oldIndex, int newIndex)
    { (void)item; (void)oldIndex; (void)newIndex; }

    virtual void OnItemRemoved(GameObject *item) { (void)item; }
};

NAMESPACE_BANG_END

#endif // IUILISTLISTENER_H

