#include "Bang/IFocusable.h"

#include "Bang/Input.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"

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

UIEventResult IFocusable::ProcessEvent(const UIEventExt &event)
{
    switch (event.type)
    {
        case UIEventExt::Type::MOUSE_ENTER:
            SetIsMouseOver(true);
        break;

        case UIEventExt::Type::MOUSE_EXIT:
            SetIsMouseOver(false);
        break;

        case UIEventExt::Type::STARTED_BEING_PRESSED:
            SetBeingPressed(true);
        break;

        case UIEventExt::Type::MOUSE_CLICK_FULL:
        case UIEventExt::Type::MOUSE_CLICK_DOUBLE:
        case UIEventExt::Type::FINISHED_BEING_PRESSED:
            SetBeingPressed(false);
        break;

        default:
        break;
    }

    // Propagate events
    UIEventResult finalResult = UIEventResult::IGNORE;
    if (IFocusable::IsEmittingEvents() && IsFocusEnabled())
    {
        for (EventCallback eventCallback : m_eventCallbacks)
        {
            UIEventResult propagationResult = eventCallback(this, event);
            if (propagationResult == UIEventResult::INTERCEPT)
            {
                finalResult = UIEventResult::INTERCEPT;
            }
        }
        EventEmitter<IEventsFocus>::PropagateToListeners(&IEventsFocus::OnUIEvent,
                                                         this, event);
    }

    return finalResult;
}

void IFocusable::AddEventCallback(IFocusable::EventCallback eventCallback)
{
    m_eventCallbacks.PushBack(eventCallback);
}

void IFocusable::ClearEventCallbacks()
{
    m_eventCallbacks.Clear();
}
bool IFocusable::IsBeingPressed() const
{
    return m_beingPressed;
}

Cursor::Type IFocusable::GetCursorType() const
{
    return m_cursorType;
}

bool IFocusable::GetConsiderForTabbing() const
{
    return m_considerForTabbing;
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

void IFocusable::SetConsiderForTabbing(bool considerForTabbing)
{
    m_considerForTabbing = considerForTabbing;
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
    if (beingPressed != IsBeingPressed())
    {
        m_beingPressed = beingPressed;
    }
}

void IFocusable::SetIsMouseOver(bool isMouseOver)
{
    if (isMouseOver != IsMouseOver())
    {
        m_isMouseOver = isMouseOver;
    }
}
