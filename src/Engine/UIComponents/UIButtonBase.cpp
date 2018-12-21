#include "Bang/UIButtonBase.h"

#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/Cursor.h"
#include "Bang/DPtr.tcc"
#include "Bang/EventEmitter.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/Key.h"
#include "Bang/MouseButton.h"
#include "Bang/RectTransform.h"
#include "Bang/UIDirLayout.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILabel.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UIRectMask.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UITheme.h"

using namespace Bang;

UIButtonBase::UIButtonBase()
{
    SET_INSTANCE_CLASS_ID(UIButtonBase)
}

UIButtonBase::~UIButtonBase()
{
}

void UIButtonBase::ClickBase()
{
    if (!IsBlocked())
    {
        Click();
    }
    UpdateAspect();
}

void UIButtonBase::SetBlocked(bool blocked)
{
    if (blocked != IsBlocked())
    {
        m_isBlocked = blocked;
        GetFocusable()->SetConsiderForTabbing(!IsBlocked());

        UpdateAspect();
    }
}

void UIButtonBase::SetIconSize(const Vector2i &size)
{
    UILayoutElement *le =
        GetIcon()->GetGameObject()->GetComponent<UILayoutElement>();
    le->SetMinSize(size);
    le->SetPreferredSize(size);
}

void UIButtonBase::SetIconTexture(Texture2D *texture)
{
    GetIcon()->SetImageTexture(texture);
}

void UIButtonBase::SetIconSpacingWithText(int spacingWithText)
{
    GetGameObject()->GetComponent<UIDirLayout>()->SetSpacing(spacingWithText);
}

void UIButtonBase::SetIcon(Texture2D *texture,
                           const Vector2i &size,
                           int spacingWithText)
{
    SetIconTexture(texture);
    SetIconSize(size);
    SetIconSpacingWithText(spacingWithText);
}

void UIButtonBase::AddClickedCallback(std::function<void()> clickedCallback)
{
    m_clickedCallbacks.PushBack(clickedCallback);
}

bool UIButtonBase::IsBlocked() const
{
    return m_isBlocked;
}

UIImageRenderer *UIButtonBase::GetBorder() const
{
    return p_border;
}
UIImageRenderer *UIButtonBase::GetIcon() const
{
    return p_icon;
}
UITextRenderer *UIButtonBase::GetText() const
{
    return p_text;
}
UIImageRenderer *UIButtonBase::GetBackground() const
{
    return p_background;
}
UIFocusable *UIButtonBase::GetFocusable() const
{
    return p_focusable;
}
UILayoutElement *UIButtonBase::GetLayoutElement() const
{
    return p_layoutElement;
}

UIDirLayout *UIButtonBase::GetDirLayout() const
{
    return GetGameObject()->GetComponent<UIDirLayout>();
}

void UIButtonBase::ChangeAspectToIdle()
{
    if (GetBackground() && GetText())
    {
        GetBackground()->SetTint(Color::White().WithValue(1.2f));
        GetText()->SetTextColor(Color::Black());
        GetFocusable()->SetCursorType(Cursor::Type::HAND);
    }
}

void UIButtonBase::ChangeAspectToOver()
{
    if (GetBackground() && GetText())
    {
        GetBackground()->SetTint(UITheme::GetOverColor());
        GetText()->SetTextColor(Color::Black());
        GetFocusable()->SetCursorType(Cursor::Type::HAND);
    }
}

void UIButtonBase::ChangeAspectToPressed()
{
    if (GetBackground() && GetText())
    {
        GetBackground()->SetTint(UITheme::GetSelectedColor());
        GetText()->SetTextColor(Color::Black());
        GetFocusable()->SetCursorType(Cursor::Type::HAND);
    }
}

void UIButtonBase::ChangeAspectToBlocked()
{
    if (GetBackground() && GetText())
    {
        GetBackground()->SetTint(UITheme::GetInputTextBlockedBackgroundColor());
        GetText()->SetTextColor(Color::DarkGray());
        GetFocusable()->SetCursorType(Cursor::Type::NO);
    }
}

UIEventResult UIButtonBase::OnUIEvent(UIFocusable *, const UIEvent &event)
{
    switch (event.type)
    {
        case UIEvent::Type::FOCUS_TAKEN:
            if (!IsBlocked())
            {
                GameObjectFactory::MakeBorderFocused(p_border);
            }
            return UIEventResult::INTERCEPT;
            break;

        case UIEvent::Type::FOCUS_LOST:
            GameObjectFactory::MakeBorderNotFocused(p_border);
            return UIEventResult::INTERCEPT;
            break;

        default: break;
    }

    if (!IsBlocked())
    {
        switch (event.type)
        {
            case UIEvent::Type::MOUSE_CLICK_DOWN:
                if (event.mouse.button == MouseButton::LEFT && !IsBlocked())
                {
                    UpdateAspect();
                    return UIEventResult::INTERCEPT;
                }
                break;

            case UIEvent::Type::MOUSE_CLICK_FULL:
                if (event.mouse.button == MouseButton::LEFT)
                {
                    ClickBase();
                    return UIEventResult::INTERCEPT;
                }
                break;

            case UIEvent::Type::STARTED_BEING_PRESSED:
            case UIEvent::Type::FINISHED_BEING_PRESSED:
            case UIEvent::Type::MOUSE_CLICK_UP:
            case UIEvent::Type::MOUSE_ENTER:
            case UIEvent::Type::MOUSE_EXIT:
                UpdateAspect();
                return UIEventResult::INTERCEPT;
                break;

            case UIEvent::Type::KEY_DOWN:
                switch (event.key.key)
                {
                    case Key::SPACE:
                    case Key::ENTER:
                        ClickBase();
                        return UIEventResult::INTERCEPT;
                        break;

                    default: break;
                }
                break;

            default: break;
        }
    }
    return UIEventResult::IGNORE;
}

UIButtonBase *UIButtonBase::CreateInto(
    std::function<UIButtonBase *(GameObject *)> createBtnFunc,
    GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIButtonBase *button = createBtnFunc(go);

    go->AddComponent<UIRectMask>();

    UIHorizontalLayout *hl = go->AddComponent<UIHorizontalLayout>();
    hl->SetSpacing(0);

    UILayoutElement *le = go->AddComponent<UILayoutElement>();
    le->SetFlexibleSize(Vector2::Zero());
    le->SetLayoutPriority(1);

    UIImageRenderer *bgImg = go->AddComponent<UIImageRenderer>();

    button->p_border = GameObjectFactory::AddInnerBorder(go);

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->SetConsiderForTabbing(true);
    focusable->EventEmitter<IEventsFocus>::RegisterListener(button);

    UILabel *label = GameObjectFactory::CreateUILabel();
    label->GetText()->SetTextColor(Color::Black());
    label->GetFocusable()->SetEnabled(false);
    label->GetMask()->SetMasking(false);

    UIImageRenderer *icon = GameObjectFactory::CreateUIImage();
    GameObject *iconGo = icon->GetGameObject();
    UILayoutElement *iconLE = iconGo->AddComponent<UILayoutElement>();
    iconLE->SetFlexibleSize(Vector2::Zero());

    button->p_icon = icon;
    button->p_background = bgImg;
    button->p_focusable = focusable;
    button->p_text = label->GetText();
    button->p_layoutElement = le;

    icon->GetGameObject()->SetParent(go);
    label->GetGameObject()->SetParent(go);

    button->GetText()->SetContent("");
    button->SetIcon(nullptr, Vector2i::Zero(), 0);

    button->UpdateAspect();
    return button;
}

void UIButtonBase::CallClickCallback()
{
    for (auto clickedCallback : m_clickedCallbacks)
    {
        clickedCallback();
    }
}
