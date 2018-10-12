#include "Bang/UIContentSizeFitter.h"

#include "Bang/AARect.h"
#include "Bang/MetaNode.h"
#include "Bang/UIRenderer.h"
#include "Bang/GameObject.h"
#include "Bang/RectTransform.h"
#include "Bang/UILayoutElement.h"
#include "Bang/UILayoutManager.h"

USING_NAMESPACE_BANG

UIContentSizeFitter::UIContentSizeFitter()
{
    CONSTRUCT_CLASS_ID(UIContentSizeFitter)
}

UIContentSizeFitter::~UIContentSizeFitter()
{

}

void UIContentSizeFitter::ApplyLayout(Axis axis)
{
    RectTransform *rt = GetGameObject()->GetRectTransform();
    if (!rt)
    {
        return;
    }

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

void UIContentSizeFitter::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("HorizontalSizeType"))
    { SetHorizontalSizeType( metaNode.Get<LayoutSizeType>("HorizontalSizeType") ); }

    if (metaNode.Contains("VerticalSizeType"))
    { SetVerticalSizeType( metaNode.Get<LayoutSizeType>("VerticalSizeType") ); }
}

void UIContentSizeFitter::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("HorizontalSizeType", GetHorizontalSizeType());
    metaNode->Set("VerticalSizeType", GetVerticalSizeType());
}

void UIContentSizeFitter::OnInvalidated()
{
    IInvalidatable<ILayoutController>::OnInvalidated();

}


