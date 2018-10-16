#ifndef UIHORIZONTALLAYOUT_H
#define UIHORIZONTALLAYOUT_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIDirLayout.h"

NAMESPACE_BANG_BEGIN

class UIHorizontalLayout : public UIDirLayout
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIHorizontalLayout)

public:
    UIHorizontalLayout();
};

NAMESPACE_BANG_END

#endif // UIHORIZONTALLAYOUT_H
