#ifndef UIRECTMASK_H
#define UIRECTMASK_H

#include "Bang/AARect.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentMacros.h"
#include "Bang/MetaNode.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"

namespace Bang
{
class UIRectMask : public Component
{
    COMPONENT(UIRectMask)

public:
    UIRectMask();
    virtual ~UIRectMask() override;

    void OnBeforeChildrenRender(RenderPass renderPass) override;
    void OnAfterChildrenRender(RenderPass renderPass) override;

    void SetMasking(bool maskEnabled);

    bool IsMasking() const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_wasScissorEnabled = false;
    AARecti m_prevScissor = AARecti::Zero();

    bool m_masking = true;
};
}

#endif  // UIRECTMASK_H
