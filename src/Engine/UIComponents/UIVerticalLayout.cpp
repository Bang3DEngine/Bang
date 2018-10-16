#include "Bang/UIVerticalLayout.h"

#include "Bang/Axis.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/Stretch.h"

USING_NAMESPACE_BANG

UIVerticalLayout::UIVerticalLayout() : UIDirLayout(Axis::VERTICAL)
{
    CONSTRUCT_CLASS_ID(UIVerticalLayout)
    SetChildrenHorizontalStretch(Stretch::FULL);
    SetChildrenVerticalStretch(Stretch::NONE);
}
