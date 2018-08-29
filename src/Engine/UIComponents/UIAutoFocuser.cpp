#include "Bang/UIAutoFocuser.h"

#include "Bang/UICanvas.h"
#include "Bang/IFocusable.h"

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
        Component::DestroyDelayed(this);
    }
}

void UIAutoFocuser::SetFocusableToAutoFocus(IFocusable *focusable)
{
    p_focusableToAutoFocus = focusable;
}

IFocusable *UIAutoFocuser::GetFocusableToAutoFocus() const
{
    return p_focusableToAutoFocus;
}

