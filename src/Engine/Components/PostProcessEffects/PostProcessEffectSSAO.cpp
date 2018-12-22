#include "Bang/PostProcessEffectSSAO.h"

#include <cmath>
#include <istream>

#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/ClassDB.h"
#include "Bang/Color.h"
#include "Bang/Component.h"
#include "Bang/Framebuffer.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/Image.h"
#include "Bang/ImageIO.h"
#include "Bang/Math.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Paths.h"
#include "Bang/Random.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"
#include "Bang/Vector2.h"

namespace Bang
{
class ICloneable;
}

using namespace Bang;

PostProcessEffectSSAO::PostProcessEffectSSAO()
{
    SET_INSTANCE_CLASS_ID(PostProcessEffectSSAO)

    m_ssaoFB = new Framebuffer();
    m_ssaoFB->CreateAttachmentTex2D(GL::Attachment::COLOR0,
                                    GL::ColorFormat::RGB10_A2);
    m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR0)
        ->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

    m_blurAuxiliarTexture = Assets::Create<Texture2D>();
    m_blurAuxiliarTexture.Get()->SetFormat(GL::ColorFormat::RGB10_A2);
    m_blurAuxiliarTexture.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

    m_blurredSSAOTexture = Assets::Create<Texture2D>();
    m_blurredSSAOTexture.Get()->SetFormat(GL::ColorFormat::RGB10_A2);
    m_blurredSSAOTexture.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);

    SetBlurRadius(1);
    SetSSAORadius(1);
    SetNumRandomAxes(8);
    SetNumRandomSamples(16);

    m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR0)
        ->SetWrapMode(GL::WrapMode::REPEAT);

    p_ssaoShaderProgram.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        EPATH("Shaders").Append("SSAO.frag")));
    p_applySSAOShaderProgram.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        EPATH("Shaders").Append("SSAOApply.frag")));

    SetType(PostProcessEffect::Type::AFTER_SCENE_AND_LIGHT);
}

PostProcessEffectSSAO::~PostProcessEffectSSAO()
{
    delete m_ssaoFB;
}

void PostProcessEffectSSAO::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);

    if (MustBeRendered(renderPass))
    {
        GL::Push(GL::Pushable::VIEWPORT);
        GL::Push(GL::BindTarget::SHADER_PROGRAM);
        GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        // Get references
        GBuffer *gbuffer = GEngine::GetActiveGBuffer();

        // Prepare state
        m_ssaoFB->Bind();
        SetFBSize(Vector2(GL::GetViewportSize()));
        GL::SetViewport(0, 0, GetFBSize().x, GetFBSize().y);

        // First do SSAO
        {
            p_ssaoShaderProgram.Get()->Bind();  // Bind shader

            // Bind random textures and set uniforms
            Vector2 randomAxesUvMult(
                (Vector2(GL::GetViewportSize()) /
                 Vector2(m_randomAxesTexture.Get()->GetSize())));
            p_ssaoShaderProgram.Get()->SetFloat("B_SSAORadius",
                                                GetSSAORadius());
            p_ssaoShaderProgram.Get()->SetVector2("B_RandomAxesUvMultiply",
                                                  randomAxesUvMult);
            p_ssaoShaderProgram.Get()->SetTexture2D("B_RandomAxes",
                                                    m_randomAxesTexture.Get());
            p_ssaoShaderProgram.Get()->SetInt("B_NumRandomOffsets",
                                              GetNumRandomSamples());
            p_ssaoShaderProgram.Get()->SetVector3Array(
                "B_RandomHemisphereOffsetsArray", m_randomHemisphereOffsets);

            GL::WrapMode prevDepthWrapMode =
                gbuffer->GetSceneDepthStencilTexture()->GetWrapMode();
            gbuffer->GetSceneDepthStencilTexture()->SetWrapMode(
                GL::WrapMode::CLAMP_TO_EDGE);

            m_ssaoFB->SetDrawBuffers({GL::Attachment::COLOR0});
            gbuffer->BindAttachmentsForReading(p_ssaoShaderProgram.Get());
            GEngine::GetInstance()->RenderViewportPlane();  // Render Pass!

            gbuffer->GetSceneDepthStencilTexture()->SetWrapMode(
                prevDepthWrapMode);
        }

        // Then blur
        if (GetBlurRadius() > 0)
        {
            GEngine *ge = GEngine::GetInstance();
            ge->BlurTexture(
                m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR0),
                m_blurAuxiliarTexture.Get(),
                m_blurredSSAOTexture.Get(),
                GetBlurRadius(),
                BlurType::KAWASE);
        }

        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        // Then apply actual SSAO blending with occlusion calculated above!
        {
            p_applySSAOShaderProgram.Get()->Bind();
            p_applySSAOShaderProgram.Get()->SetFloat("B_SSAOIntensity",
                                                     GetSSAOIntensity());
            p_applySSAOShaderProgram.Get()->SetTexture2D("B_SSAOMap",
                                                         GetSSAOTexture());
            gbuffer->ApplyPass(p_applySSAOShaderProgram.Get(), true);
        }

        GL::Pop(GL::BindTarget::SHADER_PROGRAM);
        GL::Pop(GL::Pushable::VIEWPORT);
    }
}

void PostProcessEffectSSAO::SetSSAORadius(float radius)
{
    m_ssaoRadius = radius;
}

void PostProcessEffectSSAO::SetSSAOIntensity(float ssaoIntensity)
{
    m_ssaoIntensity = ssaoIntensity;
}

void PostProcessEffectSSAO::SetBlurRadius(int blurRadius)
{
    if (blurRadius != GetBlurRadius())
    {
        m_blurRadius = blurRadius;
    }
}

void PostProcessEffectSSAO::SetNumRandomSamples(int numRandomSamples)
{
    if (numRandomSamples != GetNumRandomSamples())
    {
        m_numRandomOffsetsHemisphere = numRandomSamples;

        // Generate offset vectors in two orthogonal planes, to do
        // "separable" SSAO. Then, the random rotation will rotate them, and the
        // blur will do the effect of merging the two separable planes. A bit
        // weird but should work
        m_randomHemisphereOffsets.Clear();
        for (int i = 0; i < GetNumRandomSamples(); ++i)
        {
            Vector3 randV = Vector3::Zero();
            if (GetSeparable())
            {
                int j = (i > GetNumRandomSamples() / 2) ? 1 : 0;
                randV[j] = Random::GetRange(-1.0f, 1.0f);
                randV.z = Random::GetRange(0.2f, 1.0f);
            }
            else
            {
                randV.x = Random::GetRange(-1.0f, 1.0f);
                randV.y = Random::GetRange(-1.0f, 1.0f);
                randV.z = Random::GetRange(0.2f, 1.0f);
            }
            randV = randV.NormalizedSafe();

            // Scale exponentially close to zero, so that there are more closer
            // samples (and consequently have greater weight)
            float scale = i / SCAST<float>(GetNumRandomSamples());
            scale = Math::Lerp(0.1f, 1.0f, scale * scale);
            randV *= scale;

            m_randomHemisphereOffsets.PushBack(randV);
        }
    }
}

void PostProcessEffectSSAO::SetNumRandomAxes(int numAxes)
{
    if (numAxes != GetNumRandomAxes())
    {
        m_numAxes = numAxes;
        GenerateRandomAxesTexture(GetNumRandomAxes());
    }
}

void PostProcessEffectSSAO::SetSeparable(bool separable)
{
    if (separable != GetSeparable())
    {
        m_separable = separable;

        // Recalculate
        int numRandomSamples = GetNumRandomSamples();
        SetNumRandomSamples(0);
        SetNumRandomSamples(numRandomSamples);
    }
}

void PostProcessEffectSSAO::SetFBSize(const Vector2 &fbSize)
{
    m_fbSize = fbSize;
    m_ssaoFB->Resize(SCAST<int>(GetFBSize().x), SCAST<int>(GetFBSize().y));
}

bool PostProcessEffectSSAO::GetSeparable() const
{
    return m_separable;
}

int PostProcessEffectSSAO::GetBlurRadius() const
{
    return m_blurRadius;
}

float PostProcessEffectSSAO::GetSSAORadius() const
{
    return m_ssaoRadius;
}

float PostProcessEffectSSAO::GetSSAOIntensity() const
{
    return m_ssaoIntensity;
}

int PostProcessEffectSSAO::GetNumRandomAxes() const
{
    return m_numAxes;
}

int PostProcessEffectSSAO::GetNumRandomSamples() const
{
    return m_numRandomOffsetsHemisphere;
}

const Vector2 &PostProcessEffectSSAO::GetFBSize() const
{
    return m_fbSize;
}

Texture2D *PostProcessEffectSSAO::GetSSAOTexture() const
{
    return (GetBlurRadius() > 0
                ? m_blurredSSAOTexture.Get()
                : m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR0));
}

void PostProcessEffectSSAO::Reflect()
{
    PostProcessEffect::Reflect();

    GetReflectStructPtr()
        ->GetReflectVariablePtr("PostProcess Shader")
        ->SetHints(BANG_REFLECT_HINT_SHOWN(false));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectSSAO,
                                   "Intensity",
                                   SetSSAOIntensity,
                                   GetSSAOIntensity,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f) +
                                       BANG_REFLECT_HINT_STEP_VALUE(0.05f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectSSAO,
                                   "Radius",
                                   SetSSAORadius,
                                   GetSSAORadius,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f) +
                                       BANG_REFLECT_HINT_STEP_VALUE(0.01f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectSSAO,
                                   "Blur radius",
                                   SetBlurRadius,
                                   GetBlurRadius,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PostProcessEffectSSAO,
                                   "Num samples",
                                   SetNumRandomSamples,
                                   GetNumRandomSamples,
                                   BANG_REFLECT_HINT_MIN_VALUE(1.0f));
}

void PostProcessEffectSSAO::GenerateRandomAxesTexture(int numAxes)
{
    // Generate random axes vectors
    Array<Vector3> randomValuesInOrthogonalPlanes;
    for (int i = 0; i < numAxes; ++i)
    {
        Vector3 randomAxesVector(Random::GetRange(0.0f, 1.0f),
                                 Random::GetRange(0.0f, 1.0f),
                                 Random::GetRange(0.0f, 1.0f));
        randomValuesInOrthogonalPlanes.PushBack(randomAxesVector);
    }

    const int imgSize = SCAST<int>(Math::Ceil(Math::Sqrt(float(numAxes))));
    ASSERT(imgSize > 0);

    // Create an image with the random vectors
    Image randomsImg;
    randomsImg.Create(imgSize, imgSize);
    for (int i = 0; i < numAxes; ++i)
    {
        const int x = i % imgSize;
        const int y = i / imgSize;
        const Vector3 &randValue = randomValuesInOrthogonalPlanes[i];
        randomsImg.SetPixel(x, y, Color(randValue, 1));
    }

    // Now create the texture from the image!
    m_randomAxesTexture = Assets::Create<Texture2D>();
    m_randomAxesTexture.Get()->Resize(imgSize, imgSize);
    m_randomAxesTexture.Get()->Import(randomsImg);
    m_randomAxesTexture.Get()->SetWrapMode(GL::WrapMode::REPEAT);
    m_randomAxesTexture.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
}
