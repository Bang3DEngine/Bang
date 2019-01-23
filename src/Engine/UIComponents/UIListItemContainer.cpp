#include "Bang/UIListItemContainer.h"

#include "Bang/GameObjectFactory.h"
#include "Bang/UIDragDroppable.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UILayoutElement.h"

using namespace Bang;

UIListItemContainer::UIListItemContainer()
{
    GameObjectFactory::CreateUIGameObjectInto(this);

    UIHorizontalLayout *hl = AddComponent<UIHorizontalLayout>();
    hl->SetChildrenVerticalStretch(Stretch::FULL);
    hl->SetChildrenHorizontalStretch(Stretch::FULL);

    UILayoutElement *le = AddComponent<UILayoutElement>();
    le->SetFlexibleWidth(99999.0f);

    p_defaultFocusable = AddComponent<UIFocusable>();

    p_dragDroppable = AddComponent<UIDragDroppable>();
    p_dragDroppable->SetShowDragDropGameObject(false);
    p_dragDroppable->SetFocusable(p_defaultFocusable);
}

UIListItemContainer::~UIListItemContainer()
{
}

void UIListItemContainer::SetContainedGameObject(GameObject *go)
{
    p_containedGameObject = go;
    p_containedGameObject->SetParent(this);
}

GameObject *UIListItemContainer::GetContainedGameObject() const
{
    return p_containedGameObject;
}
