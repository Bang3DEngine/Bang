#include "Bang/UIButton.h"

#include "Bang/Input.h"
#include "Bang/Paths.h"
#include "Bang/Cursor.h"
#include "Bang/UILabel.h"
#include "Bang/Texture2D.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/UIRectMask.h"
#include "Bang/RectTransform.h"
#include "Bang/TextureFactory.h"
#include "Bang/UITextRenderer.h"
#include "Bang/MaterialFactory.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"

USING_NAMESPACE_BANG

UIButton::UIButton()
{
    CONSTRUCT_CLASS_ID(UIButton)
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
    return SCAST<UIButton*>(
    UIButtonBase::CreateInto([](GameObject *go)
    {
        return go->AddComponent<UIButton>();
    },
    gameObject));
}
