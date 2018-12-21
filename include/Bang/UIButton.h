#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIButtonBase.h"

namespace Bang
{
class GameObject;

class UIButton : public UIButtonBase
{
    COMPONENT(UIButton)

public:
    virtual void Click() override;

protected:
    virtual void UpdateAspect() override;

private:
    UIButton();
    virtual ~UIButton() override;

    static UIButton *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};
}

#endif  // UIBUTTON_H
