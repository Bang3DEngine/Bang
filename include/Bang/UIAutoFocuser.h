#ifndef UIAUTOFOCUSER_H
#define UIAUTOFOCUSER_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"

namespace Bang
{
class UIFocusable;

class UIAutoFocuser : public Component
{
    COMPONENT(UIAutoFocuser)

public:
    // Component
    void OnStart() override;
    void OnUpdate() override;

    void SetFocusableToAutoFocus(UIFocusable *focusable);
    UIFocusable *GetFocusableToAutoFocus() const;

private:
    int m_frames = 0;
    UIFocusable *p_focusableToAutoFocus = nullptr;

    UIAutoFocuser();
    virtual ~UIAutoFocuser() = default;
};
}

#endif  // UIAUTOFOCUSER_H
