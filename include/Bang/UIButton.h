#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "Bang/UIButtonBase.h"

NAMESPACE_BANG_BEGIN

class UIButton : public UIButtonBase
{
    COMPONENT(UIButton)

private:
    UIButton();
    virtual ~UIButton();

    // IEventsFocus
    virtual UIEventResult OnUIEvent(UIFocusable *focusable,
                                    const UIEvent &event) override;

    static UIButton *CreateInto(GameObject *go);

    friend class GameObjectFactory;
};

NAMESPACE_BANG_END

#endif // UIBUTTON_H
