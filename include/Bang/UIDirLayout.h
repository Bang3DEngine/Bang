#ifndef UIDIRLAYOUT_H
#define UIDIRLAYOUT_H

#include "Bang/Array.h"
#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIGroupLayout.h"

namespace Bang
{
class GameObject;
class RectTransform;

class UIDirLayout : public UIGroupLayout
{
    COMPONENT(UIDirLayout)

public:
    // ILayoutController
    virtual void ApplyLayout(Axis axis) override;

    // ILayoutElement
    virtual void CalculateLayout(Axis axis) override;

    Axis GetAxis() const;
    Vector2i GetDir() const;

protected:
    UIDirLayout();
    UIDirLayout(Axis axis);
    virtual ~UIDirLayout() override;

private:
    Axis m_axis = Axis::HORIZONTAL;

    Vector2i GetTotalSpacing(const Array<GameObject *> &children) const;

    void ApplyLayoutToChildRectTransform(Axis rebuildPassAxis,
                                         const Vector2i &layoutRectSize,
                                         RectTransform *childRT,
                                         const Vector2i &position,
                                         const Vector2i &childRTSize);

    void FillChildrenMinSizes(const Vector2i &layoutRectSize,
                              const Array<GameObject *> &children,
                              Array<Vector2i> *childrenRTSizes,
                              Vector2i *availableSpace);
    void FillChildrenPreferredSizes(const Vector2i &layoutRectSize,
                                    const Array<GameObject *> &children,
                                    Array<Vector2i> *childrenRTSizes,
                                    Vector2i *availableSpace);
    void FillChildrenFlexibleSizes(const Vector2i &layoutRectSize,
                                   const Array<GameObject *> &children,
                                   Array<Vector2i> *childrenRTSizes,
                                   Vector2i *availableSpace);
    void ApplyStretches(const Vector2i &layoutRectSize,
                        Array<Vector2i> *childrenRTSizes);
};
}

#endif  // UIDIRLAYOUT_H
