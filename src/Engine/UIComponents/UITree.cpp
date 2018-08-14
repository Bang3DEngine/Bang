#include "Bang/UITree.h"

#include "Bang/Debug.h"
#include "Bang/Input.h"
#include "Bang/Scene.h"
#include "Bang/UIButton.h"
#include "Bang/UICanvas.h"
#include "Bang/RectTransform.h"
#include "Bang/TextureFactory.h"
#include "Bang/UIDragDroppable.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"

USING_NAMESPACE_BANG

const int UITree::IndentationPx = 10;

UITree::UITree()
{
    m_rootTree.SetData(nullptr);
}

UITree::~UITree()
{
}

void UITree::OnUpdate()
{
    Component::OnUpdate();

    // Collapse with left-right buttons
    if (GetUIList()->SomeChildHasFocus())
    {
        int collapseOnOff = 0;
        if (Input::GetKeyDownRepeat(Key::LEFT)) { collapseOnOff = -1; }
        else if (Input::GetKeyDownRepeat(Key::RIGHT)) { collapseOnOff = 1; }
        if (collapseOnOff != 0)
        {
            GOItem *selItemCont = GetUIList()->GetSelectedItem();
            if (selItemCont)
            {
                UITreeItemContainer *selectedItemCont =
                                        Cast<UITreeItemContainer*>(selItemCont);
                GOItem *selectedItem = selectedItemCont->GetContainedItem();

                bool isCollapsed = selectedItemCont->IsCollapsed();
                int numChildren = GetItemTree(selectedItem)->GetChildren().Size();

                int newSelIndex = GetUIList()->GetSelectedIndex();

                if ( (numChildren == 0 || !isCollapsed) && collapseOnOff == 1)
                { ++newSelIndex; }

                else if ( (numChildren == 0 || isCollapsed) && collapseOnOff == -1)
                { --newSelIndex; }

                if (newSelIndex == GetUIList()->GetSelectedIndex())
                {
                    // Normal Collapse/UnCollapse
                    GOItem *selectedItem = selectedItemCont->GetContainedItem();
                    SetItemCollapsed(selectedItem, (collapseOnOff == -1) );
                }
                else
                {
                    // "Redundant" Collapse/UnCollapse. Go Up or Down
                    newSelIndex = Math::Clamp(newSelIndex,
                                              0, GetUIList()->GetNumItems()-1);
                    GetUIList()->SetSelection(newSelIndex);
                }
            }
        }
    }
}

void UITree::OnEvent(IFocusable *focusable, const UIEvent &event)
{
    if (event.type == UIEvent::Type::MOUSE_CLICK_FULL)
    {
        IFocusable *collapseButton = DCAST<IFocusable*>(focusable);
        Component *cCollapseButton = DCAST<Component*>(collapseButton);
        UITreeItemContainer *itemContainer =
                   SCAST<UITreeItemContainer*>(cCollapseButton->GetGameObject()->
                                               GetParent());
        if (itemContainer)
        {
            SetItemCollapsed(itemContainer->GetContainedItem(),
                             !itemContainer->IsCollapsed());
        }
    }
}

void UITree::OnDragStarted(EventEmitter<IEventsDragDrop> *dd_)
{
    IEventsDragDrop::OnDragStarted(dd_);

    UIDragDroppable *dd = DCAST<UIDragDroppable*>(dd_);
    p_itemBeingDragged = SCAST<UITreeItemContainer*>(dd->GetGameObject())->
                                                        GetContainedItem();
    p_dragMarker->SetParent( GetGameObject()->GetScene() );
}

void UITree::OnDragUpdate(EventEmitter<IEventsDragDrop> *dd_)
{
    IEventsDragDrop::OnDragUpdate(dd_);

    GOItem *childItemOver = nullptr;
    MouseItemRelativePosition markPosition = MouseItemRelativePosition::ABOVE;
    GetMousePositionInTree(&childItemOver, &markPosition);

    RH<Texture2D> markImgRH;
    constexpr int MarkStroke = 2;
    Vector2 dragMarkerPos, dragMarkerSize;
    UIImageRenderer::Mode markImgMode = UIImageRenderer::Mode::TEXTURE_INV_UVY;
    if (childItemOver)
    {
        UITreeItemContainer *childItemCont = GetItemContainer(childItemOver);
        RectTransform *childItemContRT = childItemCont->GetRectTransform();
        AARect childItemContRect = childItemContRT->GetViewportAARect();

        const AARect contCollapseButtonRect = childItemCont->GetCollapseButton()->
                                              GetGameObject()->GetRectTransform()->
                                              GetViewportAARect();
        switch (markPosition)
        {
            case MouseItemRelativePosition::ABOVE:
                dragMarkerPos.x   = contCollapseButtonRect.GetMax().x;
                dragMarkerPos.y   = contCollapseButtonRect.GetMax().y;
                dragMarkerSize    = childItemContRect.GetMaxXMaxY() - dragMarkerPos;
                dragMarkerSize.y += MarkStroke;
            break;

            case MouseItemRelativePosition::OVER:
                dragMarkerPos  = childItemContRect.GetMinXMinY();
                dragMarkerSize = childItemContRect.GetMax() - dragMarkerPos;
                markImgRH.Set(TextureFactory::Get9SliceRoundRectBorderTexture());
                markImgMode    = UIImageRenderer::Mode::SLICE_9;
            break;

            case MouseItemRelativePosition::BELOW:
                dragMarkerPos.x   = contCollapseButtonRect.GetMax().x;
                dragMarkerPos.y   = contCollapseButtonRect.GetMin().y;
                dragMarkerSize    = childItemContRect.GetMaxXMinY() - dragMarkerPos;
                dragMarkerSize.y += MarkStroke;
            break;

            default: break;
        }
    }
    else
    {
        if (markPosition == MouseItemRelativePosition::BELOW_ALL)
        {
            GameObject *lastItem = GetUIList()->GetItems().Back();
            AARect lastItemRect = lastItem->GetRectTransform()->GetViewportAARect();
            dragMarkerPos     = lastItemRect.GetMinXMinY();
            dragMarkerSize    = lastItemRect.GetMaxXMinY() - dragMarkerPos;
            dragMarkerSize.y += MarkStroke;
        }
    }

    RectTransform *dragMarkerRT = p_dragMarker->GetRectTransform();
    dragMarkerRT->SetMarginLeftBot( Vector2i(dragMarkerPos) );
    dragMarkerRT->SetMarginRightTop( Vector2i(-dragMarkerPos + -dragMarkerSize) );
    p_dragMarkerImg->SetImageTexture(markImgRH.Get());
    p_dragMarkerImg->SetMode(markImgMode);

    bool isValidDrag = IsValidDrag(p_itemBeingDragged, childItemOver);
    p_dragMarkerImg->SetTint(isValidDrag ? Color::Blue : Color::Red);
}

void UITree::OnDrop(EventEmitter<IEventsDragDrop> *dd_)
{
    IEventsDragDrop::OnDrop(dd_);

    UIDragDroppable *dragDroppable = DCAST<UIDragDroppable*>(dd_);
    UITreeItemContainer *draggedItemCont = DCAST<UITreeItemContainer*>(
                                              dragDroppable->GetGameObject());
    if ( !Contains(draggedItemCont) )
    {
        return;
    }

    GOItem *itemOver = nullptr;
    MouseItemRelativePosition relPos;
    GetMousePositionInTree(&itemOver, &relPos);

    GOItem *newParentItem = nullptr;
    int newIndexInParent = 0;
    switch (relPos)
    {
        case MouseItemRelativePosition::ABOVE:
            newParentItem = GetParentItem(itemOver);
            newIndexInParent = GetChildrenItems(newParentItem).IndexOf(itemOver);
        break;

        case MouseItemRelativePosition::OVER:
            newParentItem = itemOver;
            newIndexInParent = -1;
        break;

        case MouseItemRelativePosition::BELOW:
            newParentItem = GetParentItem(itemOver);
            newIndexInParent = GetChildrenItems(newParentItem).IndexOf(itemOver) + 1;
        break;

        case MouseItemRelativePosition::BELOW_ALL:
            newParentItem = nullptr;
            newIndexInParent = m_rootTree.GetChildren().Size();
        break;
    }

    if (p_itemBeingDragged && IsValidDrag(p_itemBeingDragged, itemOver))
    {
        MoveItem(p_itemBeingDragged, newParentItem, newIndexInParent);
    }

    p_itemBeingDragged = nullptr;
    p_dragMarker->SetParent(nullptr);
}

void UITree::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    GOItem* item = SCAST<GOItem*>(object);
    RemoveItem(item);
}

List<GOItem*> UITree::GetTopChildrenItems() const
{
    List<GOItem*> childrenItems;
    for (const auto &pair : m_itemToTree)
    {
        childrenItems.PushBack(pair.first);
    }
    return childrenItems;
}

List<GOItem*> UITree::GetChildrenItems(GOItem *item) const
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    List<GOItem*> childrenItems;
    if ( m_itemToTree.ContainsKey(item) || !item )
    {
        const Tree<GOItem*>* itemTree = item ? m_itemToTree.Get(item) : &m_rootTree;
        const List<Tree<GOItem*>*>& childrenTrees = itemTree->GetChildren();
        for (const Tree<GOItem*>* childTree : childrenTrees)
        {
            childrenItems.PushBack(childTree->GetData());
        }
    }
    return childrenItems;
}

bool UITree::ItemIsChildOfRecursive(GOItem *item, GOItem *parent) const
{
    if (!item) { return false; }

    GOItem *actualItemParent = GetParentItem(item);
    return (parent == actualItemParent) ||
            ItemIsChildOfRecursive(actualItemParent, parent);
}

void UITree::AddItem(GOItem *newItem, GOItem *parentItem, int indexInsideParent)
{
    AddItem_(newItem, parentItem, indexInsideParent, false);
}

void UITree::MoveItem(GOItem *itemToMove, GOItem *newParentItem,
                      int newIndexInsideParent_)
{
    ASSERT( !itemToMove || !DCAST<UITreeItemContainer*>(itemToMove) );
    ASSERT (!ItemIsChildOfRecursive(newParentItem, itemToMove));

    Tree<GOItem*> *newParentItemTree = GetItemTree(newParentItem);
    int numParentChildren = newParentItemTree->GetChildren().Size();
    int newIndexInsideParent = newIndexInsideParent_ < 0 ? numParentChildren :
                    Math::Clamp(newIndexInsideParent_, 0, numParentChildren);

    GOItem *oldParentItem = GetParentItem(itemToMove);
    Tree<GOItem*> *oldParentItemTree = GetItemTree(oldParentItem);

    Tree<GOItem*> *itemTree = GetItemTree(itemToMove);
    int oldIndexInParent = oldParentItemTree->GetChildren().IndexOf(itemTree);
    ASSERT(oldIndexInParent >= 0);

    if (oldParentItem == newParentItem)
    {
        if (oldIndexInParent < newIndexInsideParent)
        {
            newIndexInsideParent -= 1;
        }
    }

    Tree<GOItem*> *itemTreeCpy = itemTree->GetDeepCopy();

    RemoveItem_(itemToMove, true); // This recursively removes the items
    AddItem_(*itemTreeCpy, newParentItem, newIndexInsideParent, true);

    delete itemTreeCpy;

    EventEmitter<IEventsUITree>::PropagateToListeners(
                &IEventsUITree::OnItemMoved, itemToMove,
                oldParentItem, oldIndexInParent,
                newParentItem, newIndexInsideParent);
}

void UITree::RemoveItem(GOItem *item)
{
    RemoveItem_(item, false);
}

void UITree::AddItem_(GOItem* newItem, GOItem *parentItem,
                      int indexInsideParent, bool moving)
{
    ASSERT( !newItem || !DCAST<UITreeItemContainer*>(newItem));

    Tree<GOItem*> *parentTree = GetItemTree(parentItem);
    int newIndexInsideParent = indexInsideParent >= 0 ? indexInsideParent :
                (parentItem ? parentTree->GetChildren().Size() : 0);
    if (parentTree && !m_itemToTree.ContainsKey(newItem))
    {
        int newItemFlatListIndex = GetFlatUIListIndex(parentItem,
                                                      newIndexInsideParent);

        // Create itemContainer and populate
        UITreeItemContainer* newItemContainer = GameObject::Create<UITreeItemContainer>();
        newItemContainer->SetContainedItem(newItem);

        // Add
        ASSERT(newIndexInsideParent >= 0);
        Tree<GOItem*> *childTree = parentTree->AddChild(newItem, newIndexInsideParent);
        m_itemToTree.Add(newItem, childTree); // Add to item_tree map
        GetUIList()->AddItem(newItemContainer, newItemFlatListIndex); // Add to UIList

        // Listen to focus and destroying
        newItemContainer->GetCollapseButton()->GetFocusable()->
                EventEmitter<IEventsFocus>::RegisterListener(this);
        newItem->EventEmitter<IEventsDestroy>::RegisterListener(this);

        // Update collapsabilities
        UpdateCollapsability(newItem);
        if (parentItem) { UpdateCollapsability(parentItem); }

        IndentItem(newItem); // Indent

        if (!moving)
        {
            EventEmitter<IEventsUITree>::PropagateToListeners(
                        &IEventsUITree::OnItemAdded, newItem, parentItem,
                        indexInsideParent);
        }
    }
    else
    {
        Debug_Warn("Can't add item " << newItem << " to " << parentItem <<
                   " because it does not exist");
    }
}

void UITree::AddItem_(const Tree<GOItem*> &newItemTree, GOItem *parentItem,
                      int indexInsideParent, bool moving)
{
    GOItem *rootItem = newItemTree.GetData();
    AddItem_(rootItem, parentItem, indexInsideParent, moving);

    // Add children items
    for (Tree<GOItem*> *childTree : newItemTree.GetChildren())
    {
        AddItem_(*childTree, rootItem, 0, moving);
    }
}

void UITree::RemoveItem_(GOItem *item, bool moving)
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    Tree<GOItem*> *itemTree = GetItemTree(item);
    if (itemTree)
    {
        // Get some info before deleting
        GOItem* parentItem = GetParentItem(item);

        List< Tree<GOItem*>* > treeChildrenRec = itemTree->GetChildrenRecursive();
        treeChildrenRec.PushBack(itemTree);

        Array<GOItem*> itemsToRemove;
        Array<UITreeItemContainer*> itemsContToRemove;
        for (Tree<GOItem*>* treeChild : treeChildrenRec)
        {
            GOItem *itemToRemove = treeChild->GetData();
            UITreeItemContainer *itemCont = GetItemContainer(itemToRemove);
            itemsContToRemove.PushBack(itemCont);
            itemsToRemove.PushBack(itemToRemove);
            itemToRemove->SetParent(nullptr);
        }

        // Remove needed stuff from list, remove from map, and delete the item
        // tree (which recursively deletes its children)
        for (int i = 0; i < itemsToRemove.Size(); ++i)
        {
            GOItem *itemToRemove = itemsToRemove[i];
            UITreeItemContainer* itemContToRemove = itemsContToRemove[i];
            m_itemToTree.Remove(itemToRemove);
            GetUIList()->RemoveItem(itemContToRemove);
        }
        delete itemTree;

        if (!moving)
        {
            EventEmitter<IEventsUITree>::PropagateToListeners(
                        &IEventsUITree::OnItemRemoved, item);
        }

        // Update parent collapsability
        if (parentItem) { UpdateCollapsability(parentItem); }
    }
}

GOItem *UITree::GetSelectedItem() const
{
    GOItem *selectedItem = GetUIList()->GetSelectedItem();
    if (selectedItem)
    {
        UITreeItemContainer *selectedItemCont =
                                Cast<UITreeItemContainer*>(selectedItem);
        return selectedItemCont->GetContainedItem();
    }
    return nullptr;
}

void UITree::Clear()
{
    while (!m_rootTree.GetChildren().IsEmpty())
    {
        RemoveItem(m_rootTree.GetChildren().Front()->GetData());
    }
    GetUIList()->Clear();
    m_rootTree.Clear();
    m_itemToTree.Clear();
}

void UITree::SetSelection(GOItem *item)
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    UnCollapseUpwards( GetParentItem(item) );
    UITreeItemContainer *itemContainer = GetItemContainer(item);
    if (itemContainer)
    {
        itemContainer->SetEnabled(true);
        SetItemCollapsed(item, itemContainer->IsCollapsed());
    }
    GetUIList()->SetSelection(itemContainer);
}

void UITree::SetItemCollapsed(GOItem *item, bool collapse)
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    if (!collapse) { UnCollapseUpwards(item); }

    GetItemContainer(item)->SetCollapsed(collapse);
    SetItemCollapsedRecursive(item, collapse);
}

void UITree::SetItemCollapsedRecursive(GOItem *item, bool collapse)
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    List<GOItem*> childrenItems = GetChildrenItems(item);
    for (GOItem *childItem : childrenItems)
    {
        UITreeItemContainer *childItemContainer = GetItemContainer(childItem);
        childItemContainer->SetEnabled(!collapse);
        SetItemCollapsedRecursive(childItem,
                      (collapse || childItemContainer->IsCollapsed()));
    }
}

void UITree::UnCollapseUpwards(GOItem *item)
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    if (!item) { return; }

    UITreeItemContainer *itemContainer = GetItemContainer(item);
    itemContainer->SetEnabled(true);
    itemContainer->SetCollapsed(false);
    UnCollapseUpwards( GetParentItem(item) );
}

void UITree::SetSelectionCallback(UIList::SelectionCallback callback)
{
    m_selectionCallback = callback;
}

bool UITree::IsItemCollapsed(GOItem *item) const
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );
    return GetItemContainer(item)->IsCollapsed();
}

UIList *UITree::GetUIList() const
{
    return p_uiList;
}

void UITree::GetMousePositionInTree(
                    GOItem **itemOverOut,
                    UITree::MouseItemRelativePosition *itemRelPosOut) const
{
    if (GetGameObject()->GetRectTransform()->IsMouseOver())
    {
        if (!GetUIList()->GetItems().IsEmpty())
        {
            bool foundItemOver = false;
            const Vector2i mousePos = Input::GetMousePosition();
            for (int i = 0; i < GetUIList()->GetNumItems(); ++i)
            {
                GameObject *itChildItemCont = GetUIList()->GetItems()[i];
                UITreeItemContainer *childItemCont =
                                 SCAST<UITreeItemContainer*>(itChildItemCont);
                if (!childItemCont->IsActive()) { continue; }

                RectTransform *childItemContRT = childItemCont->GetRectTransform();
                AARect childItemContRect = childItemContRT->GetViewportAARect();
                if (childItemContRect.Contains( Vector2(mousePos) ))
                {
                    constexpr int BoundaryEps = 4;
                    if (mousePos.y >= childItemContRect.GetMax().y - BoundaryEps)
                    {
                        *itemRelPosOut = MouseItemRelativePosition::ABOVE;
                    }
                    else if (mousePos.y <= childItemContRect.GetMin().y + BoundaryEps)
                    {
                        *itemRelPosOut = MouseItemRelativePosition::BELOW;
                    }
                    else
                    {
                        *itemRelPosOut = MouseItemRelativePosition::OVER;
                    }

                    UITreeItemContainer *childItemCont =
                                    SCAST<UITreeItemContainer*>(itChildItemCont);
                    *itemOverOut = childItemCont->GetContainedItem();
                    foundItemOver = true;
                    break;
                }
            }

            if (!foundItemOver && !GetUIList()->GetItems().IsEmpty())
            {
                UITreeItemContainer *firstItemCont =
                  SCAST<UITreeItemContainer*>(GetUIList()->GetItems().Front());
                RectTransform *firstItemContRT = firstItemCont->GetRectTransform();
                if (mousePos.y >= firstItemContRT->GetViewportAARect().GetMax().y)
                {
                    *itemOverOut = firstItemCont->GetContainedItem(); // Above all items
                    *itemRelPosOut = MouseItemRelativePosition::ABOVE;
                }
                else // Below all items
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

int UITree::GetFlatUIListIndex(GOItem *parentItem, int indexInsideParent)
{
    Tree<GOItem*> *parentTree = GetItemTree(parentItem);
    int newItemFlatListIndex;
    if (!parentTree->GetChildren().IsEmpty() &&
        indexInsideParent > 0 &&
        indexInsideParent <= int(parentTree->GetChildren().Size()))
    {
        auto it = parentTree->GetChildren().Begin();
        std::advance(it, indexInsideParent - 1);
        GOItem* siblingItem = (*it)->GetData();
        Tree<GOItem*> *siblingTree = GetItemTree(siblingItem);
        UITreeItemContainer *siblingItemCont = GetItemContainer(siblingItem);

        newItemFlatListIndex = GetUIList()->GetItems().IndexOf(siblingItemCont);
        newItemFlatListIndex += 1;
        newItemFlatListIndex += siblingTree->GetChildrenRecursive().Size();
    }
    else
    {
        UITreeItemContainer *parentItemContainer = GetItemContainer(parentItem);
        int parentItemIndex = GetUIList()->GetItems().IndexOf(parentItemContainer);
        newItemFlatListIndex = parentItemIndex + 1;
    }
    return newItemFlatListIndex;
}

UITree *UITree::CreateInto(GameObject *go)
{
    UIList *list = GameObjectFactory::CreateUIListInto(go);

    UITree *uiTree = go->AddComponent<UITree>();
    uiTree->p_uiList = list;

    GameObject *dragMarker = GameObjectFactory::CreateUIGameObject();;
    UIImageRenderer *dragMarkerImg = dragMarker->AddComponent<UIImageRenderer>();
    dragMarker->GetRectTransform()->TranslateLocal( Vector3(0.0f, 0.0f, -0.4f) );
    dragMarker->GetRectTransform()->SetAnchors( -Vector2::One );

    uiTree->p_dragMarker = dragMarker;
    uiTree->p_dragMarkerImg = dragMarkerImg;

    uiTree->GetUIList()->SetSelectionCallback(
        [uiTree](GOItem *item, UIList::Action action)
        {
            // Forward selectionCallback from itemContainer to actual item
            if (uiTree->m_selectionCallback)
            {
                UITreeItemContainer *itemCont = Cast<UITreeItemContainer*>(item);
                uiTree->m_selectionCallback( itemCont->GetContainedItem(), action);
            }
        }
    );

    return uiTree;
}

UITreeItemContainer *UITree::GetItemContainer(GOItem *item) const
{
    ASSERT(!item || !DCAST<UITreeItemContainer*>(item) );
    return item ? (item->GetParent()->GetParent() ?
       DCAST<UITreeItemContainer*>(item->GetParent()->GetParent()) : nullptr) :
                  nullptr;
}

void UITree::UpdateCollapsability(GOItem *item)
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    Tree<GOItem*> *itemTree = GetItemTree(item);
    if (itemTree)
    {
        bool isLeaf = (itemTree->GetChildren().Size() == 0);
        GetItemContainer(item)->SetCollapsable(!isLeaf);
    }
}

void UITree::IndentItem(GOItem *item)
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    Tree<GOItem*> *itemTree = GetItemTree(item);
    GetItemContainer(item)->SetIndentation(UITree::IndentationPx *
                                           itemTree->GetDepth());
}

bool UITree::Contains(UITreeItemContainer *itemCont) const
{
    return itemCont && GetUIList()->GetItems().Contains(itemCont);
}

bool UITree::IsValidDrag(GOItem *itemBeingDragged, GOItem *itemOver) const
{
    if (!itemBeingDragged) { return false; }

    bool isValidDrag = (itemBeingDragged != itemOver) &&
                       !ItemIsChildOfRecursive(itemOver, itemBeingDragged);
    return isValidDrag;
}

GOItem* UITree::GetParentItem(GOItem *item) const
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item) );

    Tree<GOItem*>* itemTree = GetItemTree(item);
    if (!itemTree) { return nullptr; }

    Tree<GOItem*>* parentTree = itemTree ? itemTree->GetParent() : nullptr;
    if (!parentTree) { return nullptr; }

    return (parentTree != &m_rootTree) ? parentTree->GetData() : nullptr;
}

Tree<GOItem*>* UITree::GetItemTree(GOItem *item) const
{
    ASSERT( !item || !DCAST<UITreeItemContainer*>(item));

    if (!item) { return &((const_cast<UITree*>(this))->m_rootTree); }

    // Make sure item belongs to this UITree
    if (m_itemToTree.ContainsKey(item))  { return m_itemToTree.Get(item); }
    return nullptr;
}



// UITreeItemContainer ================================

UITreeItemContainer::UITreeItemContainer()
{
    GameObjectFactory::CreateUIGameObjectInto(this);

    UIHorizontalLayout *hLayout = AddComponent<UIHorizontalLayout>();
    hLayout->SetChildrenVerticalStretch(Stretch::FULL);

    p_userItemContainer = GameObjectFactory::CreateUIGameObject();

    UIHorizontalLayout *containerHLayout = p_userItemContainer->AddComponent<UIHorizontalLayout>();
    containerHLayout->SetChildrenHorizontalStretch(Stretch::FULL);

    UILayoutElement *containerLE = p_userItemContainer->AddComponent<UILayoutElement>();
    containerLE->SetFlexibleSize(Vector2(99999.0f));

    p_indentSpacer = GameObjectFactory::CreateUISpacer(LayoutSizeType::MIN,
                                                       Vector2::Zero);
    p_indentSpacer->SetName("IndentSpacer");

    Texture2D *iconTex = TextureFactory::GetDownArrowIcon();
    p_collapseButton = GameObjectFactory::CreateUIButton("", iconTex);
    p_collapseButton->GetLayoutElement()->SetFlexibleSize( Vector2::Zero );
    p_collapseButton->GetGameObject()->SetName("CollapseButton");
    p_collapseButton->SetIcon(iconTex, Vector2i(8), 0);
    p_collapseButton->GetBackground()->SetVisible(false);    

    p_dragDroppable = AddComponent<UIDragDroppable>();
    p_dragDroppable->SetShowDragDropGameObject(false);
    p_dragDroppable->SetFocusable(nullptr);

    p_indentSpacer->SetParent(this);
    p_collapseButton->GetGameObject()->SetParent(this);
    p_userItemContainer->SetParent(this);
}

UITreeItemContainer::~UITreeItemContainer()
{

}

void UITreeItemContainer::SetCollapsable(bool collapsable)
{
    GetCollapseButton()->SetEnabled(collapsable);
    GetCollapseButton()->GetIcon()->SetVisible(collapsable);
    GetCollapseButton()->GetFocusable()->SetEnabled(collapsable);
}

void UITreeItemContainer::SetCollapsed(bool collapsed)
{
    if (collapsed != IsCollapsed())
    {
        m_collapsed = collapsed;
        Texture2D *iconTex = IsCollapsed() ? TextureFactory::GetRightArrowIcon() :
                                             TextureFactory::GetDownArrowIcon();
        GetCollapseButton()->SetIconTexture(iconTex);
    }
}

void UITreeItemContainer::SetContainedItem(GOItem *go)
{
    p_containedGameObject = go;
    p_containedGameObject->SetParent(p_userItemContainer);
    SetName("GOItemCont_" + go->GetName());
}

GameObject *UITreeItemContainer::GetContainedItem() const
{
    return p_containedGameObject;
}

void UITreeItemContainer::SetIndentation(int indentationPx)
{
    m_indentationPx = indentationPx;
    p_indentSpacer->GetComponent<UILayoutElement>()->SetMinWidth(indentationPx);
}

bool UITreeItemContainer::IsCollapsed() const
{
    return m_collapsed;
}

int UITreeItemContainer::GetIndentationPx() const
{
    return m_indentationPx;
}

UIButton *UITreeItemContainer::GetCollapseButton() const
{
    return p_collapseButton;
}

UIDragDroppable *UITreeItemContainer::GetDragDroppable() const
{
    return p_dragDroppable;
}
