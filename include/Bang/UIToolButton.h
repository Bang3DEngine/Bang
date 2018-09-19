#ifndef UITOOLBUTTON_H
#define UITOOLBUTTON_H

#include "Bang/UIButtonBase.h"

NAMESPACE_BANG_BEGIN

class UIToolButton : public UIButtonBase
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIToolButton)

public:

    void SetOn(bool on);
    bool GetOn() const;

private:
    bool m_on = false;

    UIToolButton();
    virtual ~UIToolButton();

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

    static UIToolButton *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UITOOLBUTTON_H

