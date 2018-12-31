#include "Bang/PostProcessEffect.h"

#include <istream>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Camera.h"
#include "Bang/ClassDB.h"
#include "Bang/Extensions.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Scene.h"
#include "Bang/Shader.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"

using namespace Bang;

PostProcessEffect::PostProcessEffect()
{
    SET_INSTANCE_CLASS_ID(PostProcessEffect)
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
                return (renderPass == RenderPass::SCENE_BEFORE_ADDING_LIGHTS);

            case Type::AFTER_SCENE_AND_LIGHT:
                return (renderPass == RenderPass::SCENE_AFTER_ADDING_LIGHTS);

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

    p_postProcessShader = Assets::Create<Shader>();
    p_postProcessShader.Set(postProcessShader);

    p_shaderProgram = Assets::Create<ShaderProgram>();
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
    return p_postProcessShader ? p_postProcessShader.Get()->GetAssetFilepath()
                               : Path();
}

void PostProcessEffect::Reflect()
{
    Component::Reflect();

    BANG_REFLECT_VAR_ASSET(
        "PostProcess Shader",
        SetPostProcessShader,
        GetPostProcessShader,
        Shader,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetShaderExtensions()));

    BANG_REFLECT_VAR_MEMBER(
        PostProcessEffect, "Priority", SetPriority, GetPriority);

    BANG_REFLECT_VAR_ENUM("Type", SetType, GetType, PostProcessEffect::Type);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Type", "After Scene", PostProcessEffect::Type::AFTER_SCENE);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Type",
        "After Scene and Light",
        PostProcessEffect::Type::AFTER_SCENE_AND_LIGHT);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Type", "After Canvas", PostProcessEffect::Type::AFTER_CANVAS);
}

bool operator<(const PostProcessEffect &lhs, const PostProcessEffect &rhs)
{
    return lhs.GetPriority() < rhs.GetPriority();
}
