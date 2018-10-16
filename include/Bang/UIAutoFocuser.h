#ifndef UIAUTOFOCUSER_H
#define UIAUTOFOCUSER_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class UIFocusable;

class UIAutoFocuser : public Component
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIAutoFocuser)

public:
    // Component
    void OnStart() override;
    void OnUpdate() override;

    void SetFocusableToAutoFocus(UIFocusable* focusable);
    UIFocusable* GetFocusableToAutoFocus() const;

private:
    int m_frames = 0;
    UIFocusable* p_focusableToAutoFocus = nullptr;

    UIAutoFocuser();
    virtual ~UIAutoFocuser() = default;
};

NAMESPACE_BANG_END

#endif // UIAUTOFOCUSER_H

