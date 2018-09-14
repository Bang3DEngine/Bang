#include "Bang/UIAutoFocuser.h"

#include "Bang/UICanvas.h"
#include "Bang/UIFocusable.h"

USING_NAMESPACE_BANG

void UIAutoFocuser::OnStart()
{
    Component::OnStart();
}

void UIAutoFocuser::OnUpdate()
{
    Component::OnUpdate();

    if (++m_frames == 2 && p_focusableToAutoFocus)
    {
        UICanvas *canvas = UICanvas::GetActive( GetGameObject() );
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

