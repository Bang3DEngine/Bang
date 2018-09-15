#include "Bang/UIFocusable.h"

#include "Bang/Input.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.h"

USING_NAMESPACE_BANG

UIFocusable::UIFocusable()
{
}

UIFocusable::~UIFocusable()
{
}

UIEventResult UIFocusable::ProcessEvent(const UIEvent &event)
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

        case UIEvent::Type::MOUSE_CLICK_FULL:
        case UIEvent::Type::MOUSE_CLICK_DOUBLE:
        case UIEvent::Type::FINISHED_BEING_PRESSED:
            SetBeingPressed(false);
        break;

        default:
        break;
    }

    // Propagate events
    UIEventResult finalResult = UIEventResult::IGNORE;
    if (EventEmitter<IEventsFocus>::IsEmittingEvents() && IsFocusEnabled())
    {
        for (EventCallback eventCallback : m_eventCallbacks)
        {
            UIEventResult propagationResult = eventCallback(this, event);
            if (propagationResult == UIEventResult::INTERCEPT)
            {
                finalResult = UIEventResult::INTERCEPT;
            }
        }

        Array<UIEventResult> eventListenerResults =
                    EventEmitter<IEventsFocus>::
                        PropagateToListenersAndGatherResult<UIEventResult>(
                                    &IEventsFocus::OnUIEvent, this, event);
        for (UIEventResult eventListenerResult : eventListenerResults)
        {
            if (eventListenerResult == UIEventResult::INTERCEPT)
            {
                finalResult = UIEventResult::INTERCEPT;
            }
        }
    }

    return finalResult;
}

void UIFocusable::AddEventCallback(UIFocusable::EventCallback eventCallback)
{
    m_eventCallbacks.PushBack(eventCallback);
}

void UIFocusable::ClearEventCallbacks()
{
    m_eventCallbacks.Clear();
}
bool UIFocusable::IsBeingPressed() const
{
    return m_beingPressed;
}

Cursor::Type UIFocusable::GetCursorType() const
{
    return m_cursorType;
}

bool UIFocusable::GetConsiderForTabbing() const
{
    return m_considerForTabbing;
}

bool UIFocusable::IsMouseOver() const
{
    return m_isMouseOver;
}

void UIFocusable::SetFocusEnabled(bool focusEnabled)
{
    m_focusEnabled = focusEnabled;
}

void UIFocusable::SetCursorType(Cursor::Type cursorType)
{
    m_cursorType = cursorType;
}

void UIFocusable::SetConsiderForTabbing(bool considerForTabbing)
{
    m_considerForTabbing = considerForTabbing;
}

bool UIFocusable::HasFocus() const
{
    return m_hasFocus;
}
bool UIFocusable::IsFocusEnabled() const
{
    return m_focusEnabled;
}
bool UIFocusable::HasJustFocusChanged() const
{
    return m_hasJustFocusChanged;
}

void UIFocusable::SetFocus()
{
    if (!HasFocus())
    {
        m_hasFocus = true;
        m_hasJustFocusChanged = true;
    }
}

void UIFocusable::ClearFocus()
{
    if (HasFocus())
    {
        m_hasFocus = false;
        m_hasJustFocusChanged = true;
    }
}

void UIFocusable::SetBeingPressed(bool beingPressed)
{
    if (beingPressed != IsBeingPressed())
    {
        m_beingPressed = beingPressed;
    }
}

void UIFocusable::SetIsMouseOver(bool isMouseOver)
{
    if (isMouseOver != IsMouseOver())
    {
        m_isMouseOver = isMouseOver;
    }
}

bool UIFocusable::CanBeRepeatedInGameObject() const
{
    return false;
}
