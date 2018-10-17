#include "Bang/UIHorizontalLayout.h"

#include "Bang/Axis.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/Stretch.h"

using namespace Bang;

UIHorizontalLayout::UIHorizontalLayout() : UIDirLayout(Axis::HORIZONTAL)
{
    CONSTRUCT_CLASS_ID(UIHorizontalLayout);
    SetChildrenHorizontalStretch(Stretch::NONE);
    SetChildrenVerticalStretch(Stretch::FULL);
}
