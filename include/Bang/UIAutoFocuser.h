#ifndef UIAUTOFOCUSER_H
#define UIAUTOFOCUSER_H

#include "Bang/Component.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIFocusable;

class UIAutoFocuser : public Component
{
    COMPONENT(UIAutoFocuser)

public:
    // Component
    void OnStart() override;
    void OnUpdate() override;

    void SetFocusableToAutoFocus(UIFocusable* focusable);
    UIFocusable* GetFocusableToAutoFocus() const;

private:
    int m_frames = 0;
    UIFocusable* p_focusableToAutoFocus = nullptr;

    UIAutoFocuser() = default;
    virtual ~UIAutoFocuser() = default;
};

NAMESPACE_BANG_END

#endif // UIAUTOFOCUSER_H

