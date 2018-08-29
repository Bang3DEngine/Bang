#ifndef IFOCUSABLE_H
#define IFOCUSABLE_H

#include "Bang/Key.h"
#include "Bang/Array.h"
#include "Bang/Cursor.h"
#include "Bang/Vector2.h"
#include "Bang/EventEmitter.h"
#include "Bang/IEventsFocus.h"

NAMESPACE_BANG_BEGIN

class IFocusable : public EventEmitter<IEventsFocus>
{
public:
    using EventCallback = std::function<UIEventResult(IFocusable*,
                                                      const UIEvent&)>;

    UIEventResult ProcessEvent(const UIEvent &event);

    void SetFocusEnabled(bool focusEnabled);
    void SetCursorType(Cursor::Type cursorType);
    void AddEventCallback(IFocusable::EventCallback eventCallback);
    void ClearEventCallbacks();

    bool HasFocus() const;
    bool IsFocusEnabled() const;
    bool HasJustFocusChanged() const;
    bool IsBeingPressed() const;
    bool IsMouseOver() const;
    Cursor::Type GetCursorType() const;

protected:
    IFocusable();
    virtual ~IFocusable();

private:
    bool m_beingPressed = false;
    bool m_hasFocus = false;
    bool m_isMouseOver = false;
    bool m_focusEnabled = true;
    bool m_hasJustFocusChanged = false;
    bool m_lastMouseDownWasHere = false;
    Cursor::Type m_cursorType = Cursor::Type::ARROW;

    Array<EventCallback> m_eventCallbacks;

    void UpdateFromCanvas();
    void SetBeingPressed(bool beingPressed);
    void SetIsMouseOver(bool isMouseOver);
    void SetFocus();
    void ClearFocus();

    friend class UICanvas;
};

NAMESPACE_BANG_END

#endif // IFOCUSABLE_H

