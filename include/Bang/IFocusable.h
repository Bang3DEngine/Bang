#ifndef IFOCUSABLE_H
#define IFOCUSABLE_H

#include "Bang/Cursor.h"
#include "Bang/IEventEmitter.h"
#include "Bang/IFocusListener.h"

NAMESPACE_BANG_BEGIN

class IFocusable : public EventEmitter<IFocusListener>
{
public:
    bool IsMouseOver() const;

    void SetFocusEnabled(bool focusEnabled);
    void SetCursorType(Cursor::Type cursorType);

    void Click(ClickType clickType);
    bool HasFocus() const;
    bool IsFocusEnabled() const;
    bool HasJustFocusChanged() const;
    bool IsBeingPressed() const;
    Cursor::Type GetCursorType() const;

    using ClickedCallback = std::function<void(IFocusable*, ClickType clickType)>;
    void AddClickedCallback(ClickedCallback callback);

protected:
    IFocusable();
    virtual ~IFocusable();

    void SetFocus();
    void ClearFocus();
    virtual void PropagateFocusToListeners();
    virtual void PropagateOnClickedToListeners(ClickType clickType);
    virtual void PropagateMouseOverToListeners(bool mouseOver);

private:
    bool m_beingPressed = false;
    bool m_hasFocus = false;
    bool m_isMouseOver = false;
    bool m_focusEnabled = true;
    bool m_hasJustFocusChanged = false;
    bool m_lastMouseDownWasHere = false;
    Cursor::Type m_cursorType = Cursor::Type::Arrow;

    Array<ClickedCallback> m_clickedCallbacks;

    void UpdateFromCanvas();

    friend class UICanvas;
};

NAMESPACE_BANG_END

#endif // IFOCUSABLE_H

