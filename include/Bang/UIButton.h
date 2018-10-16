#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIButtonBase.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;

class UIButton : public UIButtonBase
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIButton)

public:
    virtual void Click() override;

protected:
    virtual void UpdateAspect() override;

private:
    UIButton();
    virtual ~UIButton();

    static UIButton *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UIBUTTON_H
