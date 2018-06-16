#ifndef IEVENTSFOCUS_H
#define IEVENTSFOCUS_H

#include "Bang/Key.h"
#include "Bang/Vector2.h"
#include "Bang/IEvents.h"
#include "Bang/ClickType.h"
#include "Bang/MouseButton.h"

NAMESPACE_BANG_BEGIN

class IEventsFocus
{
    IEVENTS(IEventsFocus);

public:
    struct Event
    {
        enum class PropagationResult
        {
            PROPAGATE_TO_PARENT,
            STOP_PROPAGATION
        };

        enum class Type
        {
            MOUSE_CLICK,
            MOUSE_ENTER,
            STARTED_BEING_PRESSED,
            FINISHED_BEING_PRESSED,
            MOUSE_MOVE,
            MOUSE_EXIT,
            FOCUS_TAKEN,
            FOCUS_LOST,
            KEY,
            WHEEL
        };

        Type type;
        Vector2i mousePosition;
        struct
        {
            ClickType type;
            MouseButton button;
            Vector2i mouseDelta;
        }
        click;

        struct
        {
            Key key;
        }
        key;

        struct
        {
            float amount;
        }
        wheel;
    };

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
