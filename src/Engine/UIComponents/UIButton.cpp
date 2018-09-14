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
}

UIButton::~UIButton()
{

}

UIEventResult
UIButton::OnFocusEvent(EventEmitter<IEventsFocus> *focusable,
                       const UIEvent &event)
{
    BANG_UNUSED(focusable);

    if (!IsBlocked())
    {
        switch (event.type)
        {
            case UIEvent::Type::MOUSE_CLICK_DOWN:
                if (event.mouse.button == MouseButton::LEFT && !IsBlocked())
                {
                    OnMouseEnter();
                    return UIEventResult::INTERCEPT;
                }
            break;
            case UIEvent::Type::MOUSE_CLICK_FULL:
                if (event.mouse.button == MouseButton::LEFT && !IsBlocked())
                {
                    OnMouseEnter();
                    for (auto clickedCallback : m_clickedCallbacks)
                    {
                        clickedCallback();
                    }
                    return UIEventResult::INTERCEPT;
                }
            break;

            case UIEvent::Type::STARTED_BEING_PRESSED:
            case UIEvent::Type::FINISHED_BEING_PRESSED:
            case UIEvent::Type::MOUSE_CLICK_UP:
            case UIEvent::Type::MOUSE_ENTER:
            case UIEvent::Type::MOUSE_EXIT:
                if (GetFocusable()->IsMouseOver())
                {
                    OnMouseEnter();
                }
                else
                {
                    OnMouseExit();
                }
                return UIEventResult::INTERCEPT;
            break;

            default: break;
        }
    }
    return UIEventResult::IGNORE;
}

void UIButton::OnMouseEnter()
{
    if (GetFocusable()->IsBeingPressed())
    {
        ChangeAspectToPressed();
    }
    else
    {
        ChangeAspectToOver();
    }
}

void UIButton::OnMouseExit()
{
    if (GetFocusable()->IsBeingPressed())
    {
        ChangeAspectToPressed();
    }
    else
    {
        ChangeAspectToIdle();
    }
}

void UIButton::OnStart()
{
    Component::OnStart();
    GetFocusable()->AddEventCallback([this](EventEmitter<IEventsFocus> *focusable,
                                            const UIEvent &event)
    {
        return OnFocusEvent(focusable, event);
    });
}

void UIButton::Click()
{
    UIEvent event;
    event.type = UIEvent::Type::MOUSE_CLICK_FULL;
    event.mouse.button = MouseButton::LEFT;
    GetFocusable()->ProcessEvent(event);
}

void UIButton::SetBlocked(bool blocked)
{
    if (blocked != IsBlocked())
    {
        m_isBlocked = blocked;

        GetFocusable()->EventEmitter<IEventsFocus>::SetEmitEvents( !IsBlocked() );
        if (!IsBlocked())
        {
            if (GetFocusable()->IsMouseOver())
            {
                OnMouseEnter();
            }
            else
            {
                OnMouseExit();
            }
        }
        else
        {
            ChangeAspectToBlocked();
        }
    }
}

void UIButton::SetIconSize(const Vector2i &size)
{
    UILayoutElement *le = GetIcon()->GetGameObject()->
                          GetComponent<UILayoutElement>();
    le->SetPreferredSize(size);
}

void UIButton::SetIconTexture(Texture2D *texture)
{
    GetIcon()->SetImageTexture(texture);
}

void UIButton::SetIconSpacingWithText(int spacingWithText)
{
    GetGameObject()->GetComponent<UIDirLayout>()->SetSpacing(spacingWithText);
}

void UIButton::SetIcon(Texture2D *texture, const Vector2i &size,
                       int spacingWithText)
{
    SetIconTexture(texture);
    SetIconSize(size);
    SetIconSpacingWithText(spacingWithText);
}

void UIButton::AddClickedCallback(UIButton::ClickedCallback clickedCallback)
{
    m_clickedCallbacks.PushBack(clickedCallback);
}

bool UIButton::IsBlocked() const
{
    return m_isBlocked;
}

UIImageRenderer *UIButton::GetBorder() const
{
    return p_border;
}
UIImageRenderer *UIButton::GetIcon() const
{
    return p_icon;
}
UITextRenderer *UIButton::GetText() const
{
    return p_text;
}
UIImageRenderer *UIButton::GetBackground() const
{
    return p_background;
}
UIFocusable *UIButton::GetFocusable() const
{
    return p_focusable;
}
UILayoutElement *UIButton::GetLayoutElement() const
{
    return p_layoutElement;
}

UIDirLayout *UIButton::GetDirLayout() const
{
    return GetGameObject()->GetComponent<UIDirLayout>();
}

void UIButton::ChangeAspectToIdle()
{
    GetBackground()->SetImageTexture( TextureFactory::GetButtonIdle() );
    GetBackground()->SetTint( Color::White.WithValue(1.2f) );
    GetText()->SetTextColor(Color::Black);
    GetFocusable()->SetCursorType( Cursor::Type::HAND );
}

void UIButton::ChangeAspectToOver()
{
    GetBackground()->SetImageTexture( TextureFactory::GetButtonIdle() );
    GetBackground()->SetTint( Color::LightBlue );
    GetText()->SetTextColor(Color::Black);
    GetFocusable()->SetCursorType( Cursor::Type::HAND );
}

void UIButton::ChangeAspectToPressed()
{
    GetBackground()->SetImageTexture( TextureFactory::GetButtonDown() );
    GetBackground()->SetTint( Color::LightBlue );
    GetText()->SetTextColor(Color::Black);
    GetFocusable()->SetCursorType( Cursor::Type::HAND );
}

void UIButton::ChangeAspectToBlocked()
{
    GetBackground()->SetImageTexture( TextureFactory::GetButtonIdle() );
    GetBackground()->SetTint( Color::White.WithValue(0.75f) );
    GetText()->SetTextColor(Color::DarkGray);
    GetFocusable()->SetCursorType( Cursor::Type::NO );
}

UIButton* UIButton::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIButton *button = go->AddComponent<UIButton>();

    UIHorizontalLayout *hl = go->AddComponent<UIHorizontalLayout>();
    hl->SetSpacing(0);

    UILayoutElement *le = go->AddComponent<UILayoutElement>();
    le->SetFlexibleSize( Vector2::Zero );
    le->SetLayoutPriority(1);

    UIImageRenderer *bgImg = go->AddComponent<UIImageRenderer>();
    // bgImg->SetImageTexture( TextureFactory::Get9SliceRoundRectTexture().Get() );
    bgImg->SetMode(UIImageRenderer::Mode::SLICE_9_INV_UVY);

    button->p_border = GameObjectFactory::AddInnerBorder(go, Vector2i(1));

    UIFocusable *btn = go->AddComponent<UIFocusable>();

    UILabel *label = GameObjectFactory::CreateUILabel();
    label->GetText()->SetTextColor(Color::Black);
    label->GetFocusable()->SetFocusEnabled(false);
    label->GetMask()->SetMasking(false);

    UIImageRenderer *icon = GameObjectFactory::CreateUIImage();

    GameObject *iconGo = icon->GetGameObject();
    UILayoutElement *iconLE = iconGo->AddComponent<UILayoutElement>();
    iconLE->SetFlexibleSize(Vector2::Zero);

    button->p_icon = icon;
    button->p_background = bgImg;
    button->p_focusable = btn;
    button->p_text = label->GetText();
    button->p_layoutElement = le;

    icon->GetGameObject()->SetParent(go);
    label->GetGameObject()->SetParent(go);

    button->GetText()->SetContent("");
    button->SetIcon(nullptr, Vector2i::Zero, 0);

    button->ChangeAspectToIdle();

    return button;
}
