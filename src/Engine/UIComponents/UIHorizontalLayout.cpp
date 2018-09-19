#include "Bang/UIHorizontalLayout.h"

USING_NAMESPACE_BANG

UIHorizontalLayout::UIHorizontalLayout() : UIDirLayout(Axis::HORIZONTAL)
{
    CONSTRUCT_CLASS_ID(UIHorizontalLayout);
    SetChildrenHorizontalStretch(Stretch::NONE);
    SetChildrenVerticalStretch(Stretch::FULL);
}
