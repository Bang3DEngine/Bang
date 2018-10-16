#include "Bang/UIHorizontalLayout.h"

#include "Bang/Axis.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/Stretch.h"

USING_NAMESPACE_BANG

UIHorizontalLayout::UIHorizontalLayout() : UIDirLayout(Axis::HORIZONTAL)
{
    CONSTRUCT_CLASS_ID(UIHorizontalLayout);
    SetChildrenHorizontalStretch(Stretch::NONE);
    SetChildrenVerticalStretch(Stretch::FULL);
}
