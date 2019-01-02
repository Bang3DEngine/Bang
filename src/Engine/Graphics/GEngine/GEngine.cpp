#include "Bang/GEngine.h"

#include <stack>

#include "Bang/Application.h"
#include "Bang/Assert.h"
#include "Bang/Camera.h"
#include "Bang/DebugRenderer.h"
#include "Bang/EventEmitter.h"
#include "Bang/Framebuffer.h"
#include "Bang/GBuffer.h"
#include "Bang/GL.h"
#include "Bang/GLUniforms.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/IEventsDestroy.h"
#include "Bang/Light.h"
#include "Bang/Material.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/MultiObjectGatherer.tcc"
#include "Bang/Path.h"
#include "Bang/Paths.h"
#include "Bang/PointLight.h"
#include "Bang/ReflectionProbe.h"
#include "Bang/RenderFactory.h"
#include "Bang/RenderFlags.h"
#include "Bang/Renderer.h"
#include "Bang/Scene.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/TextureCubeMap.h"
#include "Bang/TextureUnitManager.h"
#include "Bang/Transform.h"
#include "Bang/USet.tcc"

namespace Bang
{
class IEventsRendererChanged;
}

using namespace Bang;

GEngine::GEngine()
{
    p_renderingCameras.currentValue = nullptr;
}

GEngine::~GEngine()
{
    delete m_auxiliarFramebuffer;
    delete m_auxiliarFramebufferCM;

    if (m_debugRenderer)
    {
        GameObject::DestroyImmediate(m_debugRenderer);
    }

    if (m_renderFactory)
    {
        delete m_renderFactory;
    }

    if (m_texUnitManager)
    {
        delete m_texUnitManager;
    }

    if (m_gl)
    {
        delete m_gl;
    }

    if (m_fillCubeMapFromTexturesFB)
    {
        delete m_fillCubeMapFromTexturesFB;
    }
}

void GEngine::Init()
{
    m_gl = new GL();
    m_gl->Init();

    m_texUnitManager = new TextureUnitManager();
    m_renderFactory = new RenderFactory();
    m_debugRenderer = new DebugRenderer();

    p_windowPlaneMesh = MeshFactory::GetUIPlane();
    p_renderTextureToViewportSP.Set(
        ShaderProgramFactory::GetRenderTextureToViewport());
    p_renderTextureToViewportGammaSP.Set(
        ShaderProgramFactory::GetRenderTextureToViewportGamma());
    m_renderSkySP.Set(ShaderProgramFactory::Get(
        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
        EPATH("Shaders").Append("RenderSky.frag")));

    m_auxiliarFramebuffer = new Framebuffer();
    m_auxiliarFramebufferCM = new Framebuffer();

    p_kawaseBlurSP.Set(ShaderProgramFactory::GetKawaseBlur());
    p_separableGaussianBlurSP.Set(ShaderProgramFactory::GetSeparableBlur());
    p_separableGaussianBlurCubeMapSP.Set(
        ShaderProgramFactory::GetSeparableBlurCubeMap());

    Path shadersDir = ShaderProgramFactory::GetEngineShadersDir();
    m_fillCubeMapFromTexturesFB = new Framebuffer();
    m_fillCubeMapFromTexturesSP.Set(ShaderProgramFactory::Get(
        shadersDir.Append("FillCubeMapFromTextures.vert"),
        shadersDir.Append("FillCubeMapFromTextures.geom"),
        shadersDir.Append("FillCubeMapFromTextures.frag")));
}

void GEngine::Render(GameObject *go)
{
    if (go)
    {
        if (Camera *camera = GetActiveRenderingCamera())
        {
            RenderFlags renderFlags = camera->GetRenderFlags();

            go->BeforeRender();

            if (renderFlags.IsOn(RenderFlag::RENDER_SHADOW_MAPS))
            {
                RenderShadowMaps(go);
            }

            RenderToGBuffer(go, camera);

            if (renderFlags.IsOn(RenderFlag::RENDER_REFLECTION_PROBES))
            {
                RenderReflectionProbes(go);
            }
        }
    }
}

void GEngine::Render(GameObject *go, Camera *camera)
{
    if (go && camera)
    {
        PushActiveRenderingCamera();

        SetActiveRenderingCamera(camera);
        Render(go);

        PopActiveRenderingCamera();
    }
}

void GEngine::ApplyStenciledDeferredLightsToGBuffer(GameObject *lightsContainer,
                                                    Camera *camera,
                                                    const AARect &maskRectNDC)
{
    GL::Push(GL::Pushable::STENCIL_STATES);

    // We have marked from before the zone where we want to apply the effect
    GL::SetStencilOp(GL::StencilOperation::KEEP);
    GL::SetStencilFunc(GL::Function::EQUAL);
    GL::SetStencilValue(1);

    const Array<Light *> &lights =
        m_lightsCache.GetGatheredArray(lightsContainer);
    for (Light *light : lights)
    {
        if (!light || !light->IsEnabledRecursively())
        {
            continue;
        }
        light->ApplyLight(camera, maskRectNDC);
    }

    GL::Pop(GL::Pushable::STENCIL_STATES);
}

void GEngine::RetrieveForwardRenderingInformation(GameObject *go)
{
    m_currentForwardRenderingLightTypes.Clear();
    m_currentForwardRenderingLightColors.Clear();
    m_currentForwardRenderingLightPositions.Clear();
    m_currentForwardRenderingLightForwardDirs.Clear();
    m_currentForwardRenderingLightIntensities.Clear();
    m_currentForwardRenderingLightRanges.Clear();

    int i = 0;
    const Array<Light *> &lights = m_lightsCache.GetGatheredArray(go);
    for (Light *light : lights)
    {
        if (light->IsActiveRecursively())
        {
            uint lightType = 0;
            Transform *lightTR = light->GetGameObject()->GetTransform();
            float range = 0.0f;
            if (PointLight *pl = DCAST<PointLight *>(light))
            {
                range = pl->GetRange();
                lightType = 1;
            }

            m_currentForwardRenderingLightTypes.PushBack(lightType);
            m_currentForwardRenderingLightColors.PushBack(light->GetColor());
            m_currentForwardRenderingLightPositions.PushBack(
                lightTR->GetPosition());
            m_currentForwardRenderingLightForwardDirs.PushBack(
                lightTR->GetForward());
            m_currentForwardRenderingLightIntensities.PushBack(
                light->GetIntensity());
            m_currentForwardRenderingLightRanges.PushBack(range);

            if (++i == 128)
            {
                break;
            }
        }
    }
}

void GEngine::PrepareForForwardRendering(Renderer *rend)
{
    Material *mat = rend->GetActiveMaterial();
    if (ShaderProgram *sp = (mat ? mat->GetShaderProgram() : nullptr))
    {
        ASSERT(GL::IsBound(sp));
        int numLights = m_currentForwardRenderingLightColors.Size();
        if (numLights > 0)
        {
            sp->SetColorArray("B_ForwardRenderingLightColors",
                              m_currentForwardRenderingLightColors,
                              false);
            sp->SetVector3Array("B_ForwardRenderingLightPositions",
                                m_currentForwardRenderingLightPositions,
                                false);
            sp->SetVector3Array("B_ForwardRenderingLightForwardDirs",
                                m_currentForwardRenderingLightForwardDirs,
                                false);
            sp->SetFloatArray("B_ForwardRenderingLightIntensities",
                              m_currentForwardRenderingLightIntensities,
                              false);
            sp->SetFloatArray("B_ForwardRenderingLightRanges",
                              m_currentForwardRenderingLightRanges,
                              false);
            sp->SetIntArray("B_ForwardRenderingLightTypes",
                            m_currentForwardRenderingLightTypes,
                            false);
        }
        sp->SetInt("B_ForwardRenderingLightNumber", numLights, false);
    }
}

void GEngine::SetReplacementMaterial(Material *material)
{
    m_replacementMaterial.Set(material);
}

Material *GEngine::GetReplacementMaterial() const
{
    return m_replacementMaterial.Get();
}

Camera *GEngine::GetActiveRenderingCamera()
{
    GEngine *ge = GEngine::GetInstance();
    return ge ? ge->p_renderingCameras.currentValue : nullptr;
}

const Array<ReflectionProbe *> &GEngine::GetReflectionProbesFor(
    Scene *scene) const
{
    return m_reflProbesCache.GetGatheredArray(scene);
}

GBuffer *GEngine::GetActiveGBuffer()
{
    Camera *cam = GEngine::GetActiveRenderingCamera();
    return cam ? cam->GetGBuffer() : nullptr;
}

DebugRenderer *GEngine::GetDebugRenderer() const
{
    return m_debugRenderer;
}

RenderFactory *GEngine::GetRenderFactory() const
{
    return m_renderFactory;
}

void SetDrawBuffersToClearFromFlags(GBuffer *gbuffer, RenderFlags renderFlags)
{
    ASSERT(GL::IsBound(gbuffer));

    Array<GL::Attachment> clearAttachments;

    if (renderFlags.IsOn(RenderFlag::CLEAR_ALBEDO))
    {
        clearAttachments.PushBack(GBuffer::AttAlbedo);
    }

    if (renderFlags.IsOn(RenderFlag::CLEAR_LIGHT))
    {
        clearAttachments.PushBack(GBuffer::AttLight);
    }

    if (renderFlags.IsOn(RenderFlag::CLEAR_COLOR))
    {
        clearAttachments.PushBack(GBuffer::AttColor);
    }

    if (renderFlags.IsOn(RenderFlag::CLEAR_MISC))
    {
        clearAttachments.PushBack(GBuffer::AttMisc);
    }

    if (renderFlags.IsOn(RenderFlag::CLEAR_NORMALS))
    {
        clearAttachments.PushBack(GBuffer::AttNormal);
    }

    gbuffer->SetDrawBuffers(clearAttachments);
}

void GEngine::RenderToGBuffer(GameObject *go, Camera *camera)
{
    auto ClearNeededBuffersForTheFirstTime = [this](GBuffer *gbuffer,
                                                    Camera *camera) {
        RenderFlags renderFlags = camera->GetRenderFlags();
        SetDrawBuffersToClearFromFlags(gbuffer, renderFlags);
        Array<GL::Attachment> currentDrawAttachments =
            gbuffer->GetCurrentDrawAttachments();
        currentDrawAttachments.Remove(GBuffer::AttColor);

        if (currentDrawAttachments.Size() > 0)
        {
            gbuffer->SetDrawBuffers(currentDrawAttachments);
            GL::ClearColorBuffer(Color::Zero());  // Clear all except color
        }

        if (camera->GetClearMode() == CameraClearMode::SKY_BOX)
        {
            GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
            GL::Push(GL::BindTarget::SHADER_PROGRAM);

            gbuffer->SetColorDrawBuffer();
            GL::ClearColorBuffer();
            m_renderSkySP.Get()->Bind();
            gbuffer->ApplyPass(m_renderSkySP.Get(), false);

            GL::Pop(GL::BindTarget::SHADER_PROGRAM);
            GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
        }
        else  // COLOR
        {
            if (renderFlags.IsOn(RenderFlag::CLEAR_COLOR))
            {
                gbuffer->SetColorDrawBuffer();
                GL::ClearColorBuffer(camera->GetClearColor());
            }
        }
    };

    auto ClearDepthStencilIfNeeded = [](RenderFlags renderFlags) {
        if (renderFlags.IsOn(RenderFlag::CLEAR_DEPTH_STENCIL))
        {
            GL::ClearStencilDepthBuffers();
        }
    };

    const bool needToChangeCamera = (camera != GetActiveRenderingCamera());
    if (needToChangeCamera)
    {
        PushActiveRenderingCamera();
        SetActiveRenderingCamera(camera);
    }

    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::Pushable::DEPTH_STATES);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    GL::SetViewport(0, 0, camera->GetRenderSize().x, camera->GetRenderSize().y);

    GBuffer *gbuffer = camera->GetGBuffer();

    gbuffer->Bind();
    gbuffer->PushDepthStencilTexture();

    RenderFlags renderFlags = camera->GetRenderFlags();
    ClearNeededBuffersForTheFirstTime(gbuffer, camera);

    // GBuffer Scene rendering
    if (camera->MustRenderPass(RenderPass::SCENE_OPAQUE) ||
        camera->MustRenderPass(RenderPass::SCENE_TRANSPARENT))
    {
        gbuffer->SetSceneDepthStencil();
        ClearDepthStencilIfNeeded(renderFlags);

        GL::SetDepthMask(true);
        GL::SetDepthFunc(GL::Function::LEQUAL);

        // Render scene pass
        if (camera->MustRenderPass(RenderPass::SCENE_OPAQUE))
        {
            gbuffer->SetAllDrawBuffers();
            RenderWithPassAndMarkStencilForLights(go, RenderPass::SCENE_OPAQUE);
        }

        if (camera->MustRenderPass(RenderPass::SCENE_DECALS))
        {
            GL::Push(GL::Pushable::STENCIL_STATES);
            GL::Push(GL::Pushable::BLEND_STATES);

            gbuffer->SetDrawBuffers({GBuffer::AttAlbedo, GBuffer::AttColor});

            GL::Enable(GL::Enablable::BLEND);
            GL::BlendFunc(GL::BlendFactor::SRC_ALPHA,
                          GL::BlendFactor::ONE_MINUS_SRC_ALPHA);

            GL::Disable(GL::Enablable::STENCIL_TEST);
            RenderWithPass(go, RenderPass::SCENE_DECALS);

            GL::Pop(GL::Pushable::BLEND_STATES);
            GL::Pop(GL::Pushable::STENCIL_STATES);
        }

        if (camera->MustRenderPass(RenderPass::SCENE_OPAQUE))
        {
            gbuffer->SetAllDrawBuffers();
            ApplyStenciledDeferredLightsToGBuffer(go, camera);
        }

        // Render scene transparent
        if (camera->MustRenderPass(RenderPass::SCENE_TRANSPARENT))
        {
            RetrieveForwardRenderingInformation(go);
            gbuffer->SetColorDrawBuffer();
            RenderTransparentPass(go);
        }

        // Render scene postprocess
        if (camera->MustRenderPass(RenderPass::SCENE_BEFORE_ADDING_LIGHTS))
        {
            gbuffer->SetColorDrawBuffer();
            RenderWithPass(go, RenderPass::SCENE_BEFORE_ADDING_LIGHTS);
        }

        gbuffer->SetColorDrawBuffer();
        GL::Enable(GL::Enablable::BLEND);
        GL::BlendFunc(GL::BlendFactor::ONE, GL::BlendFactor::ONE);
        RenderTexture(gbuffer->GetAttachmentTex2D(GBuffer::AttLight));
        GL::Disable(GL::Enablable::BLEND);

        if (camera->MustRenderPass(RenderPass::SCENE_AFTER_ADDING_LIGHTS))
        {
            gbuffer->SetColorDrawBuffer();
            RenderWithPass(go, RenderPass::SCENE_AFTER_ADDING_LIGHTS);
        }
    }

    // Enable blend for transparent stuff from now on
    GL::Enablei(GL::Enablable::BLEND, 0);
    GL::BlendFunc(GL::BlendFactor::SRC_ALPHA,
                  GL::BlendFactor::ONE_MINUS_SRC_ALPHA);

    // GBuffer Canvas rendering
    if (camera->MustRenderPass(RenderPass::CANVAS))
    {
        gbuffer->SetCanvasDepthStencil();
        ClearDepthStencilIfNeeded(renderFlags);

        GL::SetDepthMask(true);
        GL::SetDepthFunc(GL::Function::LEQUAL);

        gbuffer->SetColorDrawBuffer();
        RenderWithPass(go, RenderPass::CANVAS);
        if (camera->MustRenderPass(RenderPass::CANVAS_POSTPROCESS))
        {
            RenderWithPass(go, RenderPass::CANVAS_POSTPROCESS);
        }
    }

    if (camera->MustRenderPass(RenderPass::OVERLAY))
    {
        // GBuffer Overlay rendering
        GL::Enable(GL::Enablable::BLEND);
        gbuffer->SetAllDrawBuffers();
        gbuffer->SetOverlayDepthStencil();
        ClearDepthStencilIfNeeded(renderFlags);

        GL::SetDepthMask(false);
        GL::SetDepthFunc(GL::Function::LEQUAL);

        RenderWithPass(go, RenderPass::OVERLAY);
        if (camera->MustRenderPass(RenderPass::OVERLAY_POSTPROCESS))
        {
            RenderWithPass(go, RenderPass::OVERLAY_POSTPROCESS);
        }
    }

    if (camera->GetGammaCorrection() != 1.0f)
    {
        ApplyGammaCorrection(camera->GetGBuffer(),
                             camera->GetGammaCorrection());
    }

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::BLEND_STATES);
    GL::Pop(GL::Pushable::VIEWPORT);
    gbuffer->PopDepthStencilTexture();

    if (needToChangeCamera)
    {
        PopActiveRenderingCamera();
    }
}

void GEngine::RenderWithPass(GameObject *go,
                             RenderPass renderPass,
                             bool renderChildren)
{
    go->Render(renderPass, renderChildren);
}

void GEngine::RenderWithPassAndMarkStencilForLights(GameObject *go,
                                                    RenderPass renderPass)
{
    GL::Push(GL::Pushable::STENCIL_STATES);

    GL::SetStencilValue(1);
    GL::SetStencilOp(GL::StencilOperation::REPLACE);

    RenderWithPass(go, renderPass);

    GL::Pop(GL::Pushable::STENCIL_STATES);
}

Array<float> GetGaussianBlurKernel(int blurRadius)
{
    Array<float> blurKernel;
    {
        // intialising standard deviation to 1.0
        const float sigma = blurRadius;
        const float s = 2.0 * sigma * sigma;

        float sum = 0.0;
        for (int i = -blurRadius; i <= blurRadius; i++)
        {
            const float r = sqrt(i * i + i * i);
            const float value = Math::Exp(-(r * r) / s) / (Math::Pi * s);
            blurKernel.PushBack(value);
            sum += value;
        }

        for (uint i = 0; i < blurKernel.Size(); ++i)
        {
            blurKernel[i] /= sum;  // Normalize
        }
    }
    return blurKernel;
}

void GEngine::BlurTexture(Texture2D *inputTexture,
                          Texture2D *auxiliarTexture,
                          Texture2D *blurredOutputTexture,
                          uint blurRadius,
                          BlurType blurType) const
{
    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::COLOR_MASK);
    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::BindTarget::SHADER_PROGRAM);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    GL::Disable(GL::Enablable::BLEND);

    m_auxiliarFramebuffer->Bind();
    auxiliarTexture->Resize(inputTexture->GetSize());
    blurredOutputTexture->Resize(inputTexture->GetSize());

    ShaderProgram *blurSP =
        (blurType == BlurType::GAUSSIAN ? p_separableGaussianBlurSP.Get()
                                        : p_kawaseBlurSP.Get());
    blurSP->Bind();
    blurSP->SetVector2("B_InputTextureSize", Vector2(inputTexture->GetSize()));

    GL::SetViewport(0, 0, inputTexture->GetSize().x, inputTexture->GetSize().y);

    if (blurType == BlurType::GAUSSIAN)
    {
        m_auxiliarFramebuffer->SetAttachmentTexture(blurredOutputTexture,
                                                    GL::Attachment::COLOR0);
        m_auxiliarFramebuffer->SetAttachmentTexture(auxiliarTexture,
                                                    GL::Attachment::COLOR1);

        const Array<float> blurKernel = GetGaussianBlurKernel(blurRadius);
        blurSP->SetInt("B_BlurRadius", blurRadius);
        blurSP->SetFloatArray("B_BlurKernel", blurKernel);

        // Render blur X
        blurSP->SetBool("B_BlurInX", true);
        blurSP->SetTexture2D("B_InputTexture", inputTexture);
        m_auxiliarFramebuffer->SetDrawBuffers({GL::Attachment::COLOR1});
        GEngine::GetInstance()->RenderViewportPlane();

        // Render blur Y
        blurSP->SetBool("B_BlurInX", false);
        blurSP->SetTexture2D("B_InputTexture", auxiliarTexture);
        m_auxiliarFramebuffer->SetDrawBuffers({GL::Attachment::COLOR0});
        GEngine::GetInstance()->RenderViewportPlane();
    }
    else
    {
        Texture2D *inTexture = inputTexture;
        Texture2D *outTexture =
            (blurRadius % 2 == 1) ? blurredOutputTexture : auxiliarTexture;
        m_auxiliarFramebuffer->SetDrawBuffers({GL::Attachment::COLOR0});
        for (int i = 0; i < blurRadius; ++i)
        {
            m_auxiliarFramebuffer->SetAttachmentTexture(outTexture,
                                                        GL::Attachment::COLOR0);
            blurSP->SetTexture2D("B_InputTexture", inTexture);
            blurSP->SetInt("B_Iteration", i);
            GEngine::GetInstance()->RenderViewportPlane();

            // Swap textures
            if (blurRadius % 2 == 1)
            {
                outTexture =
                    ((i % 2 == 0) ? auxiliarTexture : blurredOutputTexture);
            }
            else
            {
                outTexture =
                    ((i % 2 == 0) ? blurredOutputTexture : auxiliarTexture);
            }
            inTexture = (outTexture == auxiliarTexture ? blurredOutputTexture
                                                       : auxiliarTexture);
        }
    }

    m_auxiliarFramebuffer->UnBind();

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    GL::Pop(GL::Pushable::BLEND_STATES);
    GL::Pop(GL::Pushable::COLOR_MASK);
    GL::Pop(GL::Pushable::VIEWPORT);
}

void GEngine::BlurTextureCM(TextureCubeMap *inputTextureCM,
                            TextureCubeMap *auxiliarTextureCM,
                            TextureCubeMap *blurredOutputTextureCM,
                            uint blurRadius) const
{
    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::COLOR_MASK);
    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::BindTarget::SHADER_PROGRAM);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    uint cmSize = inputTextureCM->GetSize();
    m_auxiliarFramebufferCM->Bind();
    GL::SetViewport(0, 0, cmSize, cmSize);
    m_auxiliarFramebufferCM->Resize(cmSize, cmSize);

    p_separableGaussianBlurCubeMapSP.Get()->Bind();
    p_separableGaussianBlurCubeMapSP.Get()->SetVector2("B_InputTextureSize",
                                                       Vector2(cmSize));
    p_separableGaussianBlurCubeMapSP.Get()->SetInt("B_BlurRadius", blurRadius);

    const Array<float> blurKernel = GetGaussianBlurKernel(blurRadius);
    p_separableGaussianBlurCubeMapSP.Get()->SetFloatArray("B_BlurKernel",
                                                          blurKernel);

    for (GL::CubeMapDir cmDir : GL::GetAllCubeMapDirs())
    {
        m_auxiliarFramebufferCM->SetAttachmentTexture(
            blurredOutputTextureCM,
            GL::Attachment::COLOR0,
            SCAST<GL::TextureTarget>(cmDir));
        m_auxiliarFramebufferCM->SetAttachmentTexture(
            auxiliarTextureCM,
            GL::Attachment::COLOR1,
            SCAST<GL::TextureTarget>(cmDir));

        GL::Disable(GL::Enablable::BLEND);

        p_separableGaussianBlurCubeMapSP.Get()->SetVector3(
            "B_SampleDirection", GL::GetCubeMapDirVector(cmDir));

        // Render blur X
        p_separableGaussianBlurCubeMapSP.Get()->SetBool("B_BlurInX", true);
        p_separableGaussianBlurCubeMapSP.Get()->SetTextureCubeMap(
            "B_InputTexture", inputTextureCM);
        m_auxiliarFramebufferCM->SetDrawBuffers({GL::Attachment::COLOR1});
        GEngine::GetInstance()->RenderViewportPlane();

        // Render blur Y
        p_separableGaussianBlurCubeMapSP.Get()->SetBool("B_BlurInX", false);
        p_separableGaussianBlurCubeMapSP.Get()->SetTextureCubeMap(
            "B_InputTexture", auxiliarTextureCM);
        m_auxiliarFramebufferCM->SetDrawBuffers({GL::Attachment::COLOR0});
        GEngine::GetInstance()->RenderViewportPlane();
    }

    m_auxiliarFramebufferCM->UnBind();

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    GL::Pop(GL::Pushable::BLEND_STATES);
    GL::Pop(GL::Pushable::COLOR_MASK);
    GL::Pop(GL::Pushable::VIEWPORT);
}

void GEngine::FillTexture(Texture2D *texture, const Color &color)
{
    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    GL::Disable(GL::Enablable::BLEND);

    m_auxiliarFramebuffer->Bind();
    m_auxiliarFramebuffer->SetAttachmentTexture(texture,
                                                GL::Attachment::COLOR0);
    GL::ClearColorBuffer(color);

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::Pushable::BLEND_STATES);
}

void GEngine::CopyTexture(Texture2D *source, Texture2D *destiny)
{
    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    destiny->Resize(source->GetSize());

    GL::Disable(GL::Enablable::BLEND);

    m_auxiliarFramebuffer->Bind();
    m_auxiliarFramebuffer->SetAttachmentTexture(destiny,
                                                GL::Attachment::COLOR0);
    GEngine::RenderTexture(source);

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::Pushable::BLEND_STATES);
}

bool GEngine::CanRenderNow(Renderer *rend, RenderPass renderPass) const
{
    if (rend->GetGameObject()->IsVisibleRecursively() && rend->IsVisible())
    {
        Material *mat = GetReplacementMaterial() ? GetReplacementMaterial()
                                                 : rend->GetActiveMaterial();
        return (mat && (mat->GetShaderProgramProperties().GetRenderPass() ==
                        renderPass));
    }
    return false;
}

void GEngine::RenderViewportRect(ShaderProgram *sp, const AARect &destRectMask)
{
    GL::Push(GL::BindTarget::SHADER_PROGRAM);

    sp->Bind();
    BANG_UNUSED(destRectMask);
    sp->SetVector2(
        GLUniforms::UniformName_AlbedoUvOffset, Vector2::Zero(), false);
    sp->SetVector2(
        GLUniforms::UniformName_AlbedoUvMultiply, Vector2::One(), false);
    RenderViewportPlane();

    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
}

void GEngine::ApplyGammaCorrection(GBuffer *gbuffer, float gammaCorrection)
{
    GL::Push(GL::BindTarget::SHADER_PROGRAM);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    gbuffer->Bind();

    ShaderProgram *sp = p_renderTextureToViewportGammaSP.Get();
    sp->Bind();
    sp->SetFloat("B_GammaCorrection", gammaCorrection, false);
    sp->SetTexture2D("B_RenderTexture_Texture", gbuffer->GetDrawColorTexture());
    gbuffer->ApplyPass(p_renderTextureToViewportGammaSP.Get(), true);

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
}

void GEngine::RenderTexture_(Texture2D *texture, float gammaCorrection)
{
    GL::Push(GL::BindTarget::SHADER_PROGRAM);

    bool useGammaCorrection = (gammaCorrection != 1.0f);
    ShaderProgram *sp =
        (useGammaCorrection ? p_renderTextureToViewportGammaSP.Get()
                            : p_renderTextureToViewportSP.Get());
    sp->Bind();
    if (useGammaCorrection)
    {
        sp->SetFloat("B_GammaCorrection", gammaCorrection, false);
    }
    sp->SetTexture2D("B_RenderTexture_Texture", texture, false);
    GEngine::RenderViewportRect(sp, AARect::NDCRect());

    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
}
void GEngine::RenderTexture(Texture2D *texture)
{
    RenderTexture(texture, 1.0f);
}
void GEngine::RenderTexture(Texture2D *texture, float gammaCorrection)
{
    RenderTexture_(texture, gammaCorrection);
}

void GEngine::RenderTransparentPass(GameObject *go)
{
    Camera *cam = Camera::GetActive();
    ASSERT(cam);

    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::Pushable::DEPTH_STATES);

    GL::SetDepthMask(false);
    GL::Enable(GL::Enablable::BLEND);
    GL::BlendFunc(GL::BlendFactor::SRC_ALPHA,
                  GL::BlendFactor::ONE_MINUS_SRC_ALPHA);
    m_currentlyForwardRendering = true;

    const Vector3 camPos = cam->GetGameObject()->GetTransform()->GetPosition();

    // Sort back to front
    Array<GameObject *> goChildren = go->GetChildrenRecursively();
    goChildren.Sort(
        [camPos](const GameObject *lhs, const GameObject *rhs) -> bool {
            const Transform *lhsTrans = lhs->GetTransform();
            const Transform *rhsTrans = rhs->GetTransform();
            if (lhsTrans && rhsTrans)
            {
                const Vector3 lhsPos = lhsTrans->GetPosition();
                const Vector3 rhsPos = rhsTrans->GetPosition();
                const Vector3 lhsCamPosDiff = (lhsPos - camPos);
                const Vector3 rhsCamPosDiff = (rhsPos - camPos);
                const float lhsDistToCamSq =
                    Vector3::Dot(lhsCamPosDiff, lhsCamPosDiff);
                const float rhsDistToCamSq =
                    Vector3::Dot(rhsCamPosDiff, rhsCamPosDiff);
                return lhsDistToCamSq > rhsDistToCamSq;
            }
            return false;
        });

    // Render back to front
    for (GameObject *go : goChildren)
    {
        go->Render(RenderPass::SCENE_TRANSPARENT, false);
    }

    m_currentlyForwardRendering = false;
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::BLEND_STATES);
}

void GEngine::RenderViewportPlane()
{
    // Save state
    bool prevWireframe = GL::IsWireframe();
    GL::Push(GL::Pushable::DEPTH_STATES);
    GL::Push(GL::Pushable::CULL_FACE);

    // Set state
    GL::SetWireframe(false);
    GL::SetDepthMask(false);
    GL::Disable(GL::Enablable::CULL_FACE);
    GL::SetDepthFunc(GL::Function::ALWAYS);

    GL::Render(p_windowPlaneMesh.Get()->GetVAO(),
               GL::Primitive::TRIANGLES,
               p_windowPlaneMesh.Get()->GetNumVerticesIds());

    // Restore state
    GL::SetWireframe(prevWireframe);
    GL::Pop(GL::Pushable::CULL_FACE);
    GL::Pop(GL::Pushable::DEPTH_STATES);
}

GEngine *GEngine::GetInstance()
{
    Application *app = Application::GetInstance();
    return app ? app->GetGEngine() : nullptr;
}

void GEngine::RenderShadowMaps(GameObject *go)
{
    const Array<Light *> &lights = m_lightsCache.GetGatheredArray(go);
    for (Light *light : lights)
    {
        if (light->IsActiveRecursively())
        {
            light->RenderShadowMaps(go);
        }
    }
}

void GEngine::RenderReflectionProbes(GameObject *go)
{
    const Array<ReflectionProbe *> &reflProbes =
        m_reflProbesCache.GetGatheredArray(go);
    for (ReflectionProbe *reflProbe : reflProbes)
    {
        if (reflProbe->IsActiveRecursively())
        {
            reflProbe->RenderReflectionProbe();
        }
    }
}

void GEngine::PushActiveRenderingCamera()
{
    if (GetActiveRenderingCamera())
    {
        GetActiveRenderingCamera()
            ->EventEmitter<IEventsDestroy>::RegisterListener(this);
    }
    p_renderingCameras.stack.push(GetActiveRenderingCamera());
}

void GEngine::SetActiveRenderingCamera(Camera *camera)
{
    if (GetActiveRenderingCamera())
    {
        GetActiveRenderingCamera()->UnBind();
    }

    p_renderingCameras.currentValue = camera;
    if (GetActiveRenderingCamera())
    {
        GetActiveRenderingCamera()->Bind();
    }
}

void GEngine::PopActiveRenderingCamera()
{
    ASSERT(p_renderingCameras.stack.size() >= 1);

    // Pop until we find a non-destroyed camera or until the stack is empty.
    Camera *poppedCamera = nullptr;
    while (!p_renderingCameras.stack.empty())
    {
        Camera *pCam = p_renderingCameras.stack.top();
        p_renderingCameras.stack.pop();
        bool cameraIsDestroyed =
            m_stackedCamerasThatHaveBeenDestroyed.Contains(pCam);
        m_stackedCamerasThatHaveBeenDestroyed.Remove(pCam);

        if (!cameraIsDestroyed)
        {
            poppedCamera = pCam;
            break;
        }
    }

    SetActiveRenderingCamera(poppedCamera);
}

void GEngine::FillCubeMapFromTextures(TextureCubeMap *texCMToFill,
                                      Texture2D *topTexture,
                                      Texture2D *botTexture,
                                      Texture2D *leftTexture,
                                      Texture2D *rightTexture,
                                      Texture2D *frontTexture,
                                      Texture2D *backTexture,
                                      uint mipMapLevel)
{
    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::SHADER_PROGRAM);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    m_fillCubeMapFromTexturesFB->Bind();
    m_fillCubeMapFromTexturesSP.Get()->Bind();

    GL::SetViewport(0, 0, texCMToFill->GetSize(), texCMToFill->GetSize());
    m_fillCubeMapFromTexturesFB->SetAttachmentTexture(
        texCMToFill, GL::Attachment::COLOR0, mipMapLevel);
    m_fillCubeMapFromTexturesFB->SetAllDrawBuffers();

    m_fillCubeMapFromTexturesSP.Get()->SetTexture2D(
        "B_FillCMFromTexture_TopTexture", topTexture, false);
    m_fillCubeMapFromTexturesSP.Get()->SetTexture2D(
        "B_FillCMFromTexture_BotTexture", botTexture, false);
    m_fillCubeMapFromTexturesSP.Get()->SetTexture2D(
        "B_FillCMFromTexture_LeftTexture", leftTexture, false);
    m_fillCubeMapFromTexturesSP.Get()->SetTexture2D(
        "B_FillCMFromTexture_RightTexture", rightTexture, false);
    m_fillCubeMapFromTexturesSP.Get()->SetTexture2D(
        "B_FillCMFromTexture_FrontTexture", frontTexture, false);
    m_fillCubeMapFromTexturesSP.Get()->SetTexture2D(
        "B_FillCMFromTexture_BackTexture", backTexture, false);

    GEngine::GetInstance()->RenderViewportPlane();

    m_fillCubeMapFromTexturesSP.Get()->UnBind();
    m_fillCubeMapFromTexturesFB->UnBind();

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::Pushable::SHADER_PROGRAM);
    GL::Pop(GL::Pushable::VIEWPORT);
}

void GEngine::Render(Renderer *rend)
{
    if (!rend || !rend->GetActiveMaterial() ||
        !rend->GetActiveMaterial()->GetShaderProgram() ||
        !rend->GetActiveMaterial()->GetShaderProgram()->IsLinked())
    {
        return;
    }

    // If we have a replacement shader currently, change the renderer sp
    AH<Material> previousRendSharedMat, previousRendCopiedMat;
    previousRendSharedMat.Set(rend->GetSharedMaterial());
    previousRendCopiedMat.Set(rend->GetCopiedMaterial());
    if (GetReplacementMaterial())
    {
        rend->EventEmitter<IEventsRendererChanged>::SetEmitEvents(false);
        rend->SetMaterial(GetReplacementMaterial(), nullptr);
        rend->EventEmitter<IEventsRendererChanged>::SetEmitEvents(true);
    }

    // Render with the renderer!
    rend->Bind();

    if (m_currentlyForwardRendering)
    {
        PrepareForForwardRendering(rend);
    }

    rend->OnRender();

    rend->UnBind();

    // Restore previous sp, in case it was replaced with replacement shader
    if (GetReplacementMaterial())
    {
        rend->EventEmitter<IEventsRendererChanged>::SetEmitEvents(false);
        rend->SetMaterial(previousRendSharedMat.Get(),
                          previousRendCopiedMat.Get());
        rend->EventEmitter<IEventsRendererChanged>::SetEmitEvents(true);
    }
}

GL *GEngine::GetGL() const
{
    return m_gl;
}

TextureUnitManager *GEngine::GetTextureUnitManager() const
{
    return m_texUnitManager;
}

void GEngine::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    Camera *cam = DCAST<Camera *>(object);
    ASSERT(cam);

    m_stackedCamerasThatHaveBeenDestroyed.Add(cam);
}
