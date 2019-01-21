#include "Bang/UIListItemContainer.h"

#include "Bang/GameObjectFactory.h"
#include "Bang/UIHorizontalLayout.h"

using namespace Bang;

UIListItemContainer::UIListItemContainer()
{
    GameObjectFactory::CreateUIGameObjectInto(this);

    UIHorizontalLayout *hl = AddComponent<UIHorizontalLayout>();
}

UIListItemContainer::~UIListItemContainer()
{
}

void UIListItemContainer::SetContainedGameObject(GameObject *go)
{
    p_containedGameObject = go;
    p_containedGameObject->SetParent(this);
}
