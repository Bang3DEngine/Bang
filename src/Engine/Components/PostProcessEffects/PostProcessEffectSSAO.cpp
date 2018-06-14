#include "Bang/PostProcessEffectSSAO.h"

#include "Bang/GL.h"
#include "Bang/Scene.h"
#include "Bang/Shader.h"
#include "Bang/AARect.h"
#include "Bang/Camera.h"
#include "Bang/GEngine.h"
#include "Bang/GBuffer.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/GLUniforms.h"
#include "Bang/Framebuffer.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

PostProcessEffectSSAO::PostProcessEffectSSAO()
{
    m_ssaoFB = new Framebuffer();
    m_ssaoFB->CreateAttachmentTex2D(GL::Attachment::COLOR0,
                               GL::ColorFormat::RGB10_A2);
    m_ssaoFB->CreateAttachmentTex2D(GL::Attachment::COLOR1,
                               GL::ColorFormat::RGB10_A2);

    SetBlurRadius(1);
    SetSSAORadius(1);
    SetNumRandomAxes(8);
    SetNumRandomSamples(16);

    m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR0)->
              SetWrapMode(GL::WrapMode::REPEAT);
    m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR1)->
              SetWrapMode(GL::WrapMode::REPEAT);

    p_ssaoShaderProgram.Set( ShaderProgramFactory::Get(
           ShaderProgramFactory::GetScreenPassVertexShaderPath(),
           EPATH("Shaders/SSAO.frag") ) );
    p_blurXShaderProgram.Set( ShaderProgramFactory::Get(
           ShaderProgramFactory::GetScreenPassVertexShaderPath(),
           EPATH("Shaders/SSAOSeparableBlurX.frag") ) );
    p_blurYShaderProgram.Set( ShaderProgramFactory::Get(
           ShaderProgramFactory::GetScreenPassVertexShaderPath(),
           EPATH("Shaders/SSAOSeparableBlurY.frag") ) );
    p_applySSAOShaderProgram.Set( ShaderProgramFactory::Get(
           ShaderProgramFactory::GetScreenPassVertexShaderPath(),
           EPATH("Shaders/SSAOApply.frag") ) );
}

PostProcessEffectSSAO::~PostProcessEffectSSAO()
{
    delete m_ssaoFB;
}

#include "Bang/Input.h"
void PostProcessEffectSSAO::OnRender(RenderPass renderPass)
{
    Component::OnRender(renderPass);

    if ( MustBeRendered(renderPass) )
    {
        GL::Push(GL::Pushable::VIEWPORT);
        GL::Push(GL::BindTarget::SHADER_PROGRAM);
        GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        // Get references
        GBuffer *gbuffer = GEngine::GetActiveGBuffer();

        // Prepare state
        m_ssaoFB->Bind();
        SetFBSize( Vector2(GL::GetViewportSize())  );
        GL::SetViewport(0, 0, GetFBSize().x, GetFBSize().y);

        // First do SSAO
        {
            p_ssaoShaderProgram.Get()->Bind(); // Bind shader

            // Bind random textures and set uniforms
            Vector2 randomAxesUvMult ((Vector2(GL::GetViewportSize()) /
                         Vector2(m_randomAxesTexture.Get()->GetSize())) );
            p_ssaoShaderProgram.Get()->SetFloat("B_SSAOIntensity", GetSSAOIntensity());
            p_ssaoShaderProgram.Get()->SetFloat("B_SSAORadius", GetSSAORadius());
            p_ssaoShaderProgram.Get()->SetVector2("B_RandomAxesUvMultiply",
                                                  randomAxesUvMult);
            p_ssaoShaderProgram.Get()->SetTexture2D("B_RandomAxes",
                                                    m_randomAxesTexture.Get());
            p_ssaoShaderProgram.Get()->SetInt("B_NumRandomOffsets",
                                              GetNumRandomSamples() );
            p_ssaoShaderProgram.Get()->SetVector3Array("B_RandomHemisphereOffsetsArray",
                                                       m_randomHemisphereOffsets);

            m_ssaoFB->SetDrawBuffers({GL::Attachment::COLOR0});
            gbuffer->BindAttachmentsForReading(p_ssaoShaderProgram.Get());
            GEngine::GetInstance()->RenderViewportPlane(); // Render Pass!
        }

        // Then blur separatedly, first X, then Y
        if (GetBlurRadius() > 0)
        {
            // Blur in X
            p_blurXShaderProgram.Get()->Bind();
            gbuffer->BindAttachmentsForReading(p_blurXShaderProgram.Get());
            p_blurXShaderProgram.Get()->SetBool("B_BilateralEnabled",
                                                GetBilateralBlurEnabled());
            p_blurXShaderProgram.Get()->SetFloatArray("B_BlurKernel", m_blurKernel);
            p_blurXShaderProgram.Get()->SetInt("B_BlurRadius", GetBlurRadius());

            m_ssaoFB->SetDrawBuffers({GL::Attachment::COLOR1});
            p_blurXShaderProgram.Get()->SetTexture2D(GBuffer::GetColorsTexName(),
                        m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR0));
            GEngine::GetInstance()->RenderViewportPlane(); // Render blur X!

            // Blur in Y
            p_blurYShaderProgram.Get()->Bind();
            gbuffer->BindAttachmentsForReading(p_blurYShaderProgram.Get());
            p_blurYShaderProgram.Get()->SetBool("B_BilateralEnabled",
                                                GetBilateralBlurEnabled());
            p_blurYShaderProgram.Get()->SetFloatArray("B_BlurKernel", m_blurKernel);
            p_blurYShaderProgram.Get()->SetInt("B_BlurRadius", GetBlurRadius());
            p_blurYShaderProgram.Get()->SetTexture2D(GBuffer::GetColorsTexName(),
                                                     GetSSAOTexture());

            m_ssaoFB->SetDrawBuffers({GL::Attachment::COLOR0});
            p_blurYShaderProgram.Get()->SetTexture2D(GBuffer::GetColorsTexName(),
                        m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR1));
            GEngine::GetInstance()->RenderViewportPlane(); // Render blur Y!
        }

        GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

        // Then apply actual SSAO blending with occlusion calculated above!
        {
            p_applySSAOShaderProgram.Get()->Bind();
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

        // Recompute blur kernel
        double sum = 0.0;
        m_blurKernel.Clear();
        for (int i = -GetBlurRadius(); i <= GetBlurRadius(); ++i)
        {
            float k = std::exp(-0.5 * Math::Pow(double(i), 2.0)/3);
            m_blurKernel.PushBack(k);
            sum += m_blurKernel.Back();
        }

        for (int i = 0; i < m_blurKernel.Size(); ++i)
        { m_blurKernel[i] /= sum; } // Normalize
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
            Vector3 randV = Vector3::Zero;
            if (GetSeparable())
            {
                int j = (i > GetNumRandomSamples()/2) ? 1 : 0;
                randV[j] = Random::GetRange(-1.0f, 1.0f);
                randV.z  = Random::GetRange( 0.2f, 1.0f);
            }
            else
            {
                randV.x = Random::GetRange(-1.0f, 1.0f);
                randV.y = Random::GetRange(-1.0f, 1.0f);
                randV.z = Random::GetRange( 0.2f, 1.0f);
            }
            randV = randV.NormalizedSafe();

            // Scale exponentially close to zero, so that there are more closer
            // samples (and consequently have greater weight)
            float scale = i / SCAST<float>( GetNumRandomSamples() );
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
        GenerateRandomAxesTexture( GetNumRandomAxes() );
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

void PostProcessEffectSSAO::SetBilateralBlurEnabled(bool bilateralBlurEnabled)
{
    m_bilateralBlurEnabled = bilateralBlurEnabled;
}

void PostProcessEffectSSAO::SetFBSize(const Vector2 &fbSize)
{
    m_fbSize = fbSize;
    m_ssaoFB->Resize(GetFBSize().x, GetFBSize().y);
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

bool PostProcessEffectSSAO::GetBilateralBlurEnabled() const
{
    return m_bilateralBlurEnabled;
}

int PostProcessEffectSSAO::GetNumRandomSamples() const
{
    return m_numRandomOffsetsHemisphere;
}

const Vector2 &PostProcessEffectSSAO::GetFBSize() const
{
    return m_fbSize;
}

Texture2D* PostProcessEffectSSAO::GetSSAOTexture() const
{
    return m_ssaoFB->GetAttachmentTex2D(GL::Attachment::COLOR0);
}

void PostProcessEffectSSAO::ReloadShaders()
{
    Resources::Import(p_ssaoShaderProgram.Get()->GetVertexShader());
    Resources::Import(p_ssaoShaderProgram.Get()->GetFragmentShader());
    Resources::Import(p_blurXShaderProgram.Get()->GetVertexShader());
    Resources::Import(p_blurXShaderProgram.Get()->GetFragmentShader());
    Resources::Import(p_blurYShaderProgram.Get()->GetVertexShader());
    Resources::Import(p_blurYShaderProgram.Get()->GetFragmentShader());
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

    const int imgSize = Math::Ceil( Math::Sqrt( float(numAxes) ) );
    ASSERT(imgSize > 0);

    // Create an image with the random vectors
    Imageb randomsImg;
    randomsImg.Create(imgSize, imgSize);
    for (int i = 0; i < numAxes; ++i)
    {
        const int x = i % imgSize;
        const int y = i / imgSize;
        const Vector3 &randValue = randomValuesInOrthogonalPlanes[i];
        randomsImg.SetPixel(x, y, Color(randValue, 1));
    }

    // Now create the texture from the image!
    m_randomAxesTexture = Resources::Create<Texture2D>();
    m_randomAxesTexture.Get()->Resize(imgSize, imgSize);
    m_randomAxesTexture.Get()->Import(randomsImg);
    m_randomAxesTexture.Get()->SetWrapMode(GL::WrapMode::REPEAT);
    m_randomAxesTexture.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
}
