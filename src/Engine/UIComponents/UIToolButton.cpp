#include "Bang/UIToolButton.h"

#include "Bang/GameObject.h"

USING_NAMESPACE_BANG

UIToolButton::UIToolButton()
{
    CONSTRUCT_CLASS_ID(UIToolButton)
}

UIToolButton::~UIToolButton()
{

}

void UIToolButton::SetOn(bool on)
{
    if (on != GetOn())
    {
        if (!IsBlocked())
        {
            m_on = on;
            ChangeAspectIfItsOnOrNot();
        }
    }
}

void UIToolButton::ChangeAspectIfItsOnOrNot()
{
    if (GetOn())
    {
        ChangeAspectToPressed();
    }
    else
    {
        ChangeAspectToIdle();
    }
}

void UIToolButton::OnBlockedChanged()
{
    if (!IsBlocked())
    {
        ChangeAspectIfItsOnOrNot();
    }
}


bool UIToolButton::GetOn() const
{
    return m_on;
}

void UIToolButton::Click()
{
    SetOn( !GetOn() );
    CallClickCallback();
}

UIEventResult UIToolButton::OnUIEvent(UIFocusable *focusable,
                                      const UIEvent &event)
{
    if (!IsBlocked())
    {
        switch (event.type)
        {
            case UIEvent::Type::STARTED_BEING_PRESSED:
            case UIEvent::Type::FINISHED_BEING_PRESSED:
            case UIEvent::Type::MOUSE_CLICK_DOWN:
            case UIEvent::Type::MOUSE_CLICK_UP:
            case UIEvent::Type::MOUSE_ENTER:
            case UIEvent::Type::MOUSE_EXIT:
                return UIEventResult::INTERCEPT;
            break;

            default:
            break;
        }
    }

    return UIButtonBase::OnUIEvent(focusable, event);
}

UIToolButton *UIToolButton::CreateInto(GameObject *gameObject)
{
    return SCAST<UIToolButton*>(
    UIButtonBase::CreateInto([](GameObject *go)
    {
        return go->AddComponent<UIToolButton>();
    },
    gameObject));
}
