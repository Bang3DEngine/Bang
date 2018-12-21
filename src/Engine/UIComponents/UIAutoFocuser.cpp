#include "Bang/UIAutoFocuser.h"

#include "Bang/ClassDB.h"
#include "Bang/UICanvas.h"

using namespace Bang;

UIAutoFocuser::UIAutoFocuser()
{
    SET_INSTANCE_CLASS_ID(UIAutoFocuser);
}

void UIAutoFocuser::OnStart()
{
    Component::OnStart();
}

void UIAutoFocuser::OnUpdate()
{
    Component::OnUpdate();

    if (++m_frames == 2 && p_focusableToAutoFocus)
    {
        UICanvas *canvas = UICanvas::GetActive(GetGameObject());
        if (canvas)
        {
            canvas->SetFocus(p_focusableToAutoFocus);
        }
        Component::Destroy(this);
    }
}

void UIAutoFocuser::SetFocusableToAutoFocus(UIFocusable *focusable)
{
    p_focusableToAutoFocus = focusable;
}

UIFocusable *UIAutoFocuser::GetFocusableToAutoFocus() const
{
    return p_focusableToAutoFocus;
}
