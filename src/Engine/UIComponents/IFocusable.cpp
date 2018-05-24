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
    // Handle all cases :)
    if (IsMouseOver())
    {
        bool doubleClick = false;
        if (m_lastMouseDownWasHere) // Order dependent
        {
            if (Input::GetMouseButtonDoubleClick(MouseButton::Left))
            {
                Click(ClickType::Double);
                doubleClick = true;
            }
        }

        if (!doubleClick)
        {
            // Then this
            if (Input::GetMouseButtonDown(MouseButton::Left)) // Order dependent
            {
                if (!IsBeingPressed())
                {
                    m_beingPressed = true;
                    EventEmitter<IFocusListener>::PropagateToListeners(
                                &IFocusListener::OnStartedBeingPressed, this);
                }

                m_lastMouseDownWasHere = true;
                Click(ClickType::Down);
            }

            // Order dependent. Must go after detecting button down, to detect cases
            // in which down&up are in the same frame
            if (m_lastMouseDownWasHere)
            {
                if (Input::GetMouseButtonUp(MouseButton::Left))
                {
                    Click(ClickType::Full);
                }
            }
        }
    }
    else
    {
        if (Input::GetMouseButtonDown(MouseButton::Left))
        {
            m_lastMouseDownWasHere = false;
        }
    }

    if (IsBeingPressed() && !Input::GetMouseButton(MouseButton::Left))
    {
        EventEmitter<IFocusListener>::PropagateToListeners(
                    &IFocusListener::OnStoppedBeingPressed, this);
        m_beingPressed = false;
    }
}

Cursor::Type IFocusable::GetCursorType() const { return m_cursorType; }
bool IFocusable::IsBeingPressed() const { return m_beingPressed; }
void IFocusable::AddClickedCallback(ClickedCallback callback)
{
    m_clickedCallbacks.PushBack(callback);
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

void IFocusable::Click(ClickType clickType)
{
    PropagateOnClickedToListeners(clickType);
}

bool IFocusable::HasFocus() const { return m_hasFocus; }
bool IFocusable::IsFocusEnabled() const { return m_focusEnabled; }
bool IFocusable::HasJustFocusChanged() const { return m_hasJustFocusChanged; }

void IFocusable::SetFocus()
{
    if (!HasFocus())
    {
        m_hasFocus = true;
        m_hasJustFocusChanged = true;
        PropagateFocusToListeners();
    }
}

void IFocusable::ClearFocus()
{
    if (HasFocus())
    {
        m_hasFocus = false;
        m_hasJustFocusChanged = true;
        PropagateFocusToListeners();
    }
}

void IFocusable::PropagateMouseOverToListeners(bool mouseOver)
{
    if (IsMouseOver() != mouseOver)
    {
        m_isMouseOver = mouseOver;
        if (IsMouseOver())
        {
            EventEmitter<IFocusListener>::
                    PropagateToListeners(&IFocusListener::OnMouseEnter, this);
        }
        else
        {
            EventEmitter<IFocusListener>::
                    PropagateToListeners(&IFocusListener::OnMouseExit, this);
        }
    }
}


void IFocusable::PropagateFocusToListeners()
{
    if (HasFocus())
    {
        EventEmitter<IFocusListener>::
                PropagateToListeners(&IFocusListener::OnFocusTaken, this);
    }
    else
    {
        EventEmitter<IFocusListener>::
            PropagateToListeners(&IFocusListener::OnFocusLost, this);
    }
}

void IFocusable::PropagateOnClickedToListeners(ClickType clickType)
{
    if (IsEmittingEvents())
    {
        EventEmitter<IFocusListener>::
            PropagateToListeners(&IFocusListener::OnClicked, this, clickType);
        for (auto callback : m_clickedCallbacks) { callback(this, clickType); }
    }
}
