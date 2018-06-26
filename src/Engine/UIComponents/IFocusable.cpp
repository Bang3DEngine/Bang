#include "Bang/IFocusable.h"

#include "Bang/Input.h"

USING_NAMESPACE_BANG

IFocusable::IFocusable()
{
}

IFocusable::~IFocusable()
{
}

void IFocusable::UpdateFromCanvas()
{
}

UIEventResult IFocusable::ProcessEvent(const UIEvent &event)
{
    switch (event.type)
    {
        case UIEvent::Type::MOUSE_ENTER:
            SetIsMouseOver(true);
        break;

        case UIEvent::Type::MOUSE_EXIT:
            SetIsMouseOver(false);
        break;

        case UIEvent::Type::STARTED_BEING_PRESSED:
            SetBeingPressed(true);
        break;

        case UIEvent::Type::FINISHED_BEING_PRESSED:
            SetBeingPressed(false);
        break;

        default: break;
    }

    // Propagate events
    UIEventResult finalResult = UIEventResult::IGNORE;
    for (EventCallback eventCallback : m_eventCallbacks)
    {
        UIEventResult propagationResult = eventCallback(this, event);
        if (propagationResult == UIEventResult::INTERCEPT)
        {
            finalResult = UIEventResult::INTERCEPT;
        }
    }

    EventEmitter<IEventsFocus>::PropagateToListeners(&IEventsFocus::OnEvent,
                                                     this, event);

    return finalResult;
}

void IFocusable::AddEventCallback(IFocusable::EventCallback eventCallback)
{
    m_eventCallbacks.PushBack(eventCallback);
}
bool IFocusable::IsBeingPressed() const
{
    return m_beingPressed;
}

Cursor::Type IFocusable::GetCursorType() const
{
    return m_cursorType;
}

bool IFocusable::IsMouseOver() const
{
    return m_isMouseOver;
}

void IFocusable::SetFocusEnabled(bool focusEnabled)
{
    m_focusEnabled = focusEnabled;
}

void IFocusable::SetCursorType(Cursor::Type cursorType)
{
    m_cursorType = cursorType;
}

bool IFocusable::HasFocus() const
{
    return m_hasFocus;
}
bool IFocusable::IsFocusEnabled() const
{
    return m_focusEnabled;
}
bool IFocusable::HasJustFocusChanged() const
{
    return m_hasJustFocusChanged;
}

void IFocusable::SetFocus()
{
    if (!HasFocus())
    {
        m_hasFocus = true;
        m_hasJustFocusChanged = true;
    }
}

void IFocusable::ClearFocus()
{
    if (HasFocus())
    {
        m_hasFocus = false;
        m_hasJustFocusChanged = true;
    }
}

void IFocusable::SetBeingPressed(bool beingPressed)
{
    if (beingPressed != m_beingPressed)
    {
        m_beingPressed = beingPressed;
    }
}

void IFocusable::SetIsMouseOver(bool isMouseOver)
{
    if (isMouseOver != m_isMouseOver)
    {
        m_isMouseOver = isMouseOver;
    }
}
