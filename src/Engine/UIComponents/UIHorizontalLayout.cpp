#include "Bang/UIHorizontalLayout.h"

USING_NAMESPACE_BANG

UIHorizontalLayout::UIHorizontalLayout() : UIDirLayout(Axis::HORIZONTAL)
{
    SetChildrenHorizontalStretch(Stretch::NONE);
    SetChildrenVerticalStretch(Stretch::FULL);
}
