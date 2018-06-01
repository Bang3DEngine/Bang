#ifndef IEVENTSFOCUS_H
#define IEVENTSFOCUS_H

#include "Bang/IEvents.h"
#include "Bang/ClickType.h"

NAMESPACE_BANG_BEGIN

class IEventsFocus
{
    IEVENTS(IEventsFocus);

public:
    virtual void OnFocusTaken(EventEmitter<IEventsFocus> *focusEmitter)
    {
        (void) focusEmitter;
    }
    virtual void OnFocusLost(EventEmitter<IEventsFocus> *focusEmitter)
    {
        (void) focusEmitter;
    }

    virtual void OnMouseEnter(EventEmitter<IEventsFocus> *focusEmitter)
    {
        (void) focusEmitter;
    }
    virtual void OnMouseExit(EventEmitter<IEventsFocus> *focusEmitter)
    {
        (void) focusEmitter;
    }

    virtual void OnStartedBeingPressed(EventEmitter<IEventsFocus> *focusEmitter)
    {
        (void) focusEmitter;
    }
    virtual void OnStoppedBeingPressed(EventEmitter<IEventsFocus> *focusEmitter)
    {
        (void) focusEmitter;
    }

    virtual void OnClicked(EventEmitter<IEventsFocus> *focusEmitter,
                           ClickType clickType)
    {
        (void) focusEmitter;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSFOCUS_H
