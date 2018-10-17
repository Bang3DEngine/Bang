#include "Bang/UILayoutIgnorer.h"

#include "Bang/FastDynamicCast.h"

using namespace Bang;

UILayoutIgnorer::UILayoutIgnorer()
{
    CONSTRUCT_CLASS_ID(UILayoutIgnorer)
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
