#include "Bang/PostProcessEffectFXAA.h"

#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"

using namespace Bang;

PostProcessEffectFXAA::PostProcessEffectFXAA()
{
    CONSTRUCT_CLASS_ID(PostProcessEffectFXAA);

    m_fxaaShaderProgram.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        ShaderProgramFactory::GetEngineShadersDir().Append("FXAA.frag")));
}

PostProcessEffectFXAA::~PostProcessEffectFXAA()
{
}

void PostProcessEffectFXAA::OnRender(RenderPass renderPass)
{
    PostProcessEffect::OnRender(renderPass);

    if (MustBeRendered(renderPass))
    {
        GL::Push(GL::Pushable::VIEWPORT);
        GL::Push(GL::Pushable::BLEND_STATES);
        GL::Push(GL::BindTarget::SHADER_PROGRAM);

        GEngine *ge = GEngine::GetInstance();
        ShaderProgram *sp = m_fxaaShaderProgram.Get();
        sp->Bind();

        ge->GetActiveGBuffer()->ApplyPass(sp, true);

        GL::Pop(GL::BindTarget::SHADER_PROGRAM);
        GL::Pop(GL::Pushable::BLEND_STATES);
        GL::Pop(GL::Pushable::VIEWPORT);
    }
}

void PostProcessEffectFXAA::Reflect()
{
    PostProcessEffect::Reflect();

    GetReflectStructPtr()
        ->GetReflectVariablePtr("PostProcess Shader")
        ->SetHints(BANG_REFLECT_HINT_SHOWN(false));
    GetReflectStructPtr()->GetReflectVariablePtr("Type")->SetHints(
        BANG_REFLECT_HINT_SHOWN(false));
}
