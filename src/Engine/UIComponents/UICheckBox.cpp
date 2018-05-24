#include "Bang/UICheckBox.h"

#include "Bang/Material.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/TextureFactory.h"
#include "Bang/UIFocusable.h"
#include "Bang/RectTransform.h"
#include "Bang/MaterialFactory.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"

USING_NAMESPACE_BANG

Color UICheckBox::IdleColor = Color(1,1,1,1);
Color UICheckBox::OverColor = Color(0.8, 0.95,  1,  1);

UICheckBox::UICheckBox()
{
}

UICheckBox::~UICheckBox()
{
}

void UICheckBox::OnUpdate()
{
    Component::OnUpdate();

    if (GetFocusable()->IsMouseOver())
    {
        GetBackgroundImage()->SetTint( UICheckBox::OverColor );
    }
    else
    {
        GetBackgroundImage()->SetTint( UICheckBox::IdleColor );
    }
}

void UICheckBox::SetSize(int size)
{
    if (size != GetSize())
    {
        constexpr int CheckBoxPaddings = 2;
        m_size = size;

        GetBackgroundImage()->GetGameObject()->GetRectTransform()->
                        SetMargins( -(GetSize()/2 + CheckBoxPaddings));

        GetTickImage()->GetGameObject()->GetRectTransform()->
                        SetMargins( -(GetSize()/2) );

        GetLayoutElement()->SetMinSize( Vector2i(GetSize() + CheckBoxPaddings) );
    }
}

void UICheckBox::SetChecked(bool checked)
{
    if (checked != IsChecked())
    {
        m_isChecked = checked;

        GetTickImage()->SetEnabled( IsChecked() );
        EventEmitter<IValueChangedListener>::PropagateToListeners(
                    &IValueChangedListener::OnValueChanged, this);
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

IFocusable *UICheckBox::GetFocusable() const
{
    return p_focusable;
}

UICheckBox *UICheckBox::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UICheckBox *checkBox = go->AddComponent<UICheckBox>();

    go->AddComponent<UIHorizontalLayout>();

    UILayoutElement* goLE = go->AddComponent<UILayoutElement>();
    goLE->SetFlexibleSize( Vector2::Zero );

    GameObject *checkBgImgGo = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *checkBgImg = checkBgImgGo->AddComponent<UIImageRenderer>();
    checkBgImg->SetImageTexture( TextureFactory::Get9SliceRoundRectTexture().Get() );
    checkBgImg->SetMode(UIImageRenderer::Mode::SLICE_9);
    checkBgImg->SetTint(UICheckBox::IdleColor);
    checkBgImgGo->GetRectTransform()->SetAnchors(Vector2::Zero);

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->AddClickedCallback([checkBox](IFocusable*, ClickType clickType)
    {
        if (clickType == ClickType::FULL)
        {
            checkBox->SetChecked( !checkBox->IsChecked() );
        }
    });
    focusable->SetCursorType(Cursor::Type::HAND);

    GameObject *tickImgGo = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *tickImg = tickImgGo->AddComponent<UIImageRenderer>();
    tickImg->SetImageTexture( TextureFactory::GetCheckIcon().Get() );
    tickImg->SetTint(Color::Black);
    tickImgGo->GetRectTransform()->SetAnchors(Vector2::Zero);

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
