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
        (void) dragDropEmitter;
    }

    virtual void OnDragUpdate(EventEmitter<IEventsDragDrop> *dragDropEmitter)
    {
        (void) dragDropEmitter;
    }

    virtual void OnDrop(EventEmitter<IEventsDragDrop> *dragDropEmitter,
                        bool inside)
    {
        (void) dragDropEmitter;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSDRAGDROP_H

