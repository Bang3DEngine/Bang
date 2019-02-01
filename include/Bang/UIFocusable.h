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
    UIEventResult ProcessEvent(const UIEvent &event);

    void SetCursorType(Cursor::Type cursorType);
    void SetCanBeRectMasked(bool canBeRectMasked);
    void SetConsiderForTabbing(bool considerForTabbing);

    using EventCallback =
        std::function<UIEventResult(UIFocusable *, const UIEvent &)>;
    void AddEventCallback(
        std::function<UIEventResult(UIFocusable *, const UIEvent &)>
            eventCallback);

    void ClearEventCallbacks();

    bool HasFocus() const;
    bool HasJustFocusChanged() const;
    bool IsBeingPressed() const;
    bool IsMouseOver() const;
    bool GetCanBeRectMasked() const;
    Cursor::Type GetCursorType() const;
    bool GetConsiderForTabbing() const;

private:
    bool m_hasFocus = false;
    bool m_isMouseOver = false;
    bool m_beingPressed = false;
    bool m_canBeRectMasked = false;
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
