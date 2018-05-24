#include "Bang/UIContentSizeFitter.h"

#include "Bang/AARect.h"
#include "Bang/XMLNode.h"
#include "Bang/UIRenderer.h"
#include "Bang/GameObject.h"
#include "Bang/RectTransform.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutManager.h"

USING_NAMESPACE_BANG

UIContentSizeFitter::UIContentSizeFitter()
{
}

UIContentSizeFitter::~UIContentSizeFitter()
{

}

void UIContentSizeFitter::ApplyLayout(Axis axis)
{
    RectTransform *rt = GetGameObject()->GetRectTransform();
    if (!rt) { return; }

    if (axis == Axis::HORIZONTAL &&
        GetHorizontalSizeType() != LayoutSizeType::NONE)
    {
        Vector2i hSize (UILayoutManager::GetSize(GetGameObject(),
                                                 GetHorizontalSizeType()));
        rt->SetWidthFromPivot(hSize.x);
    }

    if (axis == Axis::VERTICAL &&
        GetVerticalSizeType() != LayoutSizeType::NONE)
    {
        Vector2i vSize (UILayoutManager::GetSize(GetGameObject(),
                                                 GetVerticalSizeType()));
        rt->SetHeightFromPivot(vSize.y);
    }
}

void UIContentSizeFitter::SetHorizontalSizeType(LayoutSizeType sizeType)
{
    ASSERT(sizeType == LayoutSizeType::NONE ||
           sizeType == LayoutSizeType::MIN  ||
           sizeType == LayoutSizeType::PREFERRED);

    if (sizeType != GetHorizontalSizeType())
    {
        m_horizontalSizeType = sizeType;
        Invalidate();
    }
}

void UIContentSizeFitter::SetVerticalSizeType(LayoutSizeType sizeType)
{
    ASSERT(sizeType == LayoutSizeType::NONE ||
           sizeType == LayoutSizeType::MIN  ||
           sizeType == LayoutSizeType::PREFERRED);

    if (sizeType != GetVerticalSizeType())
    {
        m_verticalSizeType = sizeType;
        Invalidate();
    }
}

LayoutSizeType UIContentSizeFitter::GetHorizontalSizeType() const
{
    return m_horizontalSizeType;
}

LayoutSizeType UIContentSizeFitter::GetVerticalSizeType() const
{
    return m_verticalSizeType;
}

void UIContentSizeFitter::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);

    if (xmlInfo.Contains("HorizontalSizeType"))
    { SetHorizontalSizeType( xmlInfo.Get<LayoutSizeType>("HorizontalSizeType") ); }

    if (xmlInfo.Contains("VerticalSizeType"))
    { SetVerticalSizeType( xmlInfo.Get<LayoutSizeType>("VerticalSizeType") ); }
}

void UIContentSizeFitter::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("HorizontalSizeType", GetHorizontalSizeType());
    xmlInfo->Set("VerticalSizeType", GetVerticalSizeType());
}

void UIContentSizeFitter::OnInvalidated()
{
    IInvalidatable<ILayoutController>::OnInvalidated();

}


