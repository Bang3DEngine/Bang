#ifndef UILAYOUTIGNORER_H
#define UILAYOUTIGNORER_H

#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"

namespace Bang
{
class UILayoutIgnorer : public Component
{
    COMPONENT(UILayoutIgnorer)

public:
    UILayoutIgnorer();
    virtual ~UILayoutIgnorer() override;

    void SetIgnoreLayout(bool ignoreLayout);
    bool IsIgnoreLayout() const;

private:
    bool m_ignoreLayout = true;
};
}

#endif  // UILAYOUTIGNORER_H_H
