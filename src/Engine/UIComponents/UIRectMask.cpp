#include "Bang/UIRectMask.h"

#include "Bang/FastDynamicCast.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/MetaNode.h"
#include "Bang/Rect.h"
#include "Bang/MetaNode.tcc"
#include "Bang/AARect.h"
#include "Bang/RectTransform.h"
#include "Bang/Vector.tcc"

USING_NAMESPACE_BANG

UIRectMask::UIRectMask()
{
    CONSTRUCT_CLASS_ID(UIRectMask)
}

UIRectMask::~UIRectMask()
{
}

void UIRectMask::OnBeforeChildrenRender(RenderPass renderPass)
{
    Component::OnBeforeChildrenRender(renderPass);

    if (IsMasking() && renderPass == RenderPass::CANVAS)
    {
        m_wasScissorEnabled = GL::IsEnabled(GL::Enablable::SCISSOR_TEST);
        m_prevScissor = GL::GetScissorRect();

        AARecti rectPx( GetGameObject()->GetRectTransform()->GetViewportRect() );

        GL::Enable(GL::Enablable::SCISSOR_TEST);
        GL::ScissorIntersecting(rectPx);
    }
}

void UIRectMask::OnAfterChildrenRender(RenderPass renderPass)
{
    Component::OnAfterChildrenRender(renderPass);

    if (IsMasking() && renderPass == RenderPass::CANVAS)
    {
        // Restore
        GL::Scissor(m_prevScissor);
        GL::SetEnabled(GL::Enablable::SCISSOR_TEST, m_wasScissorEnabled);
    }
}

void UIRectMask::SetMasking(bool maskEnabled) { m_masking = maskEnabled; }
bool UIRectMask::IsMasking() const { return m_masking; }

void UIRectMask::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("Masking"))
    { SetMasking( metaNode.Get<bool>("Masking") ); }
}

void UIRectMask::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("Masking", IsMasking());
}
