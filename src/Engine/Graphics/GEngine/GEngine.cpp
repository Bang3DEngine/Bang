#include "Bang/GEngine.h"

#include "Bang/GL.h"
#include "Bang/VAO.h"
#include "Bang/VBO.h"
#include "Bang/Mesh.h"
#include "Bang/Scene.h"
#include "Bang/Light.h"
#include "Bang/Input.h"
#include "Bang/Window.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/ChronoGL.h"
#include "Bang/Material.h"
#include "Bang/Renderer.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/MeshFactory.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/RectTransform.h"
#include "Bang/UILayoutManager.h"
#include "Bang/TextureUnitManager.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/SelectionFramebuffer.h"

USING_NAMESPACE_BANG

GEngine* GEngine::s_gEngine = nullptr;

GEngine::GEngine()
{
}

GEngine::~GEngine()
{
    delete m_texUnitManager;
    delete m_gl;
}

void GEngine::Init()
{
    m_gl = new GL();
    GL::SetActive( GetGL() );
    m_texUnitManager = new TextureUnitManager();

    p_windowPlaneMesh = Resources::Clone<Mesh>(MeshFactory::GetUIPlane());
    p_renderTextureToViewportSP = ShaderProgramFactory::GetRenderTextureToViewport();
    m_renderSkySP.Set( ShaderProgramFactory::Get(
                        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
                        EPATH("Shaders/RenderSky.frag")) );
    GL::SetActive( nullptr );
}

void GEngine::Render(GameObject *go, Camera *camera)
{
    if (!go) { return; }

    go->BeforeRender();
    SetActiveRenderingCamera(camera);
    RenderShadowMaps(go);
    SetActiveRenderingCamera(nullptr);

    SetActiveRenderingCamera(camera);
    RenderToGBuffer(go, camera);
    RenderToSelectionFramebuffer(go, camera);
    SetActiveRenderingCamera(nullptr);
}

void GEngine::ApplyStenciledDeferredLightsToGBuffer(GameObject *lightsContainer,
                                                    Camera *camera,
                                                    const AARect &maskRectNDC)
{
    Byte prevStencilValue              = GL::GetStencilValue();
    GL::Function prevStencilFunc       = GL::GetStencilFunc();
    GL::StencilOperation prevStencilOp = GL::GetStencilOp();

    // We have marked from before the zone where we want to apply the effect
    GL::SetStencilOp(GL::StencilOperation::Keep);
    GL::SetStencilFunc(GL::Function::Equal);
    GL::SetStencilValue(1);

    List<Light*> lights = lightsContainer->GetComponentsInChildren<Light>();
    for (Light *light : lights)
    {
        if (!light || !light->IsEnabled(true)) { continue; }
        light->ApplyLight(camera, maskRectNDC);
    }

    GL::SetStencilValue(prevStencilValue);
    GL::SetStencilFunc(prevStencilFunc);
    GL::SetStencilOp(prevStencilOp);
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
    GEngine *ge = GEngine::GetActive();
    return ge ? ge->p_currentRenderingCamera : nullptr;
}

GBuffer *GEngine::GetActiveGBuffer()
{
    Camera *cam = GEngine::GetActiveRenderingCamera();
    return cam ? cam->GetGBuffer() : nullptr;
}

SelectionFramebuffer *GEngine::GetActiveSelectionFramebuffer()
{
    Camera *cam = GEngine::GetActiveRenderingCamera();
    return cam ? cam->GetSelectionFramebuffer() : nullptr;
}

void GEngine::RenderToGBuffer(GameObject *go, Camera *camera)
{
    bool wasEnabledBlendi0 = GL::IsEnabledi(GL::Enablable::Blend, 0);
    GL::BlendFactor prevBlendSrcColorFactor = GL::GetBlendSrcFactorColor();
    GL::BlendFactor prevBlendDstColorFactor = GL::GetBlendDstFactorColor();
    GL::BlendFactor prevBlendSrcAlphaFactor = GL::GetBlendSrcFactorAlpha();
    GL::BlendFactor prevBlendDstAlphaFactor = GL::GetBlendDstFactorAlpha();

    camera->BindGBuffer();

    GBuffer *gbuffer = camera->GetGBuffer();
    gbuffer->PushDepthStencilTexture();

    GL::Enablei(GL::Enablable::Blend, 0);
    GL::BlendFunc(GL::BlendFactor::SrcAlpha, GL::BlendFactor::OneMinusSrcAlpha);

    // GBuffer Scene rendering
    gbuffer->SetAllDrawBuffers();
    gbuffer->SetSceneDepthStencil();
    GL::ClearStencilBuffer();
    GL::ClearDepthBuffer();
    GL::ClearColorBuffer(Color::Zero);
    GL::SetDepthMask(true);
    GL::SetDepthFunc(GL::Function::LEqual);

    // Render the sky / background (before so that alphas in scene can be handled)
    GLId prevBoundSP = GL::GetBoundId(m_renderSkySP.Get()->GetGLBindTarget());
    m_renderSkySP.Get()->Bind();
    gbuffer->ApplyPass(m_renderSkySP.Get(), false);
    GL::Bind(m_renderSkySP.Get()->GetGLBindTarget(), prevBoundSP); // Restore

    // Render scene pass
    RenderWithPassAndMarkStencilForLights(go, RenderPass::Scene);
    ApplyStenciledDeferredLightsToGBuffer(go, camera);

     // Render scene postprocess
    gbuffer->SetColorDrawBuffer();
    RenderWithPass(go, RenderPass::ScenePostProcess);

    // GBuffer Canvas rendering
    gbuffer->SetColorDrawBuffer();
    gbuffer->SetCanvasDepthStencil();
    GL::ClearDepthBuffer();
    GL::SetDepthMask(true);
    GL::SetDepthFunc(GL::Function::LEqual);
    RenderWithPass(go, RenderPass::Canvas);
    RenderWithPass(go, RenderPass::CanvasPostProcess);

    // GBuffer Overlay rendering
    gbuffer->SetAllDrawBuffers();
    gbuffer->SetOverlayDepthStencil();
    GL::ClearStencilBuffer();
    GL::ClearDepthBuffer();
    GL::SetDepthMask(true);
    GL::SetDepthFunc(GL::Function::LEqual);
    RenderWithPass(go, RenderPass::Overlay);
    // RenderWithPassAndMarkStencilForLights(go, RenderPass::Overlay);
    // ApplyStenciledDeferredLightsToGBuffer(go, camera);
    RenderWithPass(go, RenderPass::OverlayPostProcess);

    // GL::BlendFuncSeparate(prevBlendSrcColorFactor, prevBlendDstColorFactor,
    //                       prevBlendSrcAlphaFactor, prevBlendDstAlphaFactor);
    GL::SetEnabledi(GL::Enablable::Blend, 0, wasEnabledBlendi0);
    gbuffer->PopDepthStencilTexture();
}

void GEngine::RenderToSelectionFramebuffer(GameObject *go, Camera *camera)
{
    if (camera->GetRenderSelectionBuffer())
    {
        camera->BindSelectionFramebuffer();

        // Selection rendering
        camera->GetSelectionFramebuffer()->PrepareNewFrameForRender(go);
        go->Render(RenderPass::Scene);
        GL::ClearStencilBuffer();
        GL::ClearDepthBuffer();
        GL::SetDepthFunc(GL::Function::LEqual);
        RenderWithPass(go, RenderPass::Canvas);

        GL::ClearDepthBuffer();
        RenderWithPass(go, RenderPass::Overlay);
    }
}

void GEngine::RenderWithPass(GameObject *go, RenderPass renderPass)
{
    Camera *cam = GetActiveRenderingCamera();
    if (cam && cam->MustRenderPass(renderPass))
    {
        RenderWithPassRaw(go, renderPass);
    }
}

void GEngine::RenderWithPassRaw(GameObject *go, RenderPass renderPass)
{
    go->Render(renderPass, true);
}

void GEngine::RenderWithPassAndMarkStencilForLights(GameObject *go,
                                                    RenderPass renderPass)
{
    // Save state
    Byte prevStencilValue                     = GL::GetStencilValue();
    GL::StencilOperation prevStencilOperation = GL::GetStencilOp();

    GL::SetStencilValue(1);
    GL::SetStencilOp(GL::StencilOperation::Replace);

    // Render pass
    RenderWithPass(go, renderPass);

    // Restore state
    GL::SetStencilOp(prevStencilOperation);
    GL::SetStencilValue(prevStencilValue);
}

void GEngine::SetActive(GEngine *gEngine)
{
    GEngine::s_gEngine = gEngine;
    GL::SetActive( gEngine ? gEngine->GetGL() : nullptr );
}

void GEngine::RenderViewportRect(ShaderProgram *sp, const AARect &destRectMask)
{
    // Save state
    GLId prevBoundShaderProgram  = GL::GetBoundId(GL::BindTarget::ShaderProgram);

    // Set state, bind and draw
    sp->Bind();
    sp->SetVector2("B_AlbedoUvOffset",           Vector2::Zero, false);
    sp->SetVector2("B_AlbedoUvMultiply",          Vector2::One, false);
    sp->SetVector2("B_destRectMinCoord", destRectMask.GetMin(), false);
    sp->SetVector2("B_destRectMaxCoord", destRectMask.GetMax(), false);

    RenderViewportPlane(); // Renduurrr

    // Restore state
    GL::Bind(GL::BindTarget::ShaderProgram, prevBoundShaderProgram);
}

void GEngine::RenderTexture(Texture2D *texture)
{
    // Save state
    GLId prevBoundSP = GL::GetBoundId(GL::BindTarget::ShaderProgram);

    ShaderProgram *sp = p_renderTextureToViewportSP.Get();

    sp->Bind();
    sp->SetTexture2D(GBuffer::GetColorsTexName(), texture, false);
    GEngine::RenderViewportRect(sp, AARect::NDCRect);

    // Restore state
    GL::Bind(GL::BindTarget::ShaderProgram, prevBoundSP);
}

void GEngine::RenderWithAllPasses(GameObject *go)
{
    RenderWithPass(go, RenderPass::Scene);
    RenderWithPass(go, RenderPass::ScenePostProcess);
    RenderWithPass(go, RenderPass::Canvas);
    RenderWithPass(go, RenderPass::CanvasPostProcess);
    RenderWithPass(go, RenderPass::Overlay);
    RenderWithPass(go, RenderPass::OverlayPostProcess);
}

void GEngine::RenderViewportPlane()
{
    // Save state
    bool prevWireframe           = GL::IsWireframe();
    bool prevDepthMask           = GL::GetDepthMask();
    GL::Function prevDepthFunc   = GL::GetDepthFunc();

    // Set state
    GL::SetWireframe(false);
    GL::SetDepthMask(false);
    GL::SetDepthFunc(GL::Function::Always);

    GL::Render(p_windowPlaneMesh.Get()->GetVAO(), GL::Primitive::Triangles,
               p_windowPlaneMesh.Get()->GetVertexCount());

    // Restore state
    GL::SetWireframe(prevWireframe);
    GL::SetDepthMask(prevDepthMask);
    GL::SetDepthFunc(prevDepthFunc);
}

GEngine* GEngine::GetActive()
{
    Window *win = Window::GetActive();
    return win ? win->GetGEngine() : nullptr;
}

void GEngine::RenderShadowMaps(GameObject *go)
{
    if (!go->IsActive()) { return; }

    List<Light*> lights = go->GetComponentsInChildren<Light>(true);
    for (Light *light : lights)
    {
        if (light->IsActive()) { light->RenderShadowMaps(); }
    }
}

void GEngine::SetActiveRenderingCamera(Camera *camera)
{
    if (p_currentRenderingCamera) { p_currentRenderingCamera->UnBind(); }

    p_currentRenderingCamera = camera;
    if (p_currentRenderingCamera) { p_currentRenderingCamera->Bind(); }
}

void GEngine::Render(Renderer *rend)
{
    // If we have a replacement shader currently, change the renderer
    // shader program
    RH<Material> previousRendMat;
    previousRendMat.Set(rend->GetActiveMaterial());
    if (GetReplacementMaterial())
    {
        rend->EventEmitter<IRendererChangedListener>::SetEmitEvents(false);
        rend->SetMaterial( GetReplacementMaterial() );
        rend->EventEmitter<IRendererChangedListener>::SetEmitEvents(true);
    }

    // Render with the renderer!
    Camera *activeCamera = GetActiveRenderingCamera();
    if (activeCamera)
    {
        if (GL::IsBound(activeCamera->GetSelectionFramebuffer()))
        {
            activeCamera->GetSelectionFramebuffer()->RenderForSelectionBuffer(rend);
        }
        else
        {
            ASSERT( GL::IsBound(activeCamera->GetGBuffer()) ||
                    GL::GetBoundId(GL::BindTarget::DrawFramebuffer) > 0 );
            GEngine::RenderRaw(rend);
        }
    }
    else
    {
        GEngine::RenderRaw(rend);
    }

    // Restore previous shader program, in case it was replaced with
    // replacement shader
    if (GetReplacementMaterial())
    {
        rend->EventEmitter<IRendererChangedListener>::SetEmitEvents(false);
        rend->SetMaterial(previousRendMat.Get());
        rend->EventEmitter<IRendererChangedListener>::SetEmitEvents(true);
    }
}

void GEngine::RenderRaw(Renderer *rend)
{
    rend->Bind();
    rend->OnRender();
    rend->UnBind();
}

GL *GEngine::GetGL() const { return m_gl; }
TextureUnitManager *GEngine::GetTextureUnitManager() const
{
    return m_texUnitManager;
}
