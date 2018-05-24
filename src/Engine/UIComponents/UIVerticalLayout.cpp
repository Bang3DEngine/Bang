#include "Bang/UIVerticalLayout.h"

USING_NAMESPACE_BANG

UIVerticalLayout::UIVerticalLayout() : UIDirLayout(Axis::VERTICAL)
{
    SetChildrenHorizontalStretch(Stretch::FULL);
    SetChildrenVerticalStretch(Stretch::NONE);
}
