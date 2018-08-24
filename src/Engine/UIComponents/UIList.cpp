#include "Bang/UIList.h"

#include "Bang/Rect.h"
#include "Bang/Input.h"
#include "Bang/AARect.h"
#include "Bang/Material.h"
#include "Bang/UICanvas.h"
#include "Bang/IFocusable.h"
#include "Bang/GameObject.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIScrollBar.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UIScrollPanel.h"
#include "Bang/RectTransform.h"
#include "Bang/UITextRenderer.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutIgnorer.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIContentSizeFitter.h"

USING_NAMESPACE_BANG

UIList::UIList()
{
}

UIList::~UIList()
{
}

void UIList::OnUpdate()
{
    Component::OnUpdate();
    HandleShortcuts();
}

void UIList::AddItem(GOItem *newItem)
{
    AddItem(newItem, GetNumItems());
}

void UIList::AddItem(GOItem *newItem, int index)
{
    AddItem_(newItem, index, false);
}

void UIList::MoveItem(GOItem *item, int index)
{
    ASSERT(index >= 0 && index <= GetNumItems());

    int oldIndexOfItem = p_items.IndexOf(item);
    ASSERT(oldIndexOfItem >= 0);

    if (oldIndexOfItem != index)
    {
        int newIndex = (oldIndexOfItem < index) ? (index - 1) : index;

        p_items.Remove(item);
        p_items.Insert(item, newIndex);
        item->SetParent(GetContainer(), index);

        EventEmitter<IEventsUIList>::PropagateToListeners(
                    &IEventsUIList::OnItemMoved, item, oldIndexOfItem, newIndex);
    }
}

void UIList::RemoveItem(GOItem *item)
{
    RemoveItem_(item, false);
}

void UIList::AddItem_(GOItem *newItem, int index, bool moving)
{
    ASSERT(index >= 0 && index <= GetNumItems());

    Array<IFocusable*> newItemFocusables =
                       newItem->GetComponentsInDescendantsAndThis<IFocusable>();

    UIImageRenderer *itemBg = newItem->AddComponent<UIImageRenderer>(0);
    itemBg->SetTint( GetIdleColor() );

    for (IFocusable* newItemFocusable : newItemFocusables)
    {
        newItemFocusable->EventEmitter<IEventsFocus>::RegisterListener(this);
    }

    newItem->EventEmitter<IEventsDestroy>::RegisterListener(this);
    newItem->SetParent(GetContainer(), index);

    p_itemsBackground.Add(newItem, itemBg);
    p_items.Insert(newItem, index);

    if (!moving)
    {
        EventEmitter<IEventsUIList>::PropagateToListeners(
                    &IEventsUIList::OnItemAdded, newItem, index);
    }

    if (index <= m_selectionIndex) { ++m_selectionIndex; }
}

void UIList::RemoveItem_(GOItem *item, bool moving)
{
    int indexOfItem = p_items.IndexOf(item);
    if (indexOfItem < 0) { return; }

    if (p_itemUnderMouse == item) { p_itemUnderMouse = nullptr; }
    if (indexOfItem < GetSelectedIndex()) { m_selectionIndex -= 1; }
    if (GetSelectedIndex() == indexOfItem) { ClearSelection(); }

    p_items.Remove(item);
    p_itemsBackground.Remove(item);

    // Destroy the element
    if (!moving) { GameObject::Destroy(item); }
    item->SetParent(nullptr);

    if (!moving)
    {
        EventEmitter<IEventsUIList>::PropagateToListeners(
                    &IEventsUIList::OnItemRemoved, item);
    }
}

void UIList::ClearSelection()
{
    if (GetSelectedIndex() >= 0)
    {
        SetSelection(-1);
    }
}

void UIList::Clear()
{
    while (!p_items.IsEmpty()) { RemoveItem(p_items.Front()); }
    if (GetScrollPanel())
    {
        GetScrollPanel()->SetScrollingPercent( Vector2(0.0f) );
    }
    ClearSelection();
}

void UIList::SetIdleColor(const Color &idleColor)
{
    m_idleColor = idleColor;
}

const Array<GOItem *> &UIList::GetItems() const { return p_items; }
GOItem *UIList::GetItem(int i) const
{
    if (i >= 0 && i < p_items.Size())
    {
        // auto it = GetItems().Begin();
        // std::advance(it, GetSelectedIndex());
        // return *it;
        return GetItems()[i];
    }
    return nullptr;
}

void UIList::ScrollToBegin()
{
    GetScrollPanel()->SetScrollingPercent( Vector2(0.0f) );
}

void UIList::ScrollTo(int i)
{
    ScrollTo( GetItem(i) );
}

void UIList::ScrollTo(GOItem *item)
{
    if (!GetScrollPanel()) { return; }

    AARect itemRect ( item->GetRectTransform()-> GetViewportRect() );
    AARect panelRect ( GetScrollPanel()->GetGameObject()->GetRectTransform()->
                                                          GetViewportRect() );
    ASSERT(panelRect.IsValid());

    AARect containerRect ( GetContainer()->GetRectTransform()->GetViewportRect() );
    ASSERT(containerRect.IsValid());

    Vector2 relativeItemRectMin = itemRect.GetMin() - containerRect.GetMin();
    relativeItemRectMin.y = (containerRect.GetHeight() - relativeItemRectMin.y);
    Vector2 relativeItemRectMax = relativeItemRectMin + itemRect.GetSize();
    AARect relativeItemRect;
    relativeItemRect.SetMin( Vector2::Min(relativeItemRectMin, relativeItemRectMax) );
    relativeItemRect.SetMax( Vector2::Max(relativeItemRectMin, relativeItemRectMax) );

    Vector2i scrolling = -Vector2i::One;
    if (itemRect.GetMax().y > panelRect.GetMax().y)
    {
        scrolling = Vector2i(relativeItemRect.GetMax() - panelRect.GetHeight());
    }
    else if (itemRect.GetMin().y < panelRect.GetMin().y)
    {
        scrolling = Vector2i(relativeItemRect.GetMin() - panelRect.GetHeight());
    }

    if (scrolling != -Vector2i::One)
    {
        GetScrollPanel()->SetScrolling(scrolling);
    }
}

void UIList::ScrollToEnd()
{
    GetScrollPanel()->SetScrollingPercent( Vector2(1.0f) );
}

int UIList::GetNumItems() const
{
    return p_items.Size();
}

UIDirLayout *UIList::GetDirLayout() const
{
    return GetGameObject()->GetComponent<UIDirLayout>();
}

void UIList::SetSelection(int index)
{
    if (GetSelectedIndex() != index)
    {
        GOItem *prevSelectedItem = GetSelectedItem();
        if (prevSelectedItem)
        {
            CallSelectionCallback(prevSelectedItem, Action::SELECTION_OUT);
        }
    }

    if (GetSelectedIndex() != index && index >= 0 && index < GetNumItems())
    {
        m_selectionIndex = index;
        GOItem *selectedItem = GetSelectedItem();
        if (selectedItem)
        {
            ScrollTo(selectedItem);
            CallSelectionCallback(selectedItem, Action::SELECTION_IN);
        }
    }
    else if (index == -1)
    {
        m_selectionIndex = -1;
    }
}

void UIList::HandleShortcuts()
{
    int newSelectedIndex = -1;

    int numItems = GetNumItems();
    if (numItems > 0 && UICanvas::GetActive(this)->HasFocus(this, true))
    {
        if (Input::GetKeyDownRepeat(Key::DOWN) || Input::GetKeyDownRepeat(Key::UP))
        {
            int inc = Input::GetKeyDownRepeat(Key::DOWN) ? 1 : -1;
            GOItem *newSelectedItem;
            newSelectedIndex = GetSelectedIndex();
            do
            {
                newSelectedIndex = (newSelectedIndex + inc + numItems) % numItems;
                newSelectedItem = GetItem(newSelectedIndex);
                if (newSelectedIndex == GetSelectedIndex()) { break; }
            }
            while (newSelectedIndex != GetSelectedIndex() &&
                   !newSelectedItem->IsEnabled());
        }
        else if (Input::GetKeyDownRepeat(Key::PAGEDOWN) ||
                 Input::GetKeyDownRepeat(Key::PAGEUP))
        {
            if (GetScrollPanel())
            {
                int sign = Input::GetKeyDownRepeat(Key::PAGEDOWN) ? 1 : -1;
                GetScrollPanel()->SetScrolling( GetScrollPanel()->GetScrolling() +
                                  sign * Vector2i(GetScrollPanel()->GetContainerSize()) );
            }
        }
        else if (Input::GetKeyDown(Key::END))
        {
            newSelectedIndex = GetNumItems() - 1;
        }
        else if (Input::GetKeyDown(Key::HOME))
        {
            newSelectedIndex = 0;
        }

        if (newSelectedIndex >= 0)
        {
            SetSelection(newSelectedIndex);
        }


        if (Input::GetKeyDownRepeat(Key::RIGHT) ||
            Input::GetKeyDownRepeat(Key::ENTER))
        {
            GOItem *selectedItem = GetSelectedItem();
            if (selectedItem)
            {
                CallSelectionCallback(selectedItem, Action::PRESSED);
            }
        }
    }

}

UIEventResult UIList::UIEventCallback(IFocusable*, const UIEvent &event)
{
    switch (event.type)
    {
        case UIEvent::Type::MOUSE_EXIT:
        if (p_itemUnderMouse)
        {
            ASSERT(p_itemUnderMouse);
            CallSelectionCallback(p_itemUnderMouse, Action::MOUSE_OUT);
            p_itemUnderMouse = nullptr;
            return UIEventResult::INTERCEPT;
        }
        break;

        case UIEvent::Type::MOUSE_ENTER:
        case UIEvent::Type::MOUSE_MOVE:
        {
            UICanvas *canvas = UICanvas::GetActive(this);
            GOItem *itemUnderMouse = nullptr;
            if (canvas->IsMouseOver(GetContainer(), true))
            {
                const Vector2 mousePos = Input::GetMousePositionNDC();
                const AARect listRTNDCRect ( GetGameObject()->GetRectTransform()->
                                             GetViewportAARectNDC() );
                for (GOItem *childItem : p_items)
                {
                    if (!childItem->IsActive())
                    {
                        continue;
                    }

                    bool overChildItem;
                    if (m_wideSelectionMode)
                    {
                        AARect itemRTRect ( childItem->GetRectTransform()->GetViewportAARectNDC() );
                        overChildItem = (mousePos.x >= listRTNDCRect.GetMin().x &&
                                         mousePos.x <= listRTNDCRect.GetMax().x &&
                                         mousePos.y >= itemRTRect.GetMin().y &&
                                         mousePos.y <= itemRTRect.GetMax().y);
                    }
                    else
                    {
                        overChildItem = canvas->IsMouseOver(childItem, false);
                    }

                    if (overChildItem)
                    {
                        itemUnderMouse = childItem;
                        break;
                    }
                }
            }

            if (p_itemUnderMouse != itemUnderMouse)
            {
                if (p_itemUnderMouse)
                {
                    CallSelectionCallback(p_itemUnderMouse, Action::MOUSE_OUT);
                }

                p_itemUnderMouse = itemUnderMouse;
                if (p_itemUnderMouse)
                {
                    CallSelectionCallback(p_itemUnderMouse, Action::MOUSE_OVER);
                }
                return UIEventResult::INTERCEPT;
            }
        }
        break;

        case UIEvent::Type::MOUSE_CLICK_DOWN:
            if (p_itemUnderMouse)
            {
                if (event.mouse.button == MouseButton::LEFT)
                {
                    SetSelection(p_itemUnderMouse);
                    CallSelectionCallback(p_itemUnderMouse, Action::MOUSE_LEFT_DOWN);
                    return UIEventResult::INTERCEPT;
                }
                else if (event.mouse.button == MouseButton::RIGHT)
                {
                    CallSelectionCallback(p_itemUnderMouse, Action::MOUSE_RIGHT_DOWN);
                    return UIEventResult::INTERCEPT;
                }
            }
        break;

        case UIEvent::Type::MOUSE_CLICK_UP:
            if (p_itemUnderMouse)
            {
                if (event.mouse.button == MouseButton::LEFT)
                {
                    CallSelectionCallback(p_itemUnderMouse, Action::MOUSE_LEFT_UP);
                    return UIEventResult::INTERCEPT;
                }
            }
        break;

        case UIEvent::Type::MOUSE_CLICK_DOUBLE:
        if (p_itemUnderMouse)
        {
            CallSelectionCallback(p_itemUnderMouse, Action::DOUBLE_CLICKED_LEFT);
            return UIEventResult::INTERCEPT;
        }
        break;

        default:
        break;
    }
    return UIEventResult::IGNORE;
}

void UIList::SetSelection(GOItem *item)
{
    SetSelection(p_items.IndexOf(item));
}

GameObject *UIList::GetContainer() const
{
    return p_container;
}

int UIList::GetSelectedIndex() const
{
    return m_selectionIndex;
}

bool UIList::SomeChildHasFocus() const
{
    if (UICanvas *canvas = UICanvas::GetActive( this ))
    {
        return canvas->HasFocus(this, true);
    }
    return false;
}

GOItem *UIList::GetSelectedItem() const
{
    return GetItem( GetSelectedIndex() );
}

IFocusable *UIList::GetFocusable() const
{
    return p_focusable;
}

void UIList::SetWideSelectionMode(bool wideSelectionMode)
{
    m_wideSelectionMode = wideSelectionMode;
}

void UIList::SetOverColor(const Color &overColor)
{
    m_overColor = overColor;
}

void UIList::SetSelectedColor(const Color &selectedColor)
{
    m_selectedColor = selectedColor;
}

void UIList::SetUseSelectedColor(bool useSelectColor)
{
    m_useSelectColor = useSelectColor;
}

void UIList::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (object == p_itemUnderMouse)
    {
        CallSelectionCallback(p_itemUnderMouse, Action::SELECTION_OUT);
        p_itemUnderMouse = nullptr;
    }
}

void UIList::SetSelectionCallback(SelectionCallback selectionCallback)
{
    m_selectionCallback = selectionCallback;
}

UIList* UIList::CreateInto(GameObject *go, bool withScrollPanel)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIList *list = go->AddComponent<UIList>();
    go->SetName("UIList");

    const bool vertical = true;
    GameObject *container = withScrollPanel ?
                                GameObjectFactory::CreateUIGameObject() : go;
    container->SetName("UIList");

    UIDirLayout *dirLayout = nullptr;
    if (vertical)
    {
        dirLayout = container->AddComponent<UIVerticalLayout>();
    }
    else
    {
        dirLayout = container->AddComponent<UIHorizontalLayout>();
    }
    dirLayout->SetChildrenVerticalStretch(Stretch::NONE);
    dirLayout->SetChildrenHorizontalStretch(Stretch::FULL);

    list->p_focusable = container->AddComponent<UIFocusable>();
    list->p_focusable->AddEventCallback([list](IFocusable *focusable,
                                               const UIEvent &event)
    {
        return list->UIEventCallback(focusable, event);
    });


    if (withScrollPanel)
    {
        UIScrollPanel *scrollPanel = nullptr;
        scrollPanel = GameObjectFactory::CreateUIScrollPanelInto(go);

        UIContentSizeFitter *csf = container->AddComponent<UIContentSizeFitter>();
        csf->SetHorizontalSizeType(LayoutSizeType::NONE);
        csf->SetVerticalSizeType(LayoutSizeType::PREFERRED);
        container->GetRectTransform()->SetPivotPosition(Vector2(-1,1));

        scrollPanel->GetScrollArea()->SetContainedGameObject(container);

        list->p_scrollPanel = scrollPanel;
    }
    else { list->p_scrollPanel = nullptr; }

    list->p_container = container;

    return list;
}

void UIList::CallSelectionCallback(GOItem *item, Action action)
{
    UIImageRenderer *itemBg = p_itemsBackground.Get(item);
    ASSERT(itemBg);

    bool isSelected = (GetSelectedItem() == item);
    switch (action)
    {
        case UIList::Action::MOUSE_OVER:
            if (!m_useSelectColor || !isSelected) { itemBg->SetTint( GetOverColor() ); }
        break;

        case UIList::Action::MOUSE_OUT:
            if (!m_useSelectColor || !isSelected) { itemBg->SetTint( GetIdleColor() ); }
        break;

        case UIList::Action::SELECTION_IN:
            if (m_useSelectColor) { itemBg->SetTint( GetSelectedColor() ); }
        break;

        case UIList::Action::SELECTION_OUT:
            if (m_useSelectColor) { itemBg->SetTint( GetIdleColor() ); }
        break;

        default: break;
    }

    if (m_selectionCallback) { m_selectionCallback(item, action); }
}

UIScrollPanel *UIList::GetScrollPanel() const { return p_scrollPanel; }

const Color &UIList::GetIdleColor() const
{
    return m_idleColor;
}

const Color &UIList::GetOverColor() const
{
    return m_overColor;
}

const Color &UIList::GetSelectedColor() const
{
    return m_selectedColor;
}
