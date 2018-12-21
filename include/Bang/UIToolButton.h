#ifndef UITOOLBUTTON_H
#define UITOOLBUTTON_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIButtonBase.h"

namespace Bang
{
class GameObject;

class UIToolButton : public UIButtonBase
{
    COMPONENT(UIToolButton)

public:
    void SetOn(bool on);
    bool GetOn() const;

    // UIButtonBase
    virtual void Click() override;

protected:
    virtual void UpdateAspect() override;

private:
    bool m_on = false;

    UIToolButton();
    virtual ~UIToolButton() override;

    static UIToolButton *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};
}

#endif  // UITOOLBUTTON_H
