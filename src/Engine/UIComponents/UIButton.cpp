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

const Color UIButton::IdleColor    = Color(1,1,1,1);
const Color UIButton::OverColor    = Color(0.8, 0.95,  1,  1);
const Color UIButton::PressedColor = Color(0.8, 0.95,  1,  1)  * 0.8f;
const Color UIButton::BlockedColor = Color(0.8,  0.8, 0.8,  1) * 0.8f;

UIButton::UIButton()
{
}

UIButton::~UIButton()
{

}

void UIButton::OnStart()
{
    Component::OnStart();
    GetFocusable()->EventEmitter<IEventsFocus>::RegisterListener(this);
}

void UIButton::Click(ClickType clickType)
{
    GetFocusable()->Click(clickType);
}

void UIButton::SetBlocked(bool blocked)
{
    if (blocked != IsBlocked())
    {
        m_isBlocked = blocked;

        GetFocusable()->IFocusable::SetEmitEvents( !IsBlocked() );
        if (!IsBlocked())
        {
            GetText()->SetTextColor(Color::Black);
            if (GetFocusable()->IsMouseOver()) { OnMouseEnter(GetFocusable()); }
            else { OnMouseExit(GetFocusable()); }
            GetFocusable()->SetCursorType( Cursor::Type::HAND );
        }
        else
        {
            GetText()->SetTextColor(Color::DarkGray);
            GetBackground()->SetTint(UIButton::BlockedColor);
            GetFocusable()->SetCursorType( Cursor::Type::NO );
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

void UIButton::OnMouseEnter(EventEmitter<IEventsFocus> *focusable)
{
    IEventsFocus::OnMouseEnter(focusable);

    if (!IsBlocked())
    {
        GetBackground()->SetTint(GetFocusable()->IsBeingPressed() ?
                                     UIButton::PressedColor :
                                     UIButton::OverColor);
    }
}

void UIButton::OnMouseExit(EventEmitter<IEventsFocus> *focusable)
{
    IEventsFocus::OnMouseExit(focusable);

    if (!IsBlocked())
    {
        GetBackground()->SetTint(GetFocusable()->IsBeingPressed() ?
                                     UIButton::PressedColor :
                                     UIButton::IdleColor);
    }
}

void UIButton::OnStartedBeingPressed(EventEmitter<IEventsFocus> *focusable)
{
    IEventsFocus::OnStartedBeingPressed(focusable);

    if (!IsBlocked())
    {
        GetBackground()->SetTint(UIButton::PressedColor);
    }
}

void UIButton::OnStoppedBeingPressed(EventEmitter<IEventsFocus> *focusable)
{
    IEventsFocus::OnStoppedBeingPressed(focusable);

    if (!IsBlocked())
    {
        GetBackground()->SetTint(GetFocusable()->IsMouseOver() ?
                                     UIButton::OverColor :
                                     UIButton::IdleColor);
    }
}

void UIButton::OnClicked(EventEmitter<IEventsFocus> *focusable,
                         ClickType clickType)
{
    IEventsFocus::OnClicked(focusable, clickType);

    if (!IsBlocked())
    {
        if (clickType == ClickType::FULL)
        {
            for (auto clickedCallback : m_clickedCallbacks)
            {
                clickedCallback();
            }
        }
    }
}

bool UIButton::IsBlocked() const { return m_isBlocked; }
UIImageRenderer *UIButton::GetIcon() const { return p_icon; }
UITextRenderer *UIButton::GetText() const { return p_text; }
UIImageRenderer *UIButton::GetBackground() const { return p_background; }
UIFocusable *UIButton::GetFocusable() const { return p_focusable; }
UILayoutElement *UIButton::GetLayoutElement() const { return p_layoutElement; }

UIDirLayout *UIButton::GetDirLayout() const
{
    return GetGameObject()->GetComponent<UIDirLayout>();
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
    bgImg->SetImageTexture( TextureFactory::Get9SliceRoundRectTexture().Get() );
    bgImg->SetMode(UIImageRenderer::Mode::SLICE_9);
    bgImg->SetTint(Color::White);

    UIFocusable *btn = go->AddComponent<UIFocusable>();

    UILabel *label = GameObjectFactory::CreateUILabel();
    label->GetText()->SetTextColor(Color::Black);
    label->GetFocusable()->SetFocusEnabled(false);
    label->GetMask()->SetMasking(false);
    label->SetFocusEnabled(false);

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
    button->GetFocusable()->SetCursorType( Cursor::Type::HAND );

    return button;
}
