#ifndef UIVERTICALLAYOUT_H
#define UIVERTICALLAYOUT_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIDirLayout.h"

namespace Bang
{
class UIVerticalLayout : public UIDirLayout
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIVerticalLayout)

public:
    UIVerticalLayout();
};
}

#endif  // UIVERTICALLAYOUT_H
