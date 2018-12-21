#ifndef UIFOCUSABLE_H
#define UIFOCUSABLE_H

#include <functional>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Cursor.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsFocus.h"
#include "Bang/String.h"

namespace Bang
{
class UIFocusable : public Component, public EventEmitter<IEventsFocus>
{
    COMPONENT(UIFocusable)

public:
    using EventCallback =
        std::function<UIEventResult(UIFocusable *, const UIEvent &)>;

    UIEventResult ProcessEvent(const UIEvent &event);

    void SetCursorType(Cursor::Type cursorType);
    void SetConsiderForTabbing(bool considerForTabbing);
    void AddEventCallback(UIFocusable::EventCallback eventCallback);
    void ClearEventCallbacks();

    bool HasFocus() const;
    bool HasJustFocusChanged() const;
    bool IsBeingPressed() const;
    bool IsMouseOver() const;
    Cursor::Type GetCursorType() const;
    bool GetConsiderForTabbing() const;

private:
    bool m_hasFocus = false;
    bool m_isMouseOver = false;
    bool m_beingPressed = false;
    bool m_considerForTabbing = false;
    Cursor::Type m_cursorType = Cursor::Type::ARROW;

    Array<EventCallback> m_eventCallbacks;

    UIFocusable();
    virtual ~UIFocusable() override;

    // Component
    bool CanBeRepeatedInGameObject() const override;

    void SetBeingPressed(bool beingPressed);
    void SetIsMouseOver(bool isMouseOver);
    void SetFocus();
    void ClearFocus();

    friend class UICanvas;
};
}

#endif  // UIFOCUSABLE_H
