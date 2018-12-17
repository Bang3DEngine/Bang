#include "Bang/PostProcessEffect.h"

#include <istream>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Camera.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Resources.h"
#include "Bang/Resources.tcc"
#include "Bang/Scene.h"
#include "Bang/Shader.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"

using namespace Bang;

PostProcessEffect::PostProcessEffect()
{
    CONSTRUCT_CLASS_ID(PostProcessEffect)
    p_shaderProgram.Set(ShaderProgramFactory::GetDefaultPostProcess());
}

void PostProcessEffect::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);

    if (MustBeRendered(renderPass))
    {
        if (p_postProcessShader && p_shaderProgram.Get()->IsLinked())
        {
            p_shaderProgram.Get()->Bind();
            GEngine::GetActiveGBuffer()->ApplyPass(p_shaderProgram.Get(), true);
            p_shaderProgram.Get()->UnBind();
        }
    }
}

bool PostProcessEffect::MustBeRendered(RenderPass renderPass) const
{
    // Only render if its gameObject contains the active camera
    // Camera *activeSceneCamera = Camera::GetActive();
    // if (GetGameObject()->GetComponents().Contains(activeSceneCamera))
    {
        switch (GetType())
        {
            case Type::AFTER_SCENE:
                return (renderPass == RenderPass::SCENE_POSTPROCESS);

            case Type::AFTER_CANVAS:
                return (renderPass == RenderPass::CANVAS_POSTPROCESS);
        }
    }
    return false;
}

void PostProcessEffect::SetType(PostProcessEffect::Type type)
{
    m_type = type;
}
void PostProcessEffect::SetPriority(int priority)
{
    m_priority = priority;
}
void PostProcessEffect::SetPostProcessShader(Shader *postProcessShader)
{
    if (!postProcessShader)
    {
        return;
    }
    if (postProcessShader == p_postProcessShader.Get())
    {
        return;
    }

    p_postProcessShader = Resources::Create<Shader>();
    p_postProcessShader.Set(postProcessShader);

    p_shaderProgram = Resources::Create<ShaderProgram>();
    p_shaderProgram.Get()->SetVertexShader(
        ShaderProgramFactory::GetDefaultPostProcess()->GetVertexShader());
    p_shaderProgram.Get()->SetFragmentShader(GetPostProcessShader());
    p_shaderProgram.Get()->Link();
}

PostProcessEffect::Type PostProcessEffect::GetType() const
{
    return m_type;
}
int PostProcessEffect::GetPriority() const
{
    return m_priority;
}
ShaderProgram *PostProcessEffect::GetPostProcessShaderProgram() const
{
    return p_shaderProgram.Get();
}
Shader *PostProcessEffect::GetPostProcessShader() const
{
    return p_postProcessShader.Get();
}
Path PostProcessEffect::GetPostProcessShaderFilepath() const
{
    return p_postProcessShader
               ? p_postProcessShader.Get()->GetResourceFilepath()
               : Path();
}

void PostProcessEffect::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Component::CloneInto(clone, cloneGUID);
    PostProcessEffect *ppe = SCAST<PostProcessEffect *>(clone);
    ppe->SetPostProcessShader(GetPostProcessShader());
    ppe->SetType(GetType());
    ppe->SetPriority(GetPriority());
}

void PostProcessEffect::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("Priority"))
    {
        SetPriority(metaNode.Get<int>("Priority"));
    }

    if (metaNode.Contains("Type"))
    {
        SetType(metaNode.Get<Type>("Type"));
    }

    if (metaNode.Contains("PostProcessShader"))
    {
        GUID shaderGUID = metaNode.Get<GUID>("PostProcessShader");
        RH<Shader> ppShader = Resources::Load<Shader>(shaderGUID);
        SetPostProcessShader(ppShader.Get());
    }
}

void PostProcessEffect::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    if (GetPostProcessShader())
    {
        metaNode->Set("PostProcessShader", GetPostProcessShader()->GetGUID());
    }
    metaNode->Set("Priority", GetPriority());
    metaNode->Set("Type", GetType());
}

bool operator<(const PostProcessEffect &lhs, const PostProcessEffect &rhs)
{
    return lhs.GetPriority() < rhs.GetPriority();
}
