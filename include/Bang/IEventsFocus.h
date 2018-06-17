#ifndef IEVENTSFOCUS_H
#define IEVENTSFOCUS_H

#include "Bang/Key.h"
#include "Bang/Vector2.h"
#include "Bang/IEvents.h"
#include "Bang/ClickType.h"
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
