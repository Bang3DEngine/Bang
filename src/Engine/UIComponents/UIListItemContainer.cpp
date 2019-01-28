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
    SetName("UIListItemContainer");

    UIHorizontalLayout *hl = AddComponent<UIHorizontalLayout>();
    hl->SetChildrenVerticalStretch(Stretch::FULL);
    hl->SetChildrenHorizontalStretch(Stretch::FULL);

    UILayoutElement *le = AddComponent<UILayoutElement>();
    le->SetFlexibleWidth(99999.0f);

    p_focusable = AddComponent<UIFocusable>();
    GetFocusable()->SetCursorType(Cursor::Type::HAND);

    p_dragDroppable = AddComponent<UIDragDroppable>();
    p_dragDroppable->SetShowDragDropGameObject(false);
    p_dragDroppable->SetFocusable(p_focusable);
}

UIListItemContainer::~UIListItemContainer()
{
}

void UIListItemContainer::SetContainedGameObject(GameObject *go)
{
    p_containedGameObject = go;
    p_containedGameObject->SetParent(this);
}

UIFocusable *UIListItemContainer::GetFocusable() const
{
    return p_focusable;
}

UIDragDroppable *UIListItemContainer::GetDragDroppable() const
{
    return p_dragDroppable;
}

GameObject *UIListItemContainer::GetContainedGameObject() const
{
    return p_containedGameObject;
}
