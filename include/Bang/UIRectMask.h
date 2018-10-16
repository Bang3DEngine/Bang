#ifndef UIRECTMASK_H
#define UIRECTMASK_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/GL.h"
#include "Bang/MetaNode.h"
#include "Bang/RenderPass.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

class UIRectMask : public Component
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(UIRectMask)

public:
    UIRectMask();
    virtual ~UIRectMask();

    void OnBeforeChildrenRender(RenderPass renderPass) override;
    void OnAfterChildrenRender(RenderPass renderPass) override;

    void SetMasking(bool maskEnabled);

    bool IsMasking() const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_wasScissorEnabled = false;
    AARecti m_prevScissor = AARecti::Zero;

    bool m_masking = true;
};

NAMESPACE_BANG_END

#endif // UIRECTMASK_H
