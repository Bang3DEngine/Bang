#include "Bang/UIDirLayout.h"

#include "Bang/Alignment.h"
#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/ClassDB.h"
#include "Bang/GameObject.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/Math.h"
#include "Bang/RectTransform.h"
#include "Bang/Stretch.h"
#include "Bang/UILayoutManager.h"
#include "Bang/Vector2.h"

using namespace Bang;

UIDirLayout::UIDirLayout() : UIDirLayout(Axis::VERTICAL)
{
    SET_INSTANCE_CLASS_ID(UIDirLayout)
}

UIDirLayout::UIDirLayout(Axis axis) : m_axis(axis)
{
    SET_INSTANCE_CLASS_ID(UIDirLayout)
}

UIDirLayout::~UIDirLayout()
{
}

Vector2i UIDirLayout::GetTotalSpacing(const Array<GameObject *> &children) const
{
    const Vector2i spacing = GetDir() * GetSpacing();
    return spacing * SCAST<int>(children.Size() - 1);
}

void UIDirLayout::ApplyLayout(Axis axis)
{
    RectTransform *rt = GetGameObject()->GetRectTransform();
    if (!rt)
    {
        return;
    }
    Array<GameObject *> children =
        UILayoutManager::GetLayoutableChildrenList(GetGameObject());

    Vector2i layoutRectSize(Vector2::Round(rt->GetViewportAARect().GetSize()));
    Vector2i paddedLayoutRectSize = layoutRectSize - GetPaddingSize();

    Array<Vector2i> childrenRTSizes(children.Size(), Vector2i::Zero());
    {
        Vector2i availableSpace =
            paddedLayoutRectSize - GetTotalSpacing(children);

        FillChildrenMinSizes(
            paddedLayoutRectSize, children, &childrenRTSizes, &availableSpace);

        FillChildrenPreferredSizes(
            paddedLayoutRectSize, children, &childrenRTSizes, &availableSpace);
        FillChildrenFlexibleSizes(
            paddedLayoutRectSize, children, &childrenRTSizes, &availableSpace);
        ApplyStretches(paddedLayoutRectSize, &childrenRTSizes);
    }

    // Apply actual calculation to RectTransforms Margins
    uint i = 0;
    Vector2i currentTopLeft(GetPaddingLeft(), GetPaddingTop());
    for (GameObject *child : children)
    {
        Vector2i spacing =
            (i > 0) ? (GetDir() * GetSpacing()) : Vector2i::Zero();
        currentTopLeft += spacing;

        Vector2i childRTSize = childrenRTSizes[i];
        RectTransform *crt = child->GetRectTransform();

        ApplyLayoutToChildRectTransform(
            axis, layoutRectSize, crt, currentTopLeft, childRTSize);
        currentTopLeft += childRTSize;
        ++i;
    }
}

void UIDirLayout::ApplyLayoutToChildRectTransform(
    Axis rebuildPassAxis,
    const Vector2i &layoutRectSize,
    RectTransform *crt,
    const Vector2i &position,
    const Vector2i &childRTSize)
{
    if (!crt)
    {
        return;
    }
    crt->SetAnchors(Vector2(-1, 1));

    Vector2i paddedLayoutRectSize = layoutRectSize - GetPaddingSize();
    if (GetAxis() == Axis::VERTICAL)
    {
        if (rebuildPassAxis == Axis::HORIZONTAL)
        {
            crt->SetMarginLeft(GetPaddingLeft());

            HorizontalAlignment hAlign = GetChildrenHorizontalAlignment();
            if (hAlign == HorizontalAlignment::CENTER)
            {
                crt->AddMarginLeft((paddedLayoutRectSize.x - childRTSize.x) /
                                   2);
            }
            else if (hAlign == HorizontalAlignment::RIGHT)
            {
                crt->AddMarginLeft((paddedLayoutRectSize.x - childRTSize.x));
            }
            crt->SetMarginRight(-(crt->GetMarginLeft() + childRTSize.x));
        }
        else  // Axis::Vertical
        {
            crt->SetMarginTop(position.y);
            crt->SetMarginBot(-(crt->GetMarginTop() + childRTSize.y));
        }
    }
    else  // Axis::Horizontal
    {
        if (rebuildPassAxis == Axis::VERTICAL)
        {
            crt->SetMarginTop(GetPaddingTop());

            VerticalAlignment vAlign = GetChildrenVerticalAlignment();
            if (vAlign == VerticalAlignment::CENTER)
            {
                crt->AddMarginTop((paddedLayoutRectSize.y - childRTSize.y) / 2);
            }
            else if (vAlign == VerticalAlignment::BOT)
            {
                crt->AddMarginTop((paddedLayoutRectSize.y - childRTSize.y));
            }
            crt->SetMarginBot(-(crt->GetMarginTop() + childRTSize.y));
        }
        else  // Axis::Horizontal
        {
            crt->SetMarginLeft(position.x);
            crt->SetMarginRight(-(crt->GetMarginLeft() + childRTSize.x));
        }
    }
}

void UIDirLayout::FillChildrenMinSizes(const Vector2i &layoutRectSize,
                                       const Array<GameObject *> &children,
                                       Array<Vector2i> *childrenRTSizes,
                                       Vector2i *availableSpace)
{
    uint i = 0;
    for (GameObject *child : children)
    {
        Vector2i childRTSize = UILayoutManager::GetMinSize(child);
        if (GetAxis() == Axis::VERTICAL)
        {
            availableSpace->y -= childRTSize.y;
            childRTSize.x = Math::Min(childRTSize.x, layoutRectSize.x);
        }
        else  // Axis::Horizontal
        {
            availableSpace->x -= childRTSize.x;
            childRTSize.y = Math::Min(childRTSize.y, layoutRectSize.y);
        }

        (*childrenRTSizes)[i] = childRTSize;
        ++i;
    }
}

void UIDirLayout::FillChildrenPreferredSizes(
    const Vector2i &layoutRectSize,
    const Array<GameObject *> &children,
    Array<Vector2i> *childrenRTSizes,
    Vector2i *availableSpace)
{
    Array<Vector2i> prefSizes;
    Vector2i totalPrefPxToAdd = Vector2i::Zero();
    for (uint i = 0; i < children.Size(); ++i)
    {
        GameObject *child = children[i];
        Vector2i minChildSize = (*childrenRTSizes)[i];
        Vector2i prefSize(UILayoutManager::GetPreferredSize(child));
        Vector2i pxToAdd = prefSize - minChildSize;
        pxToAdd = Vector2i::Max(pxToAdd, Vector2i::Zero());
        totalPrefPxToAdd += pxToAdd;
        prefSizes.PushBack(prefSize);
    }
    totalPrefPxToAdd = Vector2i::Max(totalPrefPxToAdd, Vector2i::One());

    // Populate with new children sizes
    Array<Vector2i> newChildRTSizes;
    Array<Vector2i> childPreferredSizes;
    for (uint i = 0; i < children.Size(); ++i)
    {
        Vector2i minChildSize = (*childrenRTSizes)[i];
        Vector2i childPrefSize = prefSizes[i];
        Vector2i childPrefPxToAdd = (childPrefSize - minChildSize);
        childPrefPxToAdd = Vector2i::Max(childPrefPxToAdd, Vector2i::Zero());
        Vector2d sizeProportion(Vector2d(childPrefPxToAdd) /
                                Vector2d(totalPrefPxToAdd));
        Vector2i prefAvailPxToAdd(sizeProportion * Vector2d(*availableSpace));
        prefAvailPxToAdd = Vector2i::Min(prefAvailPxToAdd, childPrefPxToAdd);
        Vector2i childRTSize =
            Vector2i::Max(minChildSize, minChildSize + prefAvailPxToAdd);

        newChildRTSizes.PushBack(childRTSize);
        childPreferredSizes.PushBack(childPrefSize);
    }

    // Apply children sizes populating the final array
    for (uint i = 0; i < newChildRTSizes.Size(); ++i)
    {
        // Apply children sizes populating the final array
        Vector2i newChildRTSize = newChildRTSizes[i];
        Vector2i minChildSize = (*childrenRTSizes)[i];
        if (GetAxis() == Axis::VERTICAL)
        {
            availableSpace->y -= (newChildRTSize.y - minChildSize.y);
            newChildRTSize.x = Math::Min(newChildRTSize.x, layoutRectSize.x);
        }
        else  // Axis::Horizontal
        {
            availableSpace->x -= (newChildRTSize.x - minChildSize.x);
            newChildRTSize.y = Math::Min(newChildRTSize.y, layoutRectSize.y);
        }

        newChildRTSizes[i] = newChildRTSize;
    }

    *childrenRTSizes = newChildRTSizes;
}

void UIDirLayout::FillChildrenFlexibleSizes(const Vector2i &layoutRectSize,
                                            const Array<GameObject *> &children,
                                            Array<Vector2i> *childrenRTSizes,
                                            Vector2i *availableSpace)
{
    Vector2d totalChildrenFlexSize = Vector2d::Zero();
    Array<Vector2d> flexSizes;
    for (uint i = 0; i < children.Size(); ++i)
    {
        GameObject *child = children[i];
        Vector2d flexSize(UILayoutManager::GetFlexibleSize(child));
        flexSizes.PushBack(flexSize);
        totalChildrenFlexSize += Vector2d(flexSize);
    }
    totalChildrenFlexSize =
        Vector2d::Max(totalChildrenFlexSize, Vector2d(0.0001));

    // Populate with new children sizes
    Array<Vector2i> newChildRTSizes;
    Array<Vector2d> childFlexibleSizes;
    Vector2d originalAvailableSpace(*availableSpace);
    for (uint i = 0; i < children.Size(); ++i)
    {
        Vector2i prefChildSize((*childrenRTSizes)[i]);
        Vector2d childFlexSize(flexSizes[i]);
        Vector2d sizeProportion(childFlexSize / totalChildrenFlexSize);
        Vector2i flexAvailPxToAdd(sizeProportion * originalAvailableSpace);
        Vector2i childRTSize =
            Vector2i::Max(prefChildSize, prefChildSize + flexAvailPxToAdd);
        if (GetAxis() == Axis::VERTICAL)
        {
            childRTSize.x = Math::Min(childRTSize.x, layoutRectSize.x);
            availableSpace->y -= flexAvailPxToAdd.y;
        }
        else  // Axis::Horizontal
        {
            childRTSize.y = Math::Min(childRTSize.y, layoutRectSize.y);
            availableSpace->x -= flexAvailPxToAdd.x;
        }

        newChildRTSizes.PushBack(childRTSize);
        childFlexibleSizes.PushBack(childFlexSize);
    }

    *childrenRTSizes = newChildRTSizes;
}

void UIDirLayout::ApplyStretches(const Vector2i &layoutRectSize,
                                 Array<Vector2i> *childrenRTSizes)
{
    Array<Vector2i> newChildrenRTSizes;
    for (const Vector2i &childRTSize : *childrenRTSizes)
    {
        Vector2i newChildRTSize = childRTSize;
        if (GetChildrenHorizontalStretch() == Stretch::FULL)
        {
            if (GetAxis() == Axis::HORIZONTAL)
            {
                newChildRTSize.x = layoutRectSize.x / childrenRTSizes->Size();
            }
            else
            {
                newChildRTSize.x = layoutRectSize.x;
            }
        }

        if (GetChildrenVerticalStretch() == Stretch::FULL)
        {
            if (GetAxis() == Axis::VERTICAL)
            {
                newChildRTSize.y = layoutRectSize.y / childrenRTSizes->Size();
            }
            else
            {
                newChildRTSize.y = layoutRectSize.y;
            }
        }

        newChildrenRTSizes.PushBack(newChildRTSize);
    }

    *childrenRTSizes = newChildrenRTSizes;
}

void UIDirLayout::CalculateLayout(Axis axis)
{
    Vector2i minSize = Vector2i::Zero();
    Vector2i prefSize = Vector2i::Zero();
    Array<GameObject *> children =
        UILayoutManager::GetLayoutableChildrenList(GetGameObject());
    for (GameObject *child : children)
    {
        Vector2i cMinSize(UILayoutManager::GetSize(child, LayoutSizeType::MIN));
        Vector2i cPrefSize(
            UILayoutManager::GetSize(child, LayoutSizeType::PREFERRED));
        if (GetAxis() == Axis::VERTICAL)
        {
            minSize.x = Math::Max(minSize.x, cMinSize.x);
            prefSize.x = Math::Max(prefSize.x, cPrefSize.x);
            minSize.y += cMinSize.y;
            prefSize.y += cPrefSize.y;
        }
        else  // Axis::Horizontal
        {
            minSize.x += cMinSize.x;
            prefSize.x += cPrefSize.x;
            minSize.y = Math::Max(minSize.y, cMinSize.y);
            prefSize.y = Math::Max(prefSize.y, cPrefSize.y);
        }
    }

    Vector2i addedSize(GetTotalSpacing(children) + GetPaddingSize());
    minSize += addedSize;
    prefSize += addedSize;

    SetCalculatedLayout(axis, minSize.GetAxis(axis), prefSize.GetAxis(axis));
}

Axis UIDirLayout::GetAxis() const
{
    return m_axis;
}

Vector2i UIDirLayout::GetDir() const
{
    return Vector2i::FromAxis(m_axis);
}
