#include "Bang/PostProcessEffectSSAO.h"

#include "Bang/GL.h"
#include "Bang/Scene.h"
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
    m_ssaoFB->CreateAttachment(GL::Attachment::Color0,
                               GL::ColorFormat::RGB10_A2_UByte);
    m_ssaoFB->CreateAttachment(GL::Attachment::Color1,
                               GL::ColorFormat::RGB10_A2_UByte);

    SetBlurRadius(1);
    SetSSAORadius(1);
    SetNumRandomRotations(8);
    SetNumRandomSamples(16);

    m_ssaoFB->GetAttachmentTexture(GL::Attachment::Color0)->
              SetWrapMode(GL::WrapMode::Repeat);
    m_ssaoFB->GetAttachmentTexture(GL::Attachment::Color1)->
              SetWrapMode(GL::WrapMode::Repeat);

    p_ssaoShaderProgram = ShaderProgramFactory::Get(
           ShaderProgramFactory::GetPostProcessVertexShaderPath(),
           EPATH("Shaders/SSAO.frag") );
    p_blurXShaderProgram = ShaderProgramFactory::Get(
           ShaderProgramFactory::GetPostProcessVertexShaderPath(),
           EPATH("Shaders/SSAOSeparableBlurX.frag") );
    p_blurYShaderProgram = ShaderProgramFactory::Get(
           ShaderProgramFactory::GetPostProcessVertexShaderPath(),
           EPATH("Shaders/SSAOSeparableBlurY.frag") );
    p_applySSAOShaderProgram = ShaderProgramFactory::Get(
           ShaderProgramFactory::GetPostProcessVertexShaderPath(),
           EPATH("Shaders/SSAOApply.frag") );
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
        // Save state
        GLId prevSP = GL::GetBoundId( GL::BindTarget::ShaderProgram );
        GLId prevFB = GL::GetBoundId( GL::BindTarget::Framebuffer );
        AARecti prevVP = GL::GetViewportRect();

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
            Vector2 randomRotsUvMult ((Vector2(GL::GetViewportSize()) /
                         Vector2(m_randomRotationsTexture.Get()->GetSize())) );
            p_ssaoShaderProgram.Get()->Set("B_SSAOIntensity", GetSSAOIntensity());
            p_ssaoShaderProgram.Get()->Set("B_SSAORadius", GetSSAORadius());
            p_ssaoShaderProgram.Get()->Set("B_RandomRotationsUvMultiply",
                                           randomRotsUvMult);
            p_ssaoShaderProgram.Get()->Set("B_RandomRotations",
                                           m_randomRotationsTexture.Get());
            p_ssaoShaderProgram.Get()->Set("B_NumRandomOffsets",
                                           GetNumRandomSamples() );
            p_ssaoShaderProgram.Get()->Set("B_RandomHemisphereOffsetsArray",
                                           m_randomHemisphereOffsets);

            m_ssaoFB->SetDrawBuffers({GL::Attachment::Color0});
            gbuffer->BindAttachmentsForReading(p_ssaoShaderProgram.Get());
            GEngine::GetActive()->RenderViewportPlane(); // Render Pass!
        }

        // Then blur separatedly, first X, then Y
        if (GetBlurRadius() > 0)
        {
            // Blur in X
            p_blurXShaderProgram.Get()->Bind();
            gbuffer->BindAttachmentsForReading(p_blurXShaderProgram.Get());
            p_blurXShaderProgram.Get()->Set("B_BilateralEnabled",
                                            GetBilateralBlurEnabled());
            p_blurXShaderProgram.Get()->Set("B_BlurKernel", m_blurKernel);
            p_blurXShaderProgram.Get()->Set("B_BlurRadius", GetBlurRadius());

            m_ssaoFB->SetDrawBuffers({GL::Attachment::Color1});
            p_blurXShaderProgram.Get()->Set(GBuffer::GetColorsTexName(),
                        m_ssaoFB->GetAttachmentTexture(GL::Attachment::Color0));
            GEngine::GetActive()->RenderViewportPlane(); // Render blur X!

            // Blur in Y
            p_blurYShaderProgram.Get()->Bind();
            gbuffer->BindAttachmentsForReading(p_blurYShaderProgram.Get());
            p_blurYShaderProgram.Get()->Set("B_BilateralEnabled",
                                            GetBilateralBlurEnabled());
            p_blurYShaderProgram.Get()->Set("B_BlurKernel", m_blurKernel);
            p_blurYShaderProgram.Get()->Set("B_BlurRadius", GetBlurRadius());
            p_blurYShaderProgram.Get()->Set(GBuffer::GetColorsTexName(),
                                            GetSSAOTexture());

            m_ssaoFB->SetDrawBuffers({GL::Attachment::Color0});
            p_blurYShaderProgram.Get()->Set(GBuffer::GetColorsTexName(),
                        m_ssaoFB->GetAttachmentTexture(GL::Attachment::Color1));
            GEngine::GetActive()->RenderViewportPlane(); // Render blur Y!
        }

        // Restore previous framebuffer
        GL::Bind( GL::BindTarget::Framebuffer, prevFB );

        // Then apply actual SSAO blending with occlusion calculated above!
        {
            p_applySSAOShaderProgram.Get()->Bind();
            p_applySSAOShaderProgram.Get()->Set("B_SSAOMap", GetSSAOTexture());
            gbuffer->ApplyPass(p_applySSAOShaderProgram.Get(), true);
        }

        // Restore state
        GL::Bind( GL::BindTarget::ShaderProgram, prevSP );
        GL::SetViewport(prevVP);
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
            int j = (i > GetNumRandomSamples()/2) ? 1 : 0;
            Vector3 randV = Vector3::Zero;
            randV[j] = Random::GetRange(-1.0f, 1.0f);
            randV.z  = Random::GetRange( 0.0f, 1.0f);
            randV = randV.NormalizedSafe();
            m_randomHemisphereOffsets.PushBack(randV);
        }

        // Scale exponentially close to zero, so that there are more closer
        // samples (and consequently have greater weight)
        for (int i = 0; i < GetNumRandomSamples(); ++i)
        {
            float scale = i / SCAST<float>( GetNumRandomSamples() );
            scale = Math::Lerp(0.1f, 1.0f, scale * scale);
            m_randomHemisphereOffsets[i] *= scale;
        }
    }
}

void PostProcessEffectSSAO::SetNumRandomRotations(int numRotations)
{
    if (numRotations != GetNumRandomRotations())
    {
        m_numRotations = numRotations;
        GenerateRandomRotationsTexture( GetNumRandomRotations() );
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

int PostProcessEffectSSAO::GetNumRandomRotations() const
{
    return m_numRotations;
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
    return m_ssaoFB->GetAttachmentTexture(GL::Attachment::Color0);
}

void PostProcessEffectSSAO::GenerateRandomRotationsTexture(int numRotations)
{
    // Generate random rotation vectors
    Array<Vector3> randomValuesInOrthogonalPlanes;
    for (int i = 0; i < numRotations; ++i)
    {
        Vector3 randomRotationVector(Random::GetRange(0.0f, 1.0f),
                                     Random::GetRange(0.0f, 1.0f),
                                     Random::GetRange(0.0f, 0.0f));
        randomValuesInOrthogonalPlanes.PushBack(randomRotationVector);
    }

    const int imgSize = Math::Ceil( Math::Sqrt( float(numRotations) ) );
    ASSERT(imgSize > 0);

    // Create an image with the random vectors
    Imageb randomsImg;
    randomsImg.Create(imgSize, imgSize);
    for (int i = 0; i < numRotations; ++i)
    {
        const int x = i % imgSize;
        const int y = i / imgSize;
        const Vector3 &randValue = randomValuesInOrthogonalPlanes[i];
        randomsImg.SetPixel(x, y, Color(randValue, 1));
    }

    // Now create the texture from the image!
    m_randomRotationsTexture = Resources::Create<Texture2D>();
    m_randomRotationsTexture.Get()->Resize(imgSize, imgSize);
    m_randomRotationsTexture.Get()->Import(randomsImg);
    m_randomRotationsTexture.Get()->SetWrapMode(GL::WrapMode::Repeat);
    m_randomRotationsTexture.Get()->SetFilterMode(GL::FilterMode::Bilinear);
}
