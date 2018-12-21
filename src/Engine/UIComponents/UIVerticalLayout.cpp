#include "Bang/UIVerticalLayout.h"

#include "Bang/Axis.h"
#include "Bang/ClassDB.h"
#include "Bang/Stretch.h"

using namespace Bang;

UIVerticalLayout::UIVerticalLayout() : UIDirLayout(Axis::VERTICAL)
{
    SET_INSTANCE_CLASS_ID(UIVerticalLayout)
    SetChildrenHorizontalStretch(Stretch::FULL);
    SetChildrenVerticalStretch(Stretch::NONE);
}
