#include "Bang/UICheckBox.h"

#include "Bang/ClassDB.h"
#include "Bang/Cursor.h"
#include "Bang/DPtr.tcc"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/IEventsValueChanged.h"
#include "Bang/Key.h"
#include "Bang/RectTransform.h"
#include "Bang/TextureFactory.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UITheme.h"

using namespace Bang;

UICheckBox::UICheckBox()
{
    SET_INSTANCE_CLASS_ID(UICheckBox)
}

UICheckBox::~UICheckBox()
{
}

void UICheckBox::OnUpdate()
{
    Component::OnUpdate();
}

void UICheckBox::SetSize(int size)
{
    if (size != GetSize())
    {
        constexpr int CheckBoxPaddings = 2;
        m_size = size;

        GetBackgroundImage()->GetGameObject()->GetRectTransform()->SetMargins(
            -(GetSize() / 2 + CheckBoxPaddings));

        GetTickImage()->GetGameObject()->GetRectTransform()->SetMargins(
            -(GetSize() / 2));

        GetLayoutElement()->SetMinSize(Vector2i(GetSize() + CheckBoxPaddings));
    }
}

void UICheckBox::SetChecked(bool checked)
{
    if (checked != IsChecked())
    {
        m_isChecked = checked;

        GetTickImage()->SetEnabled(IsChecked());
        EventEmitter<IEventsValueChanged>::PropagateToListeners(
            &IEventsValueChanged::OnValueChanged, this);
    }
}

bool UICheckBox::IsChecked() const
{
    return m_isChecked;
}

int UICheckBox::GetSize() const
{
    return m_size;
}

UIImageRenderer *UICheckBox::GetTickImage() const
{
    return p_tickImage;
}

UIImageRenderer *UICheckBox::GetBackgroundImage() const
{
    return p_checkBgImage;
}

UILayoutElement *UICheckBox::GetLayoutElement() const
{
    return p_layoutElement;
}

UIFocusable *UICheckBox::GetFocusable() const
{
    return p_focusable;
}

UIEventResult UICheckBox::OnUIEvent(UIFocusable *, const UIEvent &event)
{
    switch (event.type)
    {
        case UIEvent::Type::FOCUS_TAKEN:
            GameObjectFactory::MakeBorderFocused(p_border);
            return UIEventResult::INTERCEPT;
            break;

        case UIEvent::Type::FOCUS_LOST:
            GameObjectFactory::MakeBorderNotFocused(p_border);
            return UIEventResult::INTERCEPT;
            break;

        case UIEvent::Type::MOUSE_ENTER:
            GetBackgroundImage()->SetTint(UITheme::GetOverColor());
            break;

        case UIEvent::Type::MOUSE_EXIT:
            GetBackgroundImage()->SetTint(UITheme::GetInputsBackgroundColor());
            break;

        case UIEvent::Type::MOUSE_CLICK_FULL:
            SetChecked(!IsChecked());
            return UIEventResult::INTERCEPT;
            break;

        case UIEvent::Type::KEY_DOWN:
            switch (event.key.key)
            {
                case Key::SPACE:
                case Key::ENTER:
                    SetChecked(!IsChecked());
                    return UIEventResult::INTERCEPT;
                    break;

                default: break;
            }
            break;

        default: break;
    }
    return UIEventResult::IGNORE;
}

UICheckBox *UICheckBox::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UICheckBox *checkBox = go->AddComponent<UICheckBox>();

    go->AddComponent<UIHorizontalLayout>();

    UILayoutElement *goLE = go->AddComponent<UILayoutElement>();
    goLE->SetFlexibleSize(Vector2::Zero());

    GameObject *checkBgImgGo = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *checkBgImg = checkBgImgGo->AddComponent<UIImageRenderer>();
    // checkBgImg->SetImageTexture(
    // TextureFactory::Get9SliceRoundRectTexture().Get() );
    // checkBgImg->SetMode(UIImageRenderer::Mode::SLICE_9);
    checkBgImg->SetTint(UITheme::GetCheckBoxBackgroundColor());
    checkBgImgGo->GetRectTransform()->SetAnchors(Vector2::Zero());

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->EventEmitter<IEventsFocus>::RegisterListener(checkBox);
    focusable->SetCursorType(Cursor::Type::HAND);
    focusable->SetConsiderForTabbing(true);

    GameObject *tickImgGo = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *tickImg = tickImgGo->AddComponent<UIImageRenderer>();
    tickImg->SetImageTexture(TextureFactory::GetCheckIcon());
    tickImg->SetTint(UITheme::GetTickColor());
    tickImgGo->GetRectTransform()->SetAnchors(Vector2::Zero());

    checkBox->p_border = GameObjectFactory::AddInnerBorder(checkBgImgGo);
    // GameObjectFactory::AddInnerShadow(checkBgImgGo, Vector2i(3));

    checkBox->p_focusable = focusable;
    checkBox->p_tickImage = tickImg;
    checkBox->p_checkBgImage = checkBgImg;
    checkBox->p_layoutElement = goLE;

    checkBgImgGo->SetParent(go);
    tickImgGo->SetParent(go);

    checkBox->SetSize(14);
    checkBox->SetChecked(true);

    return checkBox;
}
