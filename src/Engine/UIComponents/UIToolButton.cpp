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

            if (GetOn())
            {
                ChangeAspectToPressed();
            }
            else
            {
                ChangeAspectToIdle();
            }
        }
    }
}

bool UIToolButton::GetOn() const
{
    return m_on;
}

UIEventResult UIToolButton::OnUIEvent(UIFocusable *focusable,
                                      const UIEvent &event)
{
    if (!IsBlocked())
    {
        switch (event.type)
        {
            case UIEvent::Type::MOUSE_CLICK_UP:
                return UIEventResult::INTERCEPT;
            break;

            case UIEvent::Type::MOUSE_CLICK_FULL:
                if (event.mouse.button == MouseButton::LEFT && !IsBlocked())
                {
                    SetOn(!GetOn());
                }
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
