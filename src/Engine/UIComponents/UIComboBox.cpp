#include "Bang/UIComboBox.h"

#include "Bang/Alignment.h"
#include "Bang/Assert.h"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/Cursor.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/IEventsValueChanged.h"
#include "Bang/Input.h"
#include "Bang/Key.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/MouseButton.h"
#include "Bang/RectTransform.h"
#include "Bang/Stretch.h"
#include "Bang/TextureFactory.h"
#include "Bang/UICanvas.h"
#include "Bang/UIContentSizeFitter.h"
#include "Bang/UIDirLayout.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILabel.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutIgnorer.h"
#include "Bang/UIList.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UITheme.h"

using namespace Bang;

UIComboBox::UIComboBox()
{
    SET_INSTANCE_CLASS_ID(UIComboBox)
}

UIComboBox::~UIComboBox()
{
}

void UIComboBox::OnUpdate()
{
    Component::OnUpdate();

    m_timeWithListShown += Time::GetDeltaTime();
    m_listRecentlyToggled = false;

    // Close when click outside
    if (IsListBeingShown())
    {
        if (Input::GetMouseButtonDown(MouseButton::LEFT))
        {
            if (!UICanvas::GetActive(this)->IsMouseOver(this, true))
            {
                HideList();
            }
        }
    }
}

void UIComboBox::UpdateSelectedItemTopText()
{
    String content = "";
    if (GetMultiCheck())
    {
        if (GetSelectedIndices().Size() == 0)
        {
            content = "- None -";
        }
        else if (GetSelectedIndices().Size() == 1)
        {
            content = m_indexToLabel[GetSelectedIndex()];
        }
        else
        {
            content = "- Several -";
        }
    }
    else
    {
        if (GetSelectedIndex() >= 0)
        {
            content = m_indexToLabel[GetSelectedIndex()];
        }
    }
    p_selectedItemText->SetContent(content);
}

void UIComboBox::AddItem(const String &label, int value)
{
    GameObject *itemGo = GameObjectFactory::CreateUIGameObject();
    UIHorizontalLayout *hl = itemGo->AddComponent<UIHorizontalLayout>();
    BANG_UNUSED(hl);

    GameObject *checkGo = GameObjectFactory::CreateUIGameObject();

    UIImageRenderer *checkIcon = checkGo->AddComponent<UIImageRenderer>();
    checkIcon->SetImageTexture(TextureFactory::GetCheckIcon());
    checkIcon->SetTint(Color::White());
    UILayoutElement *checkIconLE = checkGo->AddComponent<UILayoutElement>();
    checkIconLE->SetMinWidth(16);
    checkIconLE->SetMinHeight(16);

    GameObject *textGo = GameObjectFactory::CreateUIGameObject();

    UITextRenderer *textRend = textGo->AddComponent<UITextRenderer>();
    textRend->SetContent(label);
    textRend->SetTextSize(12);
    textRend->SetVerticalAlign(VerticalAlignment::CENTER);
    textRend->SetHorizontalAlign(HorizontalAlignment::RIGHT);

    checkGo->SetEnabled(GetMultiCheck());
    checkGo->SetParent(itemGo);
    textGo->SetParent(itemGo);

    GetList()->AddItem(itemGo);
    m_indexToValue.PushBack(value);
    m_indexToLabel.PushBack(label);
    m_checkImgs.PushBack(checkIcon);

    if (m_selectedIndices.IsEmpty() && !GetMultiCheck())
    {
        SetSelectionByIndex(0);
    }
    UpdateSelectedItemTopText();
}

void UIComboBox::SetSelectionByIndex(int index, bool selected)
{
    ASSERT(index >= 0 && index < int(m_indexToValue.Size()));

    if (selected)
    {
        if (!m_selectedIndices.Contains(index))
        {
            if (GetMultiCheck())
            {
                m_checkImgs[index]->SetTint(UITheme::GetTickColor());
            }
            else
            {
                m_selectedIndices.Clear();
                GetList()->SetSelection(index);
            }

            m_selectedIndices.PushBack(index);

            UpdateSelectedItemTopText();
            EventEmitter<IEventsValueChanged>::PropagateToListeners(
                &IEventsValueChanged::OnValueChanged, this);
        }
    }
    else
    {
        if (m_selectedIndices.Contains(index))
        {
            m_selectedIndices.Remove(index);
            if (GetMultiCheck())
            {
                m_checkImgs[index]->SetTint(Color::White());
                if (m_selectedIndices.IsEmpty())
                {
                    GetList()->ClearSelection();
                }
            }
            else if (m_selectedIndices.IsEmpty())
            {
                m_selectedIndices.PushBack(0);
            }

            UpdateSelectedItemTopText();

            EventEmitter<IEventsValueChanged>::PropagateToListeners(
                &IEventsValueChanged::OnValueChanged, this);
        }
    }
}

void UIComboBox::SetSelectionByValue(int value, bool selected)
{
    uint indexOfValue = m_indexToValue.IndexOf(value);
    if (indexOfValue != SCAST<uint>(-1))
    {
        SetSelectionByIndex(indexOfValue, selected);
    }
}

void UIComboBox::SetSelectionByLabel(const String &label, bool selected)
{
    uint indexOfLabel = m_indexToLabel.IndexOf(label);
    if (indexOfLabel != SCAST<uint>(-1))
    {
        SetSelectionByIndex(indexOfLabel, selected);
    }
}

void UIComboBox::SetSelectionByIndex(int index)
{
    SetSelectionByIndex(index, true);
}

void UIComboBox::SetSelectionByValue(int value)
{
    SetSelectionByValue(value, true);
}

void UIComboBox::SetSelectionByLabel(const String &label)
{
    SetSelectionByLabel(label, true);
}

void UIComboBox::SetSelectionForFlag(int flagValue)
{
    for (int i = 0; i < GetNumItems(); ++i)
    {
        if (flagValue & (GetValues()[i]))
        {
            SetSelectionByIndex(i);
        }
    }
}

void UIComboBox::SetMultiCheck(bool multicheck)
{
    if (multicheck != GetMultiCheck())
    {
        m_multiCheck = multicheck;

        for (UIImageRenderer *checkImg : m_checkImgs)
        {
            checkImg->GetGameObject()->SetEnabled(GetMultiCheck());
        }

        if (!GetMultiCheck())
        {
            GetList()->ClearSelection();
        }

        p_focusable->SetConsiderForTabbing(!GetMultiCheck());
    }
}

void UIComboBox::ClearSelectionByIndex(int index)
{
    SetSelectionByIndex(index, false);
}

void UIComboBox::ClearSelectionByValue(int value)
{
    SetSelectionByValue(value, false);
}

void UIComboBox::ClearSelection()
{
    for (int index : m_selectedIndices)
    {
        EventEmitter<IEventsValueChanged>::SetEmitEvents(false);
        ClearSelectionByIndex(index);
        EventEmitter<IEventsValueChanged>::SetEmitEvents(true);
        EventEmitter<IEventsValueChanged>::PropagateToListeners(
            &IEventsValueChanged::OnValueChanged, this);
    }
}

void UIComboBox::ClearItems()
{
    HideList();
    while (!m_indexToLabel.IsEmpty())
    {
        const String label = m_indexToLabel.Back();
        RemoveItem(label);
    }
}

void UIComboBox::RemoveItem(const String &label)
{
    uint indexOfLabel = m_indexToLabel.IndexOf(label);
    if (indexOfLabel != SCAST<uint>(-1))
    {
        m_checkImgs.RemoveByIndex(indexOfLabel);
        m_indexToValue.RemoveByIndex(indexOfLabel);
        m_indexToLabel.RemoveByIndex(indexOfLabel);
        m_selectedIndices.Remove(indexOfLabel);
        p_list->RemoveItem(p_list->GetItem(indexOfLabel));
    }
}

void UIComboBox::ShowList()
{
    if (!IsListBeingShown())
    {
        m_timeWithListShown.SetNanos(0);
        SCAST<UIFocusable *>(GetList()->GetFocusable())->SetEnabled(true);
        GetList()->GetGameObject()->SetEnabled(true);
    }
}

bool UIComboBox::IsListBeingShown() const
{
    return GetList()->GetGameObject()->IsEnabledRecursively();
}

bool UIComboBox::IsSelectedByIndex(int index) const
{
    return m_selectedIndices.Contains(index);
}

void UIComboBox::HideList()
{
    if (IsListBeingShown())
    {
        m_timeWithListShown.SetNanos(0);
        SCAST<UIFocusable *>(GetList()->GetFocusable())->SetEnabled(false);
        GetList()->GetGameObject()->SetEnabled(false);
    }
}

int UIComboBox::GetNumItems() const
{
    return GetList()->GetNumItems();
}

bool UIComboBox::GetMultiCheck() const
{
    return m_multiCheck;
}

int UIComboBox::GetSelectedValue() const
{
    if (GetSelectedIndex() >= 0 &&
        GetSelectedIndex() < int(m_indexToValue.Size()))
    {
        return m_indexToValue[GetSelectedIndex()];
    }
    return 0;
}

int UIComboBox::GetSelectedIndex() const
{
    if (m_selectedIndices.Size() >= 1)
    {
        return m_selectedIndices[0];
    }
    return -1;
}

const Array<int> &UIComboBox::GetSelectedIndices() const
{
    return m_selectedIndices;
}

UIEventResult UIComboBox::OnUIEvent(UIFocusable *, const UIEvent &event)
{
    switch (event.type)
    {
        case UIEvent::Type::FOCUS_TAKEN:
            GameObjectFactory::MakeBorderFocused(p_border);
            break;

        case UIEvent::Type::FOCUS_LOST:
            GameObjectFactory::MakeBorderNotFocused(p_border);
            if (!GetMultiCheck())
            {
                HideList();
            }
            break;

        case UIEvent::Type::MOUSE_CLICK_UP:
        {
            if (!m_listRecentlyToggled)
            {
                if (IsListBeingShown() &&
                    GetList()->GetFocusable()->IsMouseOver() &&
                    m_timeWithListShown.GetSeconds() > 0.3)
                {
                    m_listRecentlyToggled = true;
                }
            }
            return UIEventResult::INTERCEPT;
        }
        break;

        case UIEvent::Type::MOUSE_CLICK_DOWN:
        {
            if (!m_listRecentlyToggled)
            {
                if (!IsListBeingShown())
                {
                    m_listRecentlyToggled = true;
                    ShowList();
                }
                else
                {
                    if (!GetList()->GetFocusable()->IsMouseOver())
                    {
                        HideList();
                    }
                }
            }
            return UIEventResult::INTERCEPT;
        }
        break;

        case UIEvent::Type::KEY_DOWN:
            switch (event.key.key)
            {
                case Key::SPACE:
                case Key::ENTER:
                    if (IsListBeingShown())
                    {
                        HideList();
                    }
                    else
                    {
                        ShowList();
                    }
                    return UIEventResult::INTERCEPT;
                    break;

                case Key::DOWN:
                case Key::UP:
                {
                    int numItems = GetNumItems();
                    int displacement = (event.key.key == Key::DOWN ? 1 : -1);
                    int oldIndex = GetSelectedIndex();
                    int newIndex =
                        (oldIndex + displacement + numItems) % numItems;
                    SetSelectionByIndex(newIndex);
                    return UIEventResult::INTERCEPT;
                }
                break;

                default: break;
            }
            break;

        default: break;
    }
    return UIEventResult::IGNORE;
}

String UIComboBox::GetSelectedLabel() const
{
    if (GetSelectedIndex() >= 0 &&
        GetSelectedIndex() < int(m_indexToValue.Size()))
    {
        return m_indexToLabel[GetSelectedIndex()];
    }
    return "";
}

const Array<int> &UIComboBox::GetValues() const
{
    return m_indexToValue;
}

Array<int> UIComboBox::GetSelectedValues() const
{
    Array<int> selectedValues;
    const Array<int> &selectedIndices = GetSelectedIndices();
    for (int selectedIndex : selectedIndices)
    {
        selectedValues.PushBack(m_indexToValue[selectedIndex]);
    }
    return selectedValues;
}

int UIComboBox::GetSelectedValuesForFlag() const
{
    int bitwiseOr = 0;
    auto selectedValues = GetSelectedValues();
    for (int selectedValue : selectedValues)
    {
        bitwiseOr |= selectedValue;
    }
    return bitwiseOr;
}

const Array<String> &UIComboBox::GetLabels() const
{
    return m_indexToLabel;
}

bool UIComboBox::HasFocus() const
{
    return GetList()->GetGameObject()->IsEnabledRecursively();
}

UIComboBox *UIComboBox::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIComboBox *comboBox = go->AddComponent<UIComboBox>();
    UIComboBox::CreateIntoWithoutAddingComponent(comboBox, go);
    return comboBox;
}

void UIComboBox::CreateIntoWithoutAddingComponent(UIComboBox *comboBox,
                                                  GameObject *go)
{
    UIHorizontalLayout *hl = go->AddComponent<UIHorizontalLayout>();
    hl->SetChildrenVerticalStretch(Stretch::NONE);
    hl->SetChildrenVerticalAlignment(VerticalAlignment::CENTER);
    hl->SetPaddings(6);
    hl->SetPaddingLeft(10);
    hl->SetPaddingRight(6);
    hl->SetSpacing(8);

    UIFocusable *focusable = go->AddComponent<UIFocusable>();
    focusable->EventEmitter<IEventsFocus>::RegisterListener(comboBox);
    focusable->SetCursorType(Cursor::Type::HAND);
    focusable->SetConsiderForTabbing(true);

    UILabel *currentItemLabel = GameObjectFactory::CreateUILabel();
    currentItemLabel->SetSelectable(false);
    currentItemLabel->GetFocusable()->SetEnabled(false);
    UITextRenderer *currentItemText = currentItemLabel->GetText();
    currentItemText->SetContent("None");
    currentItemText->SetTextSize(12);
    currentItemText->SetHorizontalAlign(HorizontalAlignment::RIGHT);

    UIImageRenderer *bg = go->AddComponent<UIImageRenderer>();
    // bg->SetImageTexture( TextureFactory::Get9SliceRoundRectTexture().Get() );
    // bg->SetMode(UIImageRenderer::Mode::SLICE_9);
    bg->SetTint(UITheme::GetInputsBackgroundColor());
    UIImageRenderer *border = GameObjectFactory::AddInnerBorder(go);

    UIImageRenderer *downArrowIcon = GameObjectFactory::CreateUIImage();
    downArrowIcon->SetImageTexture(TextureFactory::GetDownArrowIcon());
    downArrowIcon->SetTint(UITheme::GetArrowsColor());

    GameObject *downArrowIconGo = downArrowIcon->GetGameObject();
    UILayoutElement *downArrowLE =
        downArrowIconGo->AddComponent<UILayoutElement>();
    downArrowLE->SetMinSize(Vector2i(8));
    downArrowLE->SetFlexibleSize(Vector2::Zero());

    UIList *list = GameObjectFactory::CreateUIList(false);
    GameObject *listGo = list->GetGameObject();
    list->SetWideSelectionMode(true);
    list->GetDirLayout()->SetPaddings(3);
    list->GetDirLayout()->SetPaddingLeft(20);
    list->GetDirLayout()->SetPaddingRight(2);
    list->SetSelectionCallback(
        [comboBox](GameObject *item, UIList::Action action) {
            comboBox->OnListSelectionCallback(item, action);
        });

    UIImageRenderer *listBG = listGo->AddComponent<UIImageRenderer>();
    // listBG->SetImageTexture( TextureFactory::Get9SliceRoundRectTexture() );
    // listBG->SetMode(UIImageRenderer::Mode::SLICE_9);
    listBG->SetTint(UITheme::GetInputsBackgroundColor());
    UIFocusable *listFocusable =
        listBG->GetGameObject()->GetComponent<UIFocusable>();
    listFocusable->SetCursorType(Cursor::Type::HAND);
    GameObjectFactory::AddOuterBorder(listGo);

    listGo->AddComponent<UILayoutIgnorer>();
    RectTransform *contRT = listGo->GetRectTransform();
    contRT->SetAnchors(Vector2(1, -1));
    contRT->SetPivotPosition(Vector2(1, 1));
    contRT->TranslateLocal(Vector3(0.0f, 0.0f, -0.9f));

    UIContentSizeFitter *csf = listGo->AddComponent<UIContentSizeFitter>();
    csf->SetHorizontalSizeType(LayoutSizeType::PREFERRED);
    csf->SetVerticalSizeType(LayoutSizeType::PREFERRED);

    currentItemLabel->GetGameObject()->SetParent(go);
    downArrowIconGo->SetParent(go);
    listGo->SetParent(go);

    comboBox->p_border = border;
    comboBox->p_focusable = focusable;
    comboBox->p_selectedItemText = currentItemText;
    comboBox->p_list = list;

    comboBox->HideList();
}

void UIComboBox::OnListSelectionCallback(GameObject *item,
                                         UIList::Action action)
{
    int indexOfItem = p_list->GetItems().IndexOf(item);
    ASSERT(indexOfItem >= 0);

    switch (action)
    {
        case UIList::Action::MOUSE_LEFT_UP:
            if (IsListBeingShown() &&
                GetList()->GetFocusable()->IsMouseOver() &&
                m_timeWithListShown.GetSeconds() > 0.3)
            {
                if (!GetMultiCheck())
                {
                    SetSelectionByIndex(indexOfItem, true);
                    HideList();
                }
            }
            break;

        case UIList::Action::SELECTION_IN:
            if (GetMultiCheck())
            {
                SetSelectionByIndex(indexOfItem,
                                    !IsSelectedByIndex(indexOfItem));
                GetList()->ClearSelection();
            }
            else
            {
                SetSelectionByIndex(indexOfItem);
            }
            break;

        default: break;
    }
}

UIList *UIComboBox::GetList() const
{
    return p_list;
}
