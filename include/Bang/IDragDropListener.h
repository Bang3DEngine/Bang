#ifndef IDRAGDROPLISTENER_H
#define IDRAGDROPLISTENER_H

#include "Bang/EventListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIDragDroppable;

class IDragDropListener
{
    EVENTLISTENER(IDragDropListener);

public:

    virtual void OnDragStarted(UIDragDroppable *dragDroppable) { (void) dragDroppable; }
    virtual void OnDragUpdate(UIDragDroppable *dragDroppable) { (void) dragDroppable; }
    virtual void OnDrop(UIDragDroppable *dragDroppable) { (void) dragDroppable; }
};

NAMESPACE_BANG_END

#endif // IDRAGDROPLISTENER_H

