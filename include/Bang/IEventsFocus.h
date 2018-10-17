#ifndef IEVENTSFOCUS_H
#define IEVENTSFOCUS_H

#include "Bang/IEvents.h"
#include "Bang/Key.h"
#include "Bang/MouseButton.h"
#include "Bang/Vector2.h"

namespace Bang
{
class UIFocusable;

enum class UIEventResult
{
    IGNORE,
    INTERCEPT
};

enum class FocusType
{
    MOUSE,
    AUTO_TAB
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
        KEY_DOWN,
        KEY_UP,
        WHEEL
    };

    Type type;
    Vector2i mousePosWindow;
    struct
    {
        MouseButton button;
    } mouse;

    struct
    {
        Key key;
        KeyModifiers modifiers;
    } key;

    struct
    {
        FocusType type = FocusType::MOUSE;
    } focus;

    struct
    {
        Vector2 amount;
    } wheel;
};

class IEventsFocus
{
    IEVENTS(IEventsFocus);

public:
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event)
    {
        BANG_UNUSED_2(focusable, event);
        return UIEventResult::IGNORE;
    }
};
}

#endif  // IEVENTSFOCUS_H
