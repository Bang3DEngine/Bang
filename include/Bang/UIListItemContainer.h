#ifndef UILISTITEMCONTAINER_H
#define UILISTITEMCONTAINER_H

#include "Bang/Bang.h"
#include "Bang/GameObject.h"

namespace Bang
{
class UIFocusable;
class UIDragDroppable;
class UIListItemContainer : public GameObject
{
public:
    UIListItemContainer();
    virtual ~UIListItemContainer() override;

    void SetContainedGameObject(GameObject *go);

    GameObject *GetContainedGameObject() const;

private:
    GameObject *p_containedGameObject = nullptr;
    UIFocusable *p_defaultFocusable = nullptr;
    UIDragDroppable *p_dragDroppable = nullptr;
};
}

#endif  // UILISTITEMCONTAINER_H
