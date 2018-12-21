#include "Bang/UIButton.h"

#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/UIFocusable.h"

using namespace Bang;

UIButton::UIButton()
{
    SET_INSTANCE_CLASS_ID(UIButton)
}

UIButton::~UIButton()
{
}

void UIButton::Click()
{
    ChangeAspectToPressed();
    CallClickCallback();
    UpdateAspect();
}

void UIButton::UpdateAspect()
{
    if (!IsBlocked())
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
    else
    {
        ChangeAspectToBlocked();
    }
}

UIButton *UIButton::CreateInto(GameObject *gameObject)
{
    return SCAST<UIButton *>(UIButtonBase::CreateInto(
        [](GameObject *go) { return go->AddComponent<UIButton>(); },
        gameObject));
}
