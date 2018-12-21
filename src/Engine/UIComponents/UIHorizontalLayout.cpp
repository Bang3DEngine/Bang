#include "Bang/UIHorizontalLayout.h"

#include "Bang/Axis.h"
#include "Bang/ClassDB.h"
#include "Bang/Stretch.h"

using namespace Bang;

UIHorizontalLayout::UIHorizontalLayout() : UIDirLayout(Axis::HORIZONTAL)
{
    SET_INSTANCE_CLASS_ID(UIHorizontalLayout);
    SetChildrenHorizontalStretch(Stretch::NONE);
    SetChildrenVerticalStretch(Stretch::FULL);
}
