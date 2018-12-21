#include "Bang/UILayoutIgnorer.h"

#include "Bang/ClassDB.h"

using namespace Bang;

UILayoutIgnorer::UILayoutIgnorer()
{
    SET_INSTANCE_CLASS_ID(UILayoutIgnorer)
}

UILayoutIgnorer::~UILayoutIgnorer()
{
}

void UILayoutIgnorer::SetIgnoreLayout(bool ignoreLayout)
{
    m_ignoreLayout = ignoreLayout;
}

bool UILayoutIgnorer::IsIgnoreLayout() const
{
    return m_ignoreLayout;
}
