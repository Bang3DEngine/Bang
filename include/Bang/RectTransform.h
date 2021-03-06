﻿#ifndef RECTTRANSFORM_H
#define RECTTRANSFORM_H

#include "BangMath/AARect.h"
#include "BangMath/Axis.h"
#include "Bang/BangDefines.h"
#include "BangMath/Rect.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"
#include "Bang/Transform.h"
#include "BangMath/Vector2.h"

namespace Bang
{
class Serializable;
class Object;

class RectTransform : public Transform
{
    COMPONENT(RectTransform)

public:
    RectTransform();
    virtual ~RectTransform() override;

    void OnRender(RenderPass rp) override;

    void SetMarginLeft(int marginLeft);
    void SetMarginTop(int marginTop);
    void SetMarginRight(int marginRight);
    void SetMarginBot(int marginBot);
    void AddMarginLeft(int marginLeft);
    void AddMarginTop(int marginTop);
    void AddMarginRight(int marginRight);
    void AddMarginBot(int marginBot);
    void SetMargins(int marginAll);
    void SetMarginMin(Axis axis, int marginMax);
    void SetMarginMax(Axis axis, int marginMin);
    void SetMarginLeftBot(const Vector2i &marginLeftBot);
    void SetMarginRightTop(const Vector2i &marginRightTop);
    void SetMargins(const Vector2i &marginLeftBot,
                    const Vector2i &marginRightTop);
    void SetMargins(Axis axis, const Vector2i &margins);
    void SetMargins(Axis axis, int marginMin, int marginMax);
    void SetMargins(int left, int top, int right, int bot);
    void SetPivotPosition(const Vector2 &pivotPosition);
    void SetAnchorMin(const Vector2 &anchorMin);
    void SetAnchorMax(const Vector2 &anchorMax);
    void SetAnchorMinX(float anchorMinX);
    void SetAnchorMinY(float anchorMinY);
    void SetAnchorMaxX(float anchorMaxX);
    void SetAnchorMaxY(float anchorMaxY);
    void SetAnchorX(const Vector2 &anchorX);
    void SetAnchorY(const Vector2 &anchorY);
    void SetAnchors(const Vector2 &anchorPoint);
    void SetAnchors(const Vector2 &anchorMin, const Vector2 &anchorMax);

    void SetWidthFromPivot(int width);
    void SetHeightFromPivot(int height);
    void SetSizeFromPivot(const Vector2i &size);

    int GetMarginLeft() const;
    int GetMarginTop() const;
    int GetMarginRight() const;
    int GetMarginBot() const;
    int GetMarginMin(Axis axis) const;
    int GetMarginMax(Axis axis) const;
    Vector2 GetMargins(Axis axis) const;
    const Vector2i &GetMarginRightTop() const;
    const Vector2i &GetMarginLeftBot() const;
    const Vector2 &GetPivotPosition() const;
    const Vector2 &GetAnchorMin() const;
    const Vector2 &GetAnchorMax() const;
    Rect GetViewportRect() const;
    RectPoints GetViewportRectPointsNDC() const;
    Rect GetParentViewportRect() const;
    RectPoints GetParentViewportRectPointsNDC() const;
    AARect GetViewportAARect() const;
    AARect GetViewportAARectNDC() const;
    AARect GetViewportAARectWithoutTransform() const;
    AARect GetViewportAARectWithoutTransformNDC() const;
    AARect GetParentViewportAARect() const;
    AARect GetParentViewportAARectNDC() const;
    AARect GetParentViewportAARectWithoutTransform() const;
    AARect GetParentViewportAARectWithoutTransformNDC() const;

    // Conversion functions
    AARect FromViewportAARectNDCToLocalAARectNDC(
        const AARect &vpAARectNDC) const;
    AARect FromLocalAARectNDCToViewportAARectNDC(
        const AARect &localAARectNDC) const;
    Rect FromViewportRectNDCToLocalRectNDC(const Rect &vpRectNDC) const;
    Rect FromLocalRectNDCToViewportRectNDC(const Rect &localRectNDC) const;
    Vector2 FromViewportPointNDCToLocalPointNDC(
        const Vector2 &vpPointNDC) const;
    Vector2 FromLocalPointNDCToViewportPointNDC(
        const Vector2 &localPointNDC) const;
    Vector2 FromLocalPointToLocalPointNDC(const Vector2 &localPoint) const;
    Vector2 FromLocalPointNDCToLocalPoint(const Vector2 &localPointNDC) const;
    Vector2 FromViewportPointToLocalPointNDC(const Vector2 &vpPoint) const;
    Vector2 FromViewportPointToLocalPointNDC(const Vector2i &vpPoint) const;
    Vector2 FromViewportPointToLocalPoint(const Vector2 &vpPoint) const;
    Vector2 FromViewportPointToLocalPoint(const Vector2i &vpPoint) const;
    Vector2 FromViewportAmountToLocalAmountNDC(const Vector2 &vpAmount) const;
    Vector2 FromViewportAmountToLocalAmountNDC(const Vector2i &vpAmount) const;
    Vector2 FromWindowAmountToLocalAmountNDC(const Vector2 &winAmount) const;
    Vector2 FromWindowAmountToLocalAmountNDC(const Vector2i &winAmount) const;
    Vector2 FromLocalAmountNDCToViewportAmount(
        const Vector2 &localAmountNDC) const;
    Vector2 FromLocalPointNDCToViewportPoint(
        const Vector2 &localPointNDC) const;

    bool IsMouseOver(bool recursive = false) const;
    bool IsMouseOver(const Vector2i &mousePosVP, bool recursive = false) const;

    // Transform
    const Matrix4 &GetLocalToWorldMatrix() const override;
    const Matrix4 &GetWorldToLocalMatrix() const override;
    void InvalidateTransform() override;
    void OnTransformInvalidated() override;

    const Matrix4 &GetRectLocalToWorldMatrix() const;
    const Matrix4 &GetRectLocalToWorldMatrixInv() const;

    // IEventsObject
    void OnEnabled(Object *object) override;
    void OnDisabled(Object *object) override;

    // Serializable
    void Reflect() override;

private:
    mutable Matrix4 m_rectLocalToWorldMatrix;
    mutable Matrix4 m_rectLocalToWorldMatrixInv;
    mutable AARecti m_vpInWhichRectLocalToWorldWasCalc = AARecti::Zero();

    mutable Matrix4 m_rectTransformLocalToWorldMatrix;
    mutable Matrix4 m_rectTransformLocalToWorldMatrixInv;
    mutable AARecti m_vpInWhichRectTransformLocalToWorldWasCalc =
        AARecti::Zero();

    Vector2i m_marginRightTop = Vector2i::Zero();
    Vector2i m_marginLeftBot = Vector2i::Zero();

    Vector2 m_pivotPosition = Vector2::Zero();
    Vector2 m_anchorMin = -Vector2::One();
    Vector2 m_anchorMax = Vector2::One();

    void WarnWrongAnchorsIfNeeded();

    // Transform
    void CalculateLocalToParentMatrix() const override;

    void CalculateRectLocalToWorldMatrix() const;
    void CalculateRectTransformLocalToWorldMatrix() const;

    friend class UILayoutManager;
};
}  // namespace Bang

#endif  // RECTTRANSFORM_H
