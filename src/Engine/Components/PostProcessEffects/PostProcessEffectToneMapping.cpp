#include "Bang/PostProcessEffectToneMapping.h"

#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"

using namespace Bang;

PostProcessEffectToneMapping::PostProcessEffectToneMapping()
{
    m_toneMappingShaderProgram.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        ShaderProgramFactory::GetEngineShadersDir().Append(
            "ToneMapping.frag")));

    SetType(PostProcessEffect::Type::AFTER_SCENE_AND_LIGHT);
}

PostProcessEffectToneMapping::~PostProcessEffectToneMapping()
{
}

void PostProcessEffectToneMapping::OnRender(RenderPass renderPass)
{
    PostProcessEffect::OnRender(renderPass);

    if (MustBeRendered(renderPass))
    {
        GL::Push(GL::BindTarget::SHADER_PROGRAM);

        GEngine *ge = GEngine::GetInstance();
        ShaderProgram *sp = m_toneMappingShaderProgram.Get();
        sp->Bind();
        sp->SetFloat("B_Exposure", GetExposure());

        ge->GetActiveGBuffer()->ApplyPass(sp, true);

        GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    }
}

void PostProcessEffectToneMapping::SetExposure(float exposure)
{
    m_exposure = exposure;
}

float PostProcessEffectToneMapping::GetExposure() const
{
    return m_exposure;
}

void PostProcessEffectToneMapping::Reflect()
{
    PostProcessEffect::Reflect();

    GetReflectStructPtr()
        ->GetReflectVariablePtr("PostProcess Shader")
        ->SetHints(BANG_REFLECT_HINT_SHOWN(false));
    GetReflectStructPtr()->GetReflectVariablePtr("Type")->SetHints(
        BANG_REFLECT_HINT_SHOWN(false));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectToneMapping,
                                   "Exposure",
                                   SetExposure,
                                   GetExposure,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f) +
                                       BANG_REFLECT_HINT_STEP_VALUE(0.02f));
}
