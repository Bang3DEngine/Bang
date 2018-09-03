#include "Bang/UIAspectRatioFitter.h"

#include "Bang/Rect.h"
#include "Bang/AARect.h"
#include "Bang/MetaNode.h"
#include "Bang/GameObject.h"
#include "Bang/RectTransform.h"

USING_NAMESPACE_BANG

UIAspectRatioFitter::UIAspectRatioFitter()
{
    SetAspectRatioMode(AspectRatioMode::KEEP);
}

UIAspectRatioFitter::~UIAspectRatioFitter()
{
}

void UIAspectRatioFitter::SetAspectRatio(float aspectRatio)
{
    if (aspectRatio != GetAspectRatio())
    {
        m_aspectRatio = aspectRatio;
        Invalidate();
    }
}

void UIAspectRatioFitter::SetAspectRatio(const Vector2 &size)
{
    SetAspectRatio( size.x / Math::Max(size.y, 1.0f) );
}

void UIAspectRatioFitter::SetAspectRatio(const Vector2i &size)
{
    SetAspectRatio( Vector2(size) );
}

float UIAspectRatioFitter::GetAspectRatio() const
{
    return m_aspectRatio;
}

void UIAspectRatioFitter::SetAspectRatioMode(AspectRatioMode arMode)
{
    if (arMode != GetAspectRatioMode())
    {
        m_aspectRatioMode = arMode;
        Invalidate();
    }
}

AspectRatioMode UIAspectRatioFitter::GetAspectRatioMode() const
{
    return m_aspectRatioMode;
}

void UIAspectRatioFitter::ApplyLayout(Axis axis)
{
    RectTransform *rt = GetGameObject()->GetRectTransform();
    if (!rt) { return; }

    GameObject *parent = GetGameObject()->GetParent();
    if (!parent) { return; }

    RectTransform *parentRT = parent->GetRectTransform();
    if (!parentRT) { return; }

    const Vector2i parentSize = Vector2i::Max(Vector2i::One,
                                   Vector2i(parentRT->GetViewportAARect().GetSize()));

    Vector2i currentSize( Vector2::Round( Vector2(GetAspectRatio(), 1) * 30000.0f ) );
    Vector2i newSize = AspectRatio::GetAspectRatioedSize(currentSize,
                                                         parentSize,
                                                         GetAspectRatioMode());
    if (axis == Axis::VERTICAL)
    {
        rt->SetWidthFromPivot(newSize.x);
        rt->SetHeightFromPivot(newSize.y);
    }
}

void UIAspectRatioFitter::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("AspectRatio"))
    { SetAspectRatio(metaNode.Get<float>("AspectRatio")); }

    if (metaNode.Contains("AspectRatioMode"))
    { SetAspectRatioMode(metaNode.Get<AspectRatioMode>("AspectRatioMode")); }
}

void UIAspectRatioFitter::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("AspectRatio", GetAspectRatio());
    metaNode->Set("AspectRatioMode", GetAspectRatioMode());
}

void UIAspectRatioFitter::OnTransformChanged()
{
    ILayoutSelfController::OnTransformChanged();
    Invalidate();
}

void UIAspectRatioFitter::OnParentTransformChanged()
{
    ILayoutSelfController::OnParentTransformChanged();
    Invalidate();
}

void UIAspectRatioFitter::OnInvalidated()
{
    IInvalidatable<ILayoutController>::OnInvalidated();
}
