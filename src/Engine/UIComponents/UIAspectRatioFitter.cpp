#include "Bang/UIAspectRatioFitter.h"

#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/IInvalidatable.h"
#include "Bang/Math.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/RectTransform.h"
#include "Bang/StreamOperators.h"
#include "Bang/Vector2.h"

namespace Bang
{
class ILayoutController;
}

using namespace Bang;

UIAspectRatioFitter::UIAspectRatioFitter()
{
    SET_INSTANCE_CLASS_ID(UIAspectRatioFitter);
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
    SetAspectRatio(size.x / Math::Max(size.y, 1.0f));
}

void UIAspectRatioFitter::SetAspectRatio(const Vector2i &size)
{
    SetAspectRatio(Vector2(size));
}

void UIAspectRatioFitter::SetPaddings(int paddingsAll)
{
    SetPaddings(Vector2i(paddingsAll), Vector2i(paddingsAll));
}

void UIAspectRatioFitter::SetPaddings(const Vector2i &paddingLeftBot,
                                      const Vector2i &paddingRightTop)
{
    SetPaddingLeftBot(paddingLeftBot);
    SetPaddingRightTop(paddingRightTop);
}

void UIAspectRatioFitter::SetPaddingLeftBot(const Vector2i &paddingLeftBot)
{
    if (paddingLeftBot != GetPaddingLeftBot())
    {
        m_paddingLeftBot = paddingLeftBot;
        Invalidate();
    }
}

void UIAspectRatioFitter::SetPaddingRightTop(const Vector2i &paddingRightTop)
{
    if (paddingRightTop != GetPaddingRightTop())
    {
        m_paddingRightTop = paddingRightTop;
        Invalidate();
    }
}

const Vector2i &UIAspectRatioFitter::GetPaddingLeftBot() const
{
    return m_paddingLeftBot;
}

const Vector2i &UIAspectRatioFitter::GetPaddingRightTop() const
{
    return m_paddingRightTop;
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
    if (!rt)
    {
        return;
    }

    GameObject *parent = GetGameObject()->GetParent();
    if (!parent)
    {
        return;
    }

    RectTransform *parentRT = parent->GetRectTransform();
    if (!parentRT)
    {
        return;
    }

    const Vector2i parentRTSize = Vector2i::Max(
        Vector2i::One(), Vector2i(parentRT->GetViewportAARect().GetSize()));

    Vector2i currentSize(
        Vector2::Round(Vector2(GetAspectRatio(), 1) * 30000.0f));
    Vector2i newSize = AspectRatio::GetAspectRatioedSize(
        currentSize, parentRTSize, GetAspectRatioMode());
    newSize -= (GetPaddingLeftBot() + GetPaddingRightTop());

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
    {
        SetAspectRatio(metaNode.Get<float>("AspectRatio"));
    }

    if (metaNode.Contains("AspectRatioMode"))
    {
        SetAspectRatioMode(metaNode.Get<AspectRatioMode>("AspectRatioMode"));
    }

    if (metaNode.Contains("PaddingLeftTop"))
    {
        SetPaddingLeftBot(metaNode.Get<Vector2i>("PaddingLeftTop"));
    }

    if (metaNode.Contains("PaddingRightBot"))
    {
        SetPaddingRightTop(metaNode.Get<Vector2i>("PaddingRightBot"));
    }
}

void UIAspectRatioFitter::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("AspectRatio", GetAspectRatio());
    metaNode->Set("AspectRatioMode", GetAspectRatioMode());
    metaNode->Set("PaddingLeftBot", GetPaddingLeftBot());
    metaNode->Set("PaddingRightTop", GetPaddingRightTop());
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
