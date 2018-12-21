#include "Bang/UIMask.h"

#include "Bang/ClassDB.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"

using namespace Bang;

UIMask::UIMask()
{
    SET_INSTANCE_CLASS_ID(UIMask)
}

UIMask::~UIMask()
{
}

void UIMask::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);
    if (!m_restoringStencil && renderPass == RenderPass::CANVAS)
    {
        PrepareStencilToDrawMask();
    }
}

void UIMask::OnBeforeChildrenRender(RenderPass renderPass)
{
    Component::OnBeforeChildrenRender(renderPass);
    if (renderPass == RenderPass::CANVAS)
    {
        PrepareStencilToDrawChildren();
    }
}

void UIMask::OnAfterChildrenRender(RenderPass renderPass)
{
    Component::OnAfterChildrenRender(renderPass);
    if (renderPass == RenderPass::CANVAS)
    {
        RestoreStencilBuffer(renderPass);
    }
}

void UIMask::PrepareStencilToDrawMask()
{
    // Save values for later restoring
    m_colorMaskBefore = GL::GetColorMask();
    m_stencilFuncBefore = GL::GetStencilFunc();
    m_stencilOpBefore = GL::GetStencilOp();

    // Will this mask be drawn?
    GL::SetColorMask(m_drawMask, m_drawMask, m_drawMask, m_drawMask);

    if (IsMasking())
    {
        GL::SetStencilOp(GL::StencilOperation::INCR);
        GL::SetStencilFunc(GL::Function::EQUAL);  // Only increment once
    }
}

void UIMask::PrepareStencilToDrawChildren()
{
    // Restore color mask for children
    GL::SetColorMask(m_colorMaskBefore[0],
                     m_colorMaskBefore[1],
                     m_colorMaskBefore[2],
                     m_colorMaskBefore[3]);

    if (IsMasking())
    {
        // Test and write for current stencil value + 1
        GL::SetStencilValue(GL::GetStencilValue() + 1);
        GL::SetStencilOp(m_stencilOpBefore);
        GL::SetStencilFunc(GL::Function::EQUAL);  // Mask children
    }
}

void UIMask::RestoreStencilBuffer(RenderPass renderPass)
{
    if (!IsMasking())
    {
        return;
    }

    // Restore stencil as it was before, decrementing marked mask pixels
    GL::SetColorMask(false, false, false, false);
    GL::SetStencilFunc(GL::Function::EQUAL);
    GL::SetStencilOp(GL::StencilOperation::DECR);

    m_restoringStencil = true;
    GetGameObject()->Render(renderPass, false);
    m_restoringStencil = false;

    GL::SetStencilValue(GL::GetStencilValue() - 1);
    GL::SetColorMask(m_colorMaskBefore[0],
                     m_colorMaskBefore[1],
                     m_colorMaskBefore[2],
                     m_colorMaskBefore[3]);
    GL::SetStencilOp(m_stencilOpBefore);
    GL::SetStencilFunc(m_stencilFuncBefore);
}

void UIMask::SetMasking(bool maskEnabled)
{
    m_masking = maskEnabled;
}
void UIMask::SetDrawMask(bool drawMask)
{
    m_drawMask = drawMask;
}

bool UIMask::IsMasking() const
{
    return m_masking;
}
bool UIMask::IsDrawMask() const
{
    return true;
}  // m_drawMask; }

void UIMask::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("Masking"))
    {
        SetMasking(metaNode.Get<bool>("Masking"));
    }

    if (metaNode.Contains("DrawMask"))
    {
        SetDrawMask(metaNode.Get<bool>("DrawMask"));
    }
}

void UIMask::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("Masking", IsMasking());
    metaNode->Set("DrawMask", IsDrawMask());
}
