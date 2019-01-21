#ifndef UILISTITEMCONTAINER_H
#define UILISTITEMCONTAINER_H

#include "Bang/Bang.h"
#include "Bang/GameObject.h"

namespace Bang
{
class UIListItemContainer : public GameObject
{
public:
    UIListItemContainer();
    virtual ~UIListItemContainer() override;

    void SetContainedGameObject(GameObject *go);

private:
    GameObject *p_containedGameObject = nullptr;
};
}

#endif  // UILISTITEMCONTAINER_H
