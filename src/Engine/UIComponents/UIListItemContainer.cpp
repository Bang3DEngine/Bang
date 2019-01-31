#include "Bang/UIListItemContainer.h"

#include "Bang/GameObjectFactory.h"
#include "Bang/UIDragDroppable.h"
#include "Bang/UIFocusable.h"
#include "Bang/UIHorizontalLayout.h"
#include "Bang/UIImageRenderer.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UITheme.h"

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

    p_bg = AddComponent<UIImageRenderer>();
    p_bg->SetTint(Color::White());

    p_focusable = AddComponent<UIFocusable>();

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

UIImageRenderer *UIListItemContainer::GetBackground() const
{
    return p_bg;
}

UIDragDroppable *UIListItemContainer::GetDragDroppable() const
{
    return p_dragDroppable;
}

GameObject *UIListItemContainer::GetContainedGameObject() const
{
    return p_containedGameObject;
}
