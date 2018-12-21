#ifndef UIGRIDLAYOUT_H
#define UIGRIDLAYOUT_H

#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentMacros.h"
#include "Bang/String.h"
#include "Bang/UIGroupLayout.h"

namespace Bang
{
class UIGridLayout : public UIGroupLayout
{
    COMPONENT(UIGridLayout)

public:
    void SetCellSize(const Vector2i &cellSize);

    int GetNumRows() const;
    int GetNumColumns() const;
    const Vector2i &GetCellSize() const;

    // ILayoutController
    virtual void ApplyLayout(Axis axis) override;

    // ILayoutElement
    virtual void CalculateLayout(Axis axis) override;

private:
    Vector2i m_cellSize = Vector2i(40);

    UIGridLayout();
    virtual ~UIGridLayout() override;

    Vector2i GetTotalSpacing() const;
};
}

#endif  // UIGRIDLAYOUT_H
