#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "Bang/UIButtonBase.h"

NAMESPACE_BANG_BEGIN

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
