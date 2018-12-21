#include "Bang/UIFocusable.h"

#include "Bang/ClassDB.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/UICanvas.h"

using namespace Bang;

UIFocusable::UIFocusable()
{
    SET_INSTANCE_CLASS_ID(UIFocusable);
}

UIFocusable::~UIFocusable()
{
}

UIEventResult UIFocusable::ProcessEvent(const UIEvent &event)
{
    UIEventResult finalResult = UIEventResult::IGNORE;
    if (!IsWaitingToBeDestroyed())
    {
        switch (event.type)
        {
            case UIEvent::Type::MOUSE_ENTER: SetIsMouseOver(true); break;

            case UIEvent::Type::MOUSE_EXIT: SetIsMouseOver(false); break;

            case UIEvent::Type::STARTED_BEING_PRESSED:
                SetBeingPressed(true);
                break;

            case UIEvent::Type::MOUSE_CLICK_FULL:
            case UIEvent::Type::MOUSE_CLICK_DOUBLE:
            case UIEvent::Type::FINISHED_BEING_PRESSED:
                SetBeingPressed(false);
                break;

            default: break;
        }

        // Propagate events
        if (EventEmitter<IEventsFocus>::IsEmittingEvents())
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
                EventEmitter<IEventsFocus>::PropagateToListenersAndGatherResult<
                    UIEventResult>(&IEventsFocus::OnUIEvent, this, event);
            for (UIEventResult eventListenerResult : eventListenerResults)
            {
                if (eventListenerResult == UIEventResult::INTERCEPT)
                {
                    finalResult = UIEventResult::INTERCEPT;
                }
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

void UIFocusable::SetFocus()
{
    if (!HasFocus())
    {
        m_hasFocus = true;
    }
}

void UIFocusable::ClearFocus()
{
    if (HasFocus())
    {
        m_hasFocus = false;
    }
}

void UIFocusable::SetBeingPressed(bool beingPressed)
{
    if (beingPressed != IsBeingPressed())
    {
        m_beingPressed = beingPressed;
        if (UICanvas *canvas = UICanvas::GetActive(this))
        {
            if (IsBeingPressed())
            {
                canvas->RegisterFocusableBeingPressed(this);
            }
            else
            {
                canvas->RegisterFocusableNotBeingPressedAnymore(this);
            }
        }
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
