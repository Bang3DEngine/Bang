#ifndef UIVERTICALLAYOUT_H
#define UIVERTICALLAYOUT_H

#include "Bang/BangDefines.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIDirLayout.h"

NAMESPACE_BANG_BEGIN

class UIVerticalLayout : public UIDirLayout
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIVerticalLayout)

public:
    UIVerticalLayout();
};

NAMESPACE_BANG_END

#endif // UIVERTICALLAYOUT_H
