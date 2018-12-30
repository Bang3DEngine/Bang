#include "Bang/UIList.h"

#include <unordered_map>

#include "Bang/Assert.h"
#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/GameObjectFactory.h"
#include "Bang/IEventsDestroy.h"
#include "Bang/IEventsUIList.h"
#include "Bang/Input.h"
#include "Bang/Key.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/MouseButton.h"
#include "Bang/Rect.h"
#include "Bang/RectTransform.h"
#include "Bang/Stretch.h"
#include "Bang/UICanvas.h"
#include "Bang/UIContentSizeFitter.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIScrollArea.h"
#include "Bang/UIScrollPanel.h"
#include "Bang/UIVerticalLayout.h"
#include "Bang/UMap.tcc"
#include "Bang/Vector2.h"

using namespace Bang;

UIList::UIList()
{
    SET_INSTANCE_CLASS_ID(UIList)
}

UIList::~UIList()
{
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

    Array<UIFocusable *> newItemFocusables =
        newItem->GetComponentsInDescendantsAndThis<UIFocusable>();

    UIImageRenderer *itemBg = newItem->AddComponent<UIImageRenderer>(0);
    itemBg->SetTint(GetIdleColor());

    for (UIFocusable *newItemFocusable : newItemFocusables)
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

    if (index <= m_selectionIndex)
    {
        ++m_selectionIndex;
    }
}

void UIList::RemoveItem_(GOItem *item, bool moving)
{
    int indexOfItem = p_items.IndexOf(item);
    if (indexOfItem < 0)
    {
        return;
    }

    if (p_itemUnderMouse == item)
    {
        p_itemUnderMouse = nullptr;
    }

    if (indexOfItem < GetSelectedIndex())
    {
        m_selectionIndex -= 1;
    }

    if (GetSelectedIndex() == indexOfItem)
    {
        ClearSelection();
    }

    p_items.Remove(item);
    p_itemsBackground.Remove(item);

    // Destroy the element
    if (!moving)
    {
        EventEmitter<IEventsUIList>::PropagateToListeners(
            &IEventsUIList::OnItemRemoved, item);
        GameObject::Destroy(item);
    }
    else
    {
        item->SetParent(nullptr);
    }
}

UIEventResult UIList::OnMouseMove(bool forceColorsUpdate, bool callCallbacks)
{
    UICanvas *canvas = UICanvas::GetActive(this);
    if (!canvas)
    {
        return UIEventResult::IGNORE;
    }

    GOItem *itemUnderMouse = nullptr;
    if (canvas->IsMouseOver(GetContainer(), true))
    {
        const Vector2 mousePos = Input::GetMousePositionNDC();
        const AARect listRTNDCRect(
            GetGameObject()->GetRectTransform()->GetViewportAARectNDC());
        for (GOItem *childItem : p_items)
        {
            if (!childItem || !childItem->IsActiveRecursively())
            {
                continue;
            }

            bool overChildItem;
            if (m_wideSelectionMode)
            {
                AARect itemRTRect(
                    childItem->GetRectTransform()->GetViewportAARectNDC());
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

    if ((p_itemUnderMouse != itemUnderMouse) || forceColorsUpdate)
    {
        SetItemUnderMouse(itemUnderMouse, callCallbacks);
        return UIEventResult::INTERCEPT;
    }
    return UIEventResult::IGNORE;
}

UIImageRenderer *UIList::GetItemBg(GOItem *item) const
{
    if (!item || !p_itemsBackground.ContainsKey(item))
    {
        return nullptr;
    }
    return p_itemsBackground.Get(item);
}

void UIList::SetItemUnderMouse(GOItem *itemUnderMouse, bool callCallbacks)
{
    if (itemUnderMouse != p_itemUnderMouse)
    {
        if (p_itemUnderMouse)
        {
            if (GetSelectedItem() != p_itemUnderMouse)
            {
                if (UIImageRenderer *itemBg = GetItemBg(p_itemUnderMouse))
                {
                    itemBg->SetTint(GetIdleColor());
                }
            }

            p_itemUnderMouse->EventEmitter<IEventsDestroy>::UnRegisterListener(
                this);
            if (callCallbacks)
            {
                CallSelectionCallback(p_itemUnderMouse, Action::MOUSE_OUT);
            }
        }

        p_itemUnderMouse = itemUnderMouse;
        if (p_itemUnderMouse)
        {
            if (GetSelectedItem() != p_itemUnderMouse)
            {
                if (UIImageRenderer *itemBg = GetItemBg(p_itemUnderMouse))
                {
                    itemBg->SetTint(GetOverColor());
                }
            }

            p_itemUnderMouse->EventEmitter<IEventsDestroy>::RegisterListener(
                this);
            if (callCallbacks)
            {
                CallSelectionCallback(p_itemUnderMouse, Action::MOUSE_OVER);
            }
        }
    }
}

void UIList::ClearSelection()
{
    SetSelection(-1);
}

void UIList::Clear()
{
    while (!p_items.IsEmpty())
    {
        RemoveItem(p_items.Back());
    }

    if (GetScrollPanel())
    {
        GetScrollPanel()->SetScrollingPercent(Vector2(0.0f));
    }

    ClearSelection();
}

void UIList::SetIdleColor(const Color &idleColor)
{
    m_idleColor = idleColor;
}

const Array<GOItem *> &UIList::GetItems() const
{
    return p_items;
}

GOItem *UIList::GetItem(int i) const
{
    if (i >= 0 && i < SCAST<int>(p_items.Size()))
    {
        return GetItems()[i];
    }
    return nullptr;
}

void UIList::SetNotifySelectionOnFullClick(bool notifySelectionOnFullClick)
{
    m_notifySelectionOnFullClick = notifySelectionOnFullClick;
}

void UIList::ScrollToBegin()
{
    GetScrollPanel()->SetScrollingPercent(Vector2(0.0f));
}

void UIList::ScrollTo(int i)
{
    ScrollTo(GetItem(i));
}

void UIList::ScrollTo(GOItem *item)
{
    if (!GetScrollPanel())
    {
        return;
    }

    AARect itemRect(item->GetRectTransform()->GetViewportRect());
    AARect panelRect(GetScrollPanel()
                         ->GetGameObject()
                         ->GetRectTransform()
                         ->GetViewportRect());
    if (!panelRect.IsValid())
    {
        return;
    }

    AARect containerRect(GetContainer()->GetRectTransform()->GetViewportRect());
    if (!containerRect.IsValid())
    {
        return;
    }

    Vector2 relativeItemRectMin = itemRect.GetMin() - containerRect.GetMin();
    relativeItemRectMin.y = (containerRect.GetHeight() - relativeItemRectMin.y);
    Vector2 relativeItemRectMax = relativeItemRectMin + itemRect.GetSize();
    AARect relativeItemRect;
    relativeItemRect.SetMin(
        Vector2::Min(relativeItemRectMin, relativeItemRectMax));
    relativeItemRect.SetMax(
        Vector2::Max(relativeItemRectMin, relativeItemRectMax));

    Vector2i scrolling = -Vector2i::One();
    if (itemRect.GetMax().y > panelRect.GetMax().y)
    {
        scrolling = Vector2i(relativeItemRect.GetMax() - panelRect.GetHeight());
    }
    else if (itemRect.GetMin().y < panelRect.GetMin().y)
    {
        scrolling = Vector2i(relativeItemRect.GetMin() - panelRect.GetHeight());
    }

    if (scrolling != -Vector2i::One())
    {
        GetScrollPanel()->SetScrolling(scrolling);
    }
}

void UIList::ScrollToEnd()
{
    GetScrollPanel()->SetScrollingPercent(Vector2(1.0f));
}

int UIList::GetNumItems() const
{
    return p_items.Size();
}

UIDirLayout *UIList::GetDirLayout() const
{
    return p_dirLayout;
}

void UIList::SetSelection(int index)
{
    if (GetSelectedIndex() != index)
    {
        GOItem *prevSelectedItem = GetSelectedItem();
        if (prevSelectedItem)
        {
            GetItemBg(prevSelectedItem)->SetTint(GetIdleColor());
            CallSelectionCallback(prevSelectedItem, Action::SELECTION_OUT);
        }

        if (index >= 0 && index < GetNumItems())
        {
            m_selectionIndex = index;
            GOItem *selectedItem = GetSelectedItem();
            if (selectedItem)
            {
                ScrollTo(selectedItem);
                GetItemBg(selectedItem)->SetTint(GetSelectedColor());
                CallSelectionCallback(selectedItem, Action::SELECTION_IN);
            }
        }
        else if (index == -1)
        {
            m_selectionIndex = -1;
        }

        OnMouseMove(true, false);
    }
}

UIEventResult UIList::OnUIEvent(UIFocusable *, const UIEvent &event)
{
    switch (event.type)
    {
        case UIEvent::Type::MOUSE_EXIT: SetItemUnderMouse(nullptr, true); break;

        case UIEvent::Type::MOUSE_ENTER:
        case UIEvent::Type::MOUSE_MOVE: { return OnMouseMove();
        }
        break;

        case UIEvent::Type::MOUSE_CLICK_DOWN:
            if (p_itemUnderMouse)
            {
                if (event.mouse.button == MouseButton::LEFT)
                {
                    if (!m_notifySelectionOnFullClick)
                    {
                        SetSelection(p_itemUnderMouse);
                    }
                    CallSelectionCallback(p_itemUnderMouse,
                                          Action::MOUSE_LEFT_DOWN);
                    return UIEventResult::INTERCEPT;
                }
                else if (event.mouse.button == MouseButton::RIGHT)
                {
                    CallSelectionCallback(p_itemUnderMouse,
                                          Action::MOUSE_RIGHT_DOWN);
                    return UIEventResult::INTERCEPT;
                }
            }
            break;

        case UIEvent::Type::MOUSE_CLICK_UP:
            if (p_itemUnderMouse)
            {
                if (event.mouse.button == MouseButton::LEFT)
                {
                    CallSelectionCallback(p_itemUnderMouse,
                                          Action::MOUSE_LEFT_UP);
                    return UIEventResult::INTERCEPT;
                }
            }
            break;

        case UIEvent::Type::MOUSE_CLICK_DOUBLE:
            if (p_itemUnderMouse)
            {
                CallSelectionCallback(p_itemUnderMouse,
                                      Action::DOUBLE_CLICKED_LEFT);
                return UIEventResult::INTERCEPT;
            }
            break;

        case UIEvent::Type::MOUSE_CLICK_FULL:
            if (p_itemUnderMouse && m_notifySelectionOnFullClick)
            {
                if (event.mouse.button == MouseButton::LEFT)
                {
                    SetSelection(p_itemUnderMouse);
                    return UIEventResult::INTERCEPT;
                }
            }
            break;

        case UIEvent::Type::KEY_DOWN:
        {
            int newSelectedIndex = -1;
            int numItems = GetNumItems();
            switch (event.key.key)
            {
                case Key::UP:
                case Key::DOWN:
                {
                    int inc = (event.key.key == Key::DOWN ? 1 : -1);
                    GOItem *newSelectedItem;
                    newSelectedIndex = GetSelectedIndex();
                    do
                    {
                        newSelectedIndex =
                            (newSelectedIndex + inc + numItems) % numItems;
                        newSelectedItem = GetItem(newSelectedIndex);
                        if (newSelectedIndex == GetSelectedIndex())
                        {
                            break;
                        }
                    } while (newSelectedIndex != GetSelectedIndex() &&
                             !newSelectedItem->IsEnabledRecursively());
                }
                break;

                case Key::HOME: newSelectedIndex = 0; break;

                case Key::END: newSelectedIndex = GetNumItems() - 1; break;

                case Key::RIGHT:
                case Key::ENTER:
                {
                    GOItem *selectedItem = GetSelectedItem();
                    if (selectedItem)
                    {
                        CallSelectionCallback(selectedItem, Action::PRESSED);
                        return UIEventResult::INTERCEPT;
                    }
                }
                break;

                default: break;
            }

            if (newSelectedIndex >= 0)
            {
                SetSelection(newSelectedIndex);
                return UIEventResult::INTERCEPT;
            }
        }
        break;

        default: break;
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
    if (UICanvas *canvas = UICanvas::GetActive(this))
    {
        return canvas->HasFocus(this, true);
    }
    return false;
}

GOItem *UIList::GetSelectedItem() const
{
    return GetItem(GetSelectedIndex());
}

UIFocusable *UIList::GetFocusable() const
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

void UIList::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (object == p_itemUnderMouse)
    {
        if (UIImageRenderer *bg = GetItemBg(p_itemUnderMouse))
        {
            bg->SetTint(GetIdleColor());
        }
        CallSelectionCallback(p_itemUnderMouse, Action::SELECTION_OUT);
        p_itemUnderMouse = nullptr;
    }
}

void UIList::SetSelectionCallback(SelectionCallback selectionCallback)
{
    m_selectionCallback = selectionCallback;
}

void UIList::ClearSelectionCallback()
{
    m_selectionCallback = nullptr;
}

UIList *UIList::CreateInto(GameObject *go, bool withScrollPanel)
{
    REQUIRE_COMPONENT(go, RectTransform);

    UIList *list = go->AddComponent<UIList>();
    go->SetName("UIList");

    GameObject *container =
        withScrollPanel ? GameObjectFactory::CreateUIGameObject() : go;
    container->SetName("UIList");

    UIVerticalLayout *containerVL = container->AddComponent<UIVerticalLayout>();
    containerVL->SetChildrenVerticalStretch(Stretch::NONE);
    containerVL->SetChildrenHorizontalStretch(Stretch::FULL);
    list->p_dirLayout = containerVL;

    list->p_focusable = container->AddComponent<UIFocusable>();
    list->p_focusable->EventEmitter<IEventsFocus>::RegisterListener(list);

    if (withScrollPanel)
    {
        UIScrollPanel *scrollPanel = nullptr;
        scrollPanel = GameObjectFactory::CreateUIScrollPanelInto(go);

        UIContentSizeFitter *csf =
            container->AddComponent<UIContentSizeFitter>();
        csf->SetHorizontalSizeType(LayoutSizeType::NONE);
        csf->SetVerticalSizeType(LayoutSizeType::PREFERRED);
        container->GetRectTransform()->SetPivotPosition(Vector2(-1, 1));

        scrollPanel->GetScrollArea()->SetContainedGameObject(container);

        list->p_scrollPanel = scrollPanel;
    }
    else
    {
        list->p_scrollPanel = nullptr;
    }

    list->p_container = container;

    return list;
}

void UIList::CallSelectionCallback(GOItem *item, Action action)
{
    if (m_selectionCallback)
    {
        m_selectionCallback(item, action);
    }
}

UIScrollPanel *UIList::GetScrollPanel() const
{
    return p_scrollPanel;
}

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
