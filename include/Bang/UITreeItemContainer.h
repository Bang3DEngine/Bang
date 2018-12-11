#ifndef UITREEITEMCONTAINER_H
#define UITREEITEMCONTAINER_H

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/DPtr.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/IEvents.h"
#include "Bang/UIFocusable.h"

namespace Bang
{
class UIButton;
class UIImageRenderer;
class UIDragDroppable;

class UITreeItemContainer : public GameObject
{
public:
    using GOItem = GameObject;

    UITreeItemContainer();
    virtual ~UITreeItemContainer() override;

    void SetCollapsable(bool collapsable);
    void SetCollapsed(bool collapsed);
    void SetContainedItem(GOItem *item);
    void SetIndentation(int indentationPx);
    void SetBeingDragged(bool beingDragged);

    bool IsCollapsed() const;
    int GetIndentationPx() const;
    GOItem *GetContainedItem() const;
    UIFocusable *GetFocusable() const;
    UIButton *GetCollapseButton() const;
    UIDragDroppable *GetDragDroppable() const;

private:
    int m_indentationPx = 0;
    bool m_collapsed = false;
    GOItem *p_containedGameObject = nullptr;

    UIImageRenderer *p_dragBg = nullptr;
    DPtr<UIFocusable> p_defaultFocusable;
    GameObject *p_userItemContainer = nullptr;
    UIButton *p_collapseButton = nullptr;
    GameObject *p_indentSpacer = nullptr;
    UIDragDroppable *p_dragDroppable = nullptr;
};
}

#endif  // UITREEITEMCONTAINER_H
