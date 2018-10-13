#ifndef UITREE_H
#define UITREE_H

#include "Bang/List.h"
#include "Bang/Tree.h"
#include "Bang/UMap.h"
#include "Bang/UIList.h"
#include "Bang/UIButton.h"
#include "Bang/Component.h"
#include "Bang/GameObject.h"
#include "Bang/IEventsFocus.h"
#include "Bang/IEventsUITree.h"
#include "Bang/IEventsDragDrop.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIDragDroppable;
FORWARD class UITreeItemContainer;

class UITree : public Component,
               public EventListener<IEventsFocus>,
               public EventListener<IEventsDestroy>,
               public EventListener<IEventsDragDrop>,
               public EventEmitter<IEventsUITree>
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UITree)

public:
    enum class MouseItemRelativePosition
    {
        ABOVE,
        OVER,
        BELOW,
        BELOW_ALL
    };

    void AddItem(GOItem *newItem,
                 GOItem *parentItem,
                 int indexInsideParent = 0);
    void MoveItem(GOItem *item,
                  GOItem *newParentItem,
                  int newIndexInsideParent = 0);
    void RemoveItem(GOItem *itemToRemove);
    GOItem* GetSelectedItem() const;
    void Clear();

    void SetSelection(GOItem *item);
    void SetItemCollapsed(GOItem *item, bool collapsed);
    void SetSelectionCallback(UIList::SelectionCallback callback);

    GOItem* GetParentItem(GOItem *item) const;
    bool IsItemCollapsed(GOItem *item) const;
    List<GOItem*> GetTopChildrenItems() const;
    bool Contains(UITreeItemContainer *item) const;
    List<GOItem*> GetChildrenItems(GOItem *item) const;
    bool ItemIsChildOfRecursive(GOItem *item, GOItem *parent) const;
    UIList* GetUIList() const;

    void GetMousePositionInTree(
                    GOItem **itemOverOut,
                    UITree::MouseItemRelativePosition *itemRelPosOut) const;
    int GetFlatUIListIndex(GOItem *parentItem, int indexInsideParent);

    // Component
    void OnUpdate() override;

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;


    // IEventsDragDrop
    virtual void OnDragStarted(EventEmitter<IEventsDragDrop> *dragDroppable) override;
    virtual void OnDragUpdate(EventEmitter<IEventsDragDrop> *dragDroppable) override;
    virtual void OnDrop(EventEmitter<IEventsDragDrop> *dragDroppable) override;

    // IEventsDestroy
    void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

protected:
    UITree();
    virtual ~UITree();

    static UITree* CreateInto(GameObject *go);

private:
    static const int IndentationPx;

    Tree<GOItem*> m_rootTree;
    UIList *p_uiList = nullptr;
    GOItem *p_itemBeingDragged = nullptr;
    UMap<GOItem*, Tree<GOItem*>*> m_itemToTree;
    UIList::SelectionCallback m_selectionCallback;

    GameObject *p_dragMarker = nullptr;
    UIImageRenderer *p_dragMarkerImg = nullptr;

    GOItem* AddItem_(GOItem* newItemTree,
                     GOItem *parentItem,
                     int indexInsideParent,
                     bool moving);
    GOItem* AddItem_(const Tree<GOItem*> &newItemTree,
                     GOItem *parentItem,
                     int indexInsideParent,
                     bool moving);
    void RemoveItem_(GOItem *itemToRemove, bool moving);
    bool NeedsToBeEnabled(GOItem *item, bool recursive);
    void UpdateCollapsabilityOnThisAndDescendants(GOItem *item);
    void IndentItem(GOItem *item);
    bool IsValidDrag(UIDragDroppable *dd,
                     GOItem *itemBeingDragged,
                     GOItem *itemOver) const;

    Tree<GOItem*>* GetItemTree(GOItem* item) const;
    UITreeItemContainer* GetTreeItemContainer(GOItem *item) const;

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UITREE_H

