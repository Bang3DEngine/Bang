#ifndef UIHORIZONTALLAYOUT_H
#define UIHORIZONTALLAYOUT_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIDirLayout.h"

namespace Bang
{
class UIHorizontalLayout : public UIDirLayout
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIHorizontalLayout)

public:
    UIHorizontalLayout();
};
}

#endif  // UIHORIZONTALLAYOUT_H
