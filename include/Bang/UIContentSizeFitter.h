#ifndef UICONTENTSIZEFITTER_H
#define UICONTENTSIZEFITTER_H

#include "Bang/Axis.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/ILayoutSelfController.h"
#include "Bang/LayoutSizeType.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"

namespace Bang
{
class UIContentSizeFitter : public Component, public ILayoutSelfController
{
    COMPONENT(UIContentSizeFitter)

public:
    void SetHorizontalSizeType(LayoutSizeType sizeType);
    void SetVerticalSizeType(LayoutSizeType sizeType);

    LayoutSizeType GetHorizontalSizeType() const;
    LayoutSizeType GetVerticalSizeType() const;

    // ILayoutElement
    virtual void ApplyLayout(Axis axis) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // IInvalidatable
    void OnInvalidated() override;

private:
    LayoutSizeType m_verticalSizeType = LayoutSizeType::NONE;
    LayoutSizeType m_horizontalSizeType = LayoutSizeType::NONE;

    UIContentSizeFitter();
    virtual ~UIContentSizeFitter() override;
};
}

#endif  // UICONTENTSIZEFITTER_H
