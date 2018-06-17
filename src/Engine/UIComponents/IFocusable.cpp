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

IEventsFocus::Event::PropagationResult
IFocusable::ProcessEvent(IEventsFocus::Event event)
{
    switch (event.type)
    {
        case IEventsFocus::Event::Type::MOUSE_ENTER:
            SetIsMouseOver(true);
        break;

        case IEventsFocus::Event::Type::MOUSE_EXIT:
            SetIsMouseOver(false);
        break;

        case IEventsFocus::Event::Type::STARTED_BEING_PRESSED:
            SetBeingPressed(true);
        break;

        case IEventsFocus::Event::Type::FINISHED_BEING_PRESSED:
            SetBeingPressed(false);
        break;

        default: break;
    }

    // Propagate events
    IEventsFocus::Event::PropagationResult finalResult =
            IEventsFocus::Event::PropagationResult::PROPAGATE_TO_PARENT;
    for (EventCallback eventCallback : m_eventCallbacks)
    {
        IEventsFocus::Event::PropagationResult propagationResult =
                                                    eventCallback(this, event);
        if (propagationResult ==
            IEventsFocus::Event::PropagationResult::STOP_PROPAGATION)
        {
            finalResult = IEventsFocus::Event::PropagationResult::STOP_PROPAGATION;
        }
    }
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
