#include "Bang/UIComboBox.h"

#include <functional>

#include "Bang/Input.h"
#include "Bang/UIList.h"
#include "Bang/UILabel.h"
#include "Bang/Material.h"
#include "Bang/UICanvas.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/GameObject.h"
#include "Bang/TextureFactory.h"
#include "Bang/UIFocusable.h"
#include "Bang/RectTransform.h"
#include "Bang/UITextRenderer.h"
#include "Bang/MaterialFactory.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutIgnorer.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIContentSizeFitter.h"

USING_NAMESPACE_BANG

UIComboBox::UIComboBox()
{
}

UIComboBox::~UIComboBox()
{
}

void UIComboBox::OnUpdate()
{
    Component::OnUpdate();

    /*
    if (Input::GetMouseButtonDown(MouseButton::LEFT))
    {
        if (IsListBeingShown() && !m_justStartedToShowList)
        {
            if (!UICanvas::GetActive(this)->IsMouseOver(GetList(), true))
            {
                HideList();
            }
        }
    }
    m_justStartedToShowList = false;
    */
}

void UIComboBox::AddItem(const String &label, int value)
{
    GameObject *textGo = GameObjectFactory::CreateUIGameObject();
    UITextRenderer *textRend = textGo->AddComponent<UITextRenderer>();
    textRend->SetContent(label);
    textRend->SetTextSize(12);
    textRend->SetVerticalAlign(VerticalAlignment::CENTER);
    textRend->SetHorizontalAlign(HorizontalAlignment::RIGHT);

    GetList()->AddItem(textGo);
    m_indexToValue.PushBack( value );
    m_indexToLabel.PushBack( label );

    if (m_selectedIndex < 0) { SetSelectionByIndex(0); }
}

void UIComboBox::SetSelectionByIndex(int index)
{
    ASSERT(index >= 0 && index < int( m_indexToValue.Size() ) );

    if (m_selectedIndex != index)
    {
        m_selectedIndex = index;
        p_currentItemText->SetContent( m_indexToLabel[index] );
        EventEmitter<IEventsValueChanged>::PropagateToListeners(
                    &IEventsValueChanged::OnValueChanged, this);
    }
}

void UIComboBox::SetSelectionByValue(int value)
{
    int indexOfValue = 0;
    if (m_indexToValue.Contains(value))
    {
        indexOfValue = m_indexToValue.IndexOf(value);
    }
    SetSelectionByIndex(indexOfValue);
}

void UIComboBox::ShowList()
{
    if (!IsListBeingShown())
    {
        GetList()->GetGameObject()->SetEnabled(true);
        m_justStartedToShowList = true;
    }
}

bool UIComboBox::IsListBeingShown() const
{
    return GetList()->GetGameObject()->IsEnabled();
}

void UIComboBox::HideList()
{
    GetList()->GetGameObject()->SetEnabled(false);
}

int UIComboBox::GetSelectedValue() const
{
    if (GetSelectedIndex() >= 0 && GetSelectedIndex() < int( m_indexToValue.Size() ))
    {
        return m_indexToValue[ GetSelectedIndex() ];
    }
    return 0;
}

int UIComboBox::GetSelectedIndex() const
{
    return m_selectedIndex;
}

String UIComboBox::GetSelectedLabel() const
{
    if (GetSelectedIndex() >= 0 && GetSelectedIndex() < int( m_indexToValue.Size() ))
    {
        return m_indexToLabel[ GetSelectedIndex() ];
    }
    return "";
}

bool UIComboBox::HasFocus() const
{
    return GetList()->GetGameObject()->IsEnabled();
}

UIComboBox *UIComboBox::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIComboBox *comboBox = go->AddComponent<UIComboBox>();

    UIHorizontalLayout *hl = go->AddComponent<UIHorizontalLayout>();
    hl->SetChildrenVerticalStretch(Stretch::NONE);
    hl->SetChildrenVerticalAlignment(VerticalAlignment::CENTER);
    hl->SetPaddings(6);
    hl->SetPaddingLeft(10);
    hl->SetPaddingRight(6);
    hl->SetSpacing(8);

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->AddEventCallback([comboBox](IFocusable*, const UIEvent &event)
    {
        if (event.type == UIEvent::Type::MOUSE_CLICK)
        {
            if (event.click.type == ClickType::FULL)
            {
                if (comboBox->IsListBeingShown())
                {
                    comboBox->HideList();
                }
                else
                {
                    comboBox->ShowList();
                }
                return UIEventResult::INTERCEPT;
            }
        }
        return UIEventResult::IGNORE;
    });
    focusable->SetCursorType(Cursor::Type::HAND);

    GameObject *currentItemTextGo = GameObjectFactory::CreateUIGameObject();
    UITextRenderer *currentItemText = currentItemTextGo->AddComponent<UITextRenderer>();
    currentItemText->SetContent("Current");
    currentItemText->SetTextSize(12);
    currentItemText->SetHorizontalAlign(HorizontalAlignment::RIGHT);

    UIImageRenderer *bg = go->AddComponent<UIImageRenderer>();
    bg->SetImageTexture( TextureFactory::Get9SliceRoundRectTexture().Get() );
    bg->SetMode(UIImageRenderer::Mode::SLICE_9);
    bg->SetTint(Color::White);

    UIImageRenderer *downArrowIcon = GameObjectFactory::CreateUIImage();
    downArrowIcon->SetImageTexture( TextureFactory::GetDownArrowIcon().Get() );
    downArrowIcon->SetTint( Color::Black );

    GameObject *downArrowIconGo = downArrowIcon->GetGameObject();
    UILayoutElement *downArrowLE = downArrowIconGo->AddComponent<UILayoutElement>();
    downArrowLE->SetMinSize( Vector2i(8) );
    downArrowLE->SetFlexibleSize( Vector2::Zero );

    UIList *list = GameObjectFactory::CreateUIList(false);
    GameObject *listGo = list->GetGameObject();
    list->SetWideSelectionMode(true);
    list->GetDirLayout()->SetPaddings(3);
    list->GetDirLayout()->SetPaddingLeft(20);
    list->GetDirLayout()->SetPaddingRight(2);
    list->SetSelectionCallback([comboBox](GameObject *item, UIList::Action action)
    {
        comboBox->OnListSelectionCallback(item, action);
    });

    UIImageRenderer *listBG = listGo->AddComponent<UIImageRenderer>();
    listBG->SetImageTexture( TextureFactory::Get9SliceRoundRectTexture().Get() );
    listBG->SetMode(UIImageRenderer::Mode::SLICE_9);
    listBG->SetTint(Color::White);
    UIFocusable *listFocusable = listBG->GetGameObject()->GetComponent<UIFocusable>();
    listFocusable->SetCursorType(Cursor::Type::HAND);

    listGo->AddComponent<UILayoutIgnorer>();
    RectTransform *contRT = listGo->GetRectTransform();
    contRT->SetAnchors( Vector2(1, -1) );
    contRT->SetPivotPosition( Vector2(1, 1) );
    contRT->TranslateLocal(Vector3(0.0f, 0.0f, -0.0001f));

    UIContentSizeFitter *csf = listGo->AddComponent<UIContentSizeFitter>();
    csf->SetHorizontalSizeType(LayoutSizeType::PREFERRED);
    csf->SetVerticalSizeType(LayoutSizeType::PREFERRED);

    currentItemTextGo->SetParent(go);
    downArrowIconGo->SetParent(go);
    listGo->SetParent(go);

    comboBox->p_currentItemText = currentItemText;
    comboBox->p_list = list;

    comboBox->HideList();

    return comboBox;
}

void UIComboBox::OnListSelectionCallback(GameObject *item, UIList::Action action)
{
    int indexOfItem = p_list->GetItems().IndexOf(item);
    ASSERT(indexOfItem >= 0);

    switch (action)
    {
        case UIList::Action::CLICKED_LEFT:
        case UIList::Action::CLICKED_RIGHT:
        case UIList::Action::PRESSED:
            SetSelectionByIndex(indexOfItem);
            HideList();
            break;

        default: break;
    }
}

UIList *UIComboBox::GetList() const
{
    return p_list;
}
