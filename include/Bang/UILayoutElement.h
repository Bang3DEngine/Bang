#ifndef UILAYOUTELEMENT_H
#define UILAYOUTELEMENT_H

#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/ILayoutElement.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"

namespace Bang
{
class UILayoutElement : public Component, public ILayoutElement
{
    COMPONENT(UILayoutElement)

public:
    UILayoutElement();
    virtual ~UILayoutElement() override;

    void SetMinWidth(int minWidth);
    void SetMinHeight(int minHeight);
    void SetMinSize(const Vector2i &minSize);
    void SetMinSizeInAxis(int minMagnitude, Axis axis);
    void SetPreferredWidth(int preferredWidth);
    void SetPreferredHeight(int preferredHeight);
    void SetPreferredSize(const Vector2i &preferredSize);
    void SetPreferredSizeInAxis(int preferredMagnitude, Axis axis);
    void SetFlexibleWidth(float flexibleWidth);
    void SetFlexibleHeight(float flexibleHeight);
    void SetFlexibleSize(const Vector2 &flexibleSize);
    void SetFlexibleSizeInAxis(float flexibleMagnitude, Axis axis);

    int GetMinWidth() const;
    int GetMinHeight() const;
    int GetPreferredWidth() const;
    int GetPreferredHeight() const;
    float GetFlexibleWidth() const;
    float GetFlexibleHeight() const;

    Vector2i GetMinSize() const;
    Vector2i GetPreferredSize() const;
    Vector2 GetFlexibleSize() const;

protected:
    virtual void CalculateLayout(Axis axis) override;

private:
    Vector2i m_minSize = -Vector2i::One();
    Vector2i m_preferredSize = -Vector2i::One();
    Vector2 m_flexibleSize = -Vector2::One();

    void OnChanged();
};
}  // namespace Bang

#endif  // UILAYOUTELEMENT_H
