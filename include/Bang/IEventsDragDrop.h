#ifndef IEVENTSDRAGDROP_H
#define IEVENTSDRAGDROP_H

#include "Bang/IEvents.h"

NAMESPACE_BANG_BEGIN

class IEventsDragDrop
{
    IEVENTS(IEventsDragDrop);

public:
    virtual void OnDragStarted(EventEmitter<IEventsDragDrop> *dragDropEmitter)
    {
        BANG_UNUSED(dragDropEmitter);
    }

    virtual void OnDragUpdate(EventEmitter<IEventsDragDrop> *dragDropEmitter)
    {
        BANG_UNUSED(dragDropEmitter);
    }

    virtual void OnDrop(EventEmitter<IEventsDragDrop> *dragDropEmitter)
    {
        BANG_UNUSED(dragDropEmitter);
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSDRAGDROP_H

