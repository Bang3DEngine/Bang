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
#include "Bang/Scene.h"
#include "Bang/Stretch.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureFactory.h"
#include "Bang/UICanvas.h"
#include "Bang/UIContentSizeFitter.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UIListItemContainer.h"
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
    GameObject::Destroy(p_dragMarker);
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
        int newIndexAfterRemove =
            (oldIndexOfItem < index) ? (index - 1) : index;

        p_items.RemoveByIndex(oldIndexOfItem);
        p_items.Insert(item, newIndexAfterRemove);

        UIListItemContainer *itemCont = GetItemContainer(item);
        itemCont->SetParent(GetContainer(), index);

        if (GetSelectedIndex() == oldIndexOfItem)
        {
            m_selectionIndex = newIndexAfterRemove;
        }

        EventEmitter<IEventsUIList>::PropagateToListeners(
            &IEventsUIList::OnItemMoved,
            item,
            oldIndexOfItem,
            newIndexAfterRemove);
    }
}

void UIList::RemoveItem(GOItem *item)
{
    RemoveItem_(item, false);
}

void UIList::AddItem_(GOItem *newItem, int index, bool moving)
{
    ASSERT(index >= 0 && index <= GetNumItems());

    UIListItemContainer *newItemCont = new UIListItemContainer();
    newItemCont->SetContainedGameObject(newItem);

    newItemCont->GetFocusable()->EventEmitter<IEventsFocus>::RegisterListener(
        this);
    newItemCont->GetDragDroppable()->SetEnabled(GetDragDropEnabled());

    newItem->EventEmitter<IEventsDestroy>::RegisterListener(this);

    newItemCont->SetParent(GetContainer(), index);

    GetItemBg(newItem)->SetTint(GetIdleColor());

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

    // Destroy the element
    if (!moving)
    {
        EventEmitter<IEventsUIList>::PropagateToListeners(
            &IEventsUIList::OnItemRemoved, item);
        GameObject::Destroy(item->GetParent());
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
        for (GOItem *item : p_items)
        {
            if (!item || !item->IsActiveRecursively())
            {
                continue;
            }

            UIListItemContainer *itemCont = GetItemContainer(item);
            if (itemCont->GetFocusable()->IsMouseOver())
            {
                itemUnderMouse = item;
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

UIImageRenderer *UIList::GetItemBg(GameObject *item) const
{
    if (item)
    {
        if (UIListItemContainer *itemCont = GetItemContainer(item))
        {
            return itemCont->GetBackground();
        }
    }
    return nullptr;
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
        if (GOItem *prevSelectedItem = GetSelectedItem())
        {
            if (UIImageRenderer *itemBg = GetItemBg(prevSelectedItem))
            {
                itemBg->SetTint(GetIdleColor());
            }
            CallSelectionCallback(prevSelectedItem, Action::SELECTION_OUT);
        }

        m_selectionIndex = index;
        if (index >= 0 && index < GetNumItems())
        {
            if (GOItem *selectedItem = GetSelectedItem())
            {
                if (UIImageRenderer *itemBg = GetItemBg(selectedItem))
                {
                    itemBg->SetTint(GetSelectedColor());
                }
                CallSelectionCallback(selectedItem, Action::SELECTION_IN);
            }
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
        case UIEvent::Type::MOUSE_MOVE: return OnMouseMove(); break;

        case UIEvent::Type::MOUSE_CLICK_DOWN:
            if (p_itemUnderMouse)
            {
                if (event.mouse.button == MouseButton::LEFT ||
                    event.mouse.button == MouseButton::RIGHT)
                {
                    if (!m_notifySelectionOnFullClick)
                    {
                        SetSelection(p_itemUnderMouse);
                    }
                    CallSelectionCallback(
                        p_itemUnderMouse,
                        event.mouse.button == MouseButton::LEFT
                            ? Action::MOUSE_LEFT_DOWN
                            : Action::MOUSE_RIGHT_DOWN);
                    return UIEventResult::IGNORE;
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
                if (event.mouse.button == MouseButton::LEFT ||
                    event.mouse.button == MouseButton::RIGHT)
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

void UIList::GetMousePositionInList(
    GOItem **itemOverOut,
    UIList::MouseItemRelativePosition *itemRelPosOut) const
{
    if (GetGameObject()->GetRectTransform()->IsMouseOver())
    {
        if (!GetItems().IsEmpty())
        {
            bool foundItemOver = false;
            const Vector2i mousePos = Input::GetMousePosition();
            for (int i = 0; i < GetNumItems(); ++i)
            {
                GameObject *itChildItemCont = GetItems()[i];
                GameObject *childItemCont = itChildItemCont;
                if (!childItemCont->IsActiveRecursively())
                {
                    continue;
                }

                RectTransform *childItemContRT =
                    childItemCont->GetRectTransform();
                AARect childItemContRect = childItemContRT->GetViewportAARect();
                if (childItemContRect.Contains(Vector2(mousePos)))
                {
                    if (mousePos.y >= childItemContRect.GetCenter().y)
                    {
                        *itemRelPosOut = MouseItemRelativePosition::ABOVE;
                    }
                    else
                    {
                        *itemRelPosOut = MouseItemRelativePosition::BELOW;
                    }

                    GameObject *childItemGo = itChildItemCont;
                    *itemOverOut = childItemGo;
                    foundItemOver = true;
                    break;
                }
            }

            if (!foundItemOver && !GetItems().IsEmpty())
            {
                GameObject *firstItemGo = GetItems().Front();
                RectTransform *firstItemContRT =
                    firstItemGo->GetRectTransform();
                if (mousePos.y >=
                    firstItemContRT->GetViewportAARect().GetMax().y)
                {
                    *itemOverOut = firstItemGo;  // Above all items
                    *itemRelPosOut = MouseItemRelativePosition::ABOVE;
                }
                else  // Below all items
                {
                    *itemOverOut = nullptr;
                    *itemRelPosOut = MouseItemRelativePosition::BELOW_ALL;
                }
            }
        }
        else
        {
            *itemOverOut = nullptr;
        }
    }
    else
    {
        *itemOverOut = nullptr;
    }
}

void UIList::OnDragStarted(EventEmitter<IEventsDragDrop> *dd_)
{
    IEventsDragDrop::OnDragStarted(dd_);

    if (!GetDragDropEnabled())
    {
        return;
    }

    UIDragDroppable *dd = DCAST<UIDragDroppable *>(dd_);
    if (UIListItemContainer *draggedItemCont =
            GetItemContainer(dd->GetGameObject()))
    {
        p_itemGoBeingDragged = draggedItemCont;
    }
    else
    {
        p_itemGoBeingDragged = nullptr;
    }

    p_dragMarker->SetParent(GetGameObject()->GetScene());
}

void UIList::OnDragUpdate(EventEmitter<IEventsDragDrop> *dd_)
{
    IEventsDragDrop::OnDragUpdate(dd_);

    if (!GetDragDropEnabled())
    {
        return;
    }

    UIDragDroppable *dragDroppable = DCAST<UIDragDroppable *>(dd_);

    GOItem *childItemOver = nullptr;
    MouseItemRelativePosition markPosition = MouseItemRelativePosition::ABOVE;
    GetMousePositionInList(&childItemOver, &markPosition);

    AH<Texture2D> markImgAH;
    constexpr int MarkStroke = 2;
    Vector2 dragMarkerPos, dragMarkerSize;
    UIImageRenderer::Mode markImgMode = UIImageRenderer::Mode::TEXTURE_INV_UVY;
    if (childItemOver)
    {
        GOItem *childItem = childItemOver;
        RectTransform *childItemRT = childItem->GetRectTransform();
        AARect childItemRect = childItemRT->GetViewportAARect();
        switch (markPosition)
        {
            case MouseItemRelativePosition::ABOVE:
                dragMarkerPos = childItemRect.GetMinXMaxY();
                dragMarkerSize = childItemRect.GetMax() - dragMarkerPos;
                dragMarkerSize.y += MarkStroke;
                break;

            case MouseItemRelativePosition::OVER:
                dragMarkerSize = Vector2::Zero();
                break;

            case MouseItemRelativePosition::BELOW:
                dragMarkerPos = childItemRect.GetMinXMinY();
                dragMarkerSize = childItemRect.GetMaxXMinY() - dragMarkerPos;
                dragMarkerSize.y += MarkStroke;
                break;

            default: break;
        }
    }
    else
    {
        if (markPosition == MouseItemRelativePosition::BELOW_ALL)
        {
            GOItem *lastItem = nullptr;
            for (GameObject *item : GetItems())
            {
                if (item->IsActiveRecursively())
                {
                    lastItem = item;
                }
            }

            if (lastItem)
            {
                AARect lastItemRect =
                    lastItem->GetRectTransform()->GetViewportAARect();
                dragMarkerPos = lastItemRect.GetMinXMinY();
                dragMarkerSize = lastItemRect.GetMaxXMinY() - dragMarkerPos;
                dragMarkerSize.y += MarkStroke;
            }
        }
    }

    RectTransform *dragMarkerRT = p_dragMarker->GetRectTransform();
    dragMarkerRT->SetMarginLeftBot(Vector2i(dragMarkerPos));
    dragMarkerRT->SetMarginRightTop(Vector2i(-dragMarkerPos + -dragMarkerSize));
    p_dragMarkerImg->SetImageTexture(markImgAH.Get());
    p_dragMarkerImg->SetMode(markImgMode);

    p_dragMarkerImg->SetTint(Color::Blue());
}

void UIList::OnDrop(EventEmitter<IEventsDragDrop> *dd_)
{
    IEventsDragDrop::OnDrop(dd_);

    if (!GetDragDropEnabled())
    {
        return;
    }

    UIDragDroppable *dragDroppable = DCAST<UIDragDroppable *>(dd_);
    if (GetGameObject()->GetRectTransform()->IsMouseOver(false))
    {
        GOItem *itemOver = nullptr;
        MouseItemRelativePosition relPos;
        GetMousePositionInList(&itemOver, &relPos);

        int newIndex = 0;
        switch (relPos)
        {
            case MouseItemRelativePosition::ABOVE:
                newIndex = GetItems().IndexOf(itemOver);
                break;

            case MouseItemRelativePosition::OVER:
            case MouseItemRelativePosition::BELOW:
                newIndex = GetItems().IndexOf(itemOver) + 1;
                break;

            case MouseItemRelativePosition::BELOW_ALL:
                newIndex = GetItems().Size();
                break;
        }

        if (UIListItemContainer *draggedItemCont =
                GetItemContainer(dragDroppable->GetGameObject()))
        {
            if (GetItems().Contains(draggedItemCont->GetContainedGameObject()))
            {
                MoveItem(p_itemGoBeingDragged->GetContainedGameObject(),
                         newIndex);
                for (GameObject *item : GetItems())
                {
                    if (item != GetSelectedItem())
                    {
                        GetItemBg(item)->SetTint(GetIdleColor());
                    }
                }
            }
        }
    }
    else
    {
        EventEmitter<IEventsUIList>::PropagateToListeners(
            &IEventsUIList::OnDropOutside, dragDroppable);
    }

    p_itemGoBeingDragged = nullptr;
    p_dragMarker->SetParent(nullptr);
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

UIListItemContainer *UIList::GetItemContainer(GameObject *itemGo) const
{
    if (itemGo)
    {
        if (UIListItemContainer *itemCont =
                DCAST<UIListItemContainer *>(itemGo))
        {
            return itemCont;
        }

        if (GameObject *parent = itemGo->GetParent())
        {
            return DCAST<UIListItemContainer *>(parent);
        }
    }
    return nullptr;
}

bool UIList::GetDragDropEnabled() const
{
    return m_dragDropEnabled;
}

void UIList::SetOverColor(const Color &overColor)
{
    m_overColor = overColor;
}

void UIList::SetSelectedColor(const Color &selectedColor)
{
    m_selectedColor = selectedColor;
}

void UIList::SetDragDropEnabled(bool dragDropEnabled)
{
    m_dragDropEnabled = dragDropEnabled;
}

void UIList::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    if (object == p_itemUnderMouse)
    {
        if (UIImageRenderer *itemBg = GetItemBg(p_itemUnderMouse))
        {
            itemBg->SetTint(GetIdleColor());
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

    UIList *uiList = go->AddComponent<UIList>();
    go->SetName("UIList");

    uiList->GetGameObject()->AddComponent<UIVerticalLayout>();

    GameObject *container =
        withScrollPanel ? GameObjectFactory::CreateUIGameObject() : go;
    container->SetName("UIList");

    UIVerticalLayout *containerVL = container->AddComponent<UIVerticalLayout>();
    containerVL->SetChildrenVerticalStretch(Stretch::NONE);
    containerVL->SetChildrenHorizontalStretch(Stretch::FULL);
    uiList->p_dirLayout = containerVL;

    uiList->p_focusable = container->AddComponent<UIFocusable>();

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

        uiList->p_scrollPanel = scrollPanel;
    }
    else
    {
        uiList->p_scrollPanel = nullptr;
    }

    GameObject *dragMarker = GameObjectFactory::CreateUIGameObject();
    UIImageRenderer *dragMarkerImg =
        dragMarker->AddComponent<UIImageRenderer>();
    dragMarker->GetRectTransform()->TranslateLocal(Vector3(0.0f, 0.0f, -0.4f));
    dragMarker->GetRectTransform()->SetAnchors(-Vector2::One());

    uiList->p_dragMarker = dragMarker;
    uiList->p_dragMarkerImg = dragMarkerImg;

    uiList->p_container = container;

    return uiList;
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
