#ifndef IEVENTSFOCUS_H
#define IEVENTSFOCUS_H

#include "Bang/Key.h"
#include "Bang/Vector2.h"
#include "Bang/IEvents.h"
#include "Bang/MouseButton.h"

NAMESPACE_BANG_BEGIN

FORWARD class IFocusable;

enum class UIEventResult
{
    IGNORE,
    INTERCEPT
};

struct UIEvent
{
    enum class Type
    {
        MOUSE_CLICK_DOWN,
        MOUSE_CLICK_UP,
        MOUSE_CLICK_FULL,
        MOUSE_CLICK_DOUBLE,
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
        MouseButton button;
        Vector2i delta;
    }
    mouse;

    struct
    {
        Key key;
    }
    key;

    struct
    {
        Vector2 amount;
    }
    wheel;
};

class IEventsFocus
{
    IEVENTS(IEventsFocus);

public:
    virtual void OnEvent(IFocusable *focusable, const UIEvent &event)
    {
        (void) focusable;
        (void) event;
    }
};

NAMESPACE_BANG_END

#endif // IEVENTSFOCUS_H
