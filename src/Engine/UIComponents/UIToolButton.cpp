#include "Bang/UIToolButton.h"

#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/UIFocusable.h"

using namespace Bang;

UIToolButton::UIToolButton()
{
    SET_INSTANCE_CLASS_ID(UIToolButton)
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
            UpdateAspect();
        }
    }
}

bool UIToolButton::GetOn() const
{
    return m_on;
}

void UIToolButton::Click()
{
    SetOn(!GetOn());
    CallClickCallback();
}

void UIToolButton::UpdateAspect()
{
    if (!IsBlocked())
    {
        if (GetOn())
        {
            ChangeAspectToPressed();
        }
        else
        {
            if (GetFocusable()->IsBeingPressed())
            {
                ChangeAspectToPressed();
            }
            else
            {
                if (GetFocusable()->IsMouseOver())
                {
                    ChangeAspectToOver();
                }
                else
                {
                    ChangeAspectToIdle();
                }
            }
        }
    }
    else
    {
        ChangeAspectToBlocked();
    }
}

UIToolButton *UIToolButton::CreateInto(GameObject *gameObject)
{
    return SCAST<UIToolButton *>(UIButtonBase::CreateInto(
        [](GameObject *go) { return go->AddComponent<UIToolButton>(); },
        gameObject));
}
