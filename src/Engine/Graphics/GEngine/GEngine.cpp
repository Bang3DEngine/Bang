#include "Bang/GEngine.h"

#include "Bang/GL.h"
#include "Bang/VAO.h"
#include "Bang/VBO.h"
#include "Bang/Mesh.h"
#include "Bang/Scene.h"
#include "Bang/Light.h"
#include "Bang/Input.h"
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
#include "Bang/Application.h"
#include "Bang/MeshFactory.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/RectTransform.h"
#include "Bang/UILayoutManager.h"
#include "Bang/TextureUnitManager.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/SelectionFramebuffer.h"

USING_NAMESPACE_BANG

GEngine::GEngine()
{
    p_renderingCameras.currentValue = nullptr;
}

GEngine::~GEngine()
{
    delete m_texUnitManager;
    delete m_gl;
}

void GEngine::Init()
{
    m_gl = new GL();
    m_gl->Init();

    m_texUnitManager = new TextureUnitManager();

    p_windowPlaneMesh = Resources::Clone<Mesh>(MeshFactory::GetUIPlane());
    p_renderTextureToViewportSP = ShaderProgramFactory::GetRenderTextureToViewport();
    m_renderSkySP.Set( ShaderProgramFactory::Get(
                        ShaderProgramFactory::GetScreenPassVertexShaderPath(),
                        EPATH("Shaders/RenderSky.frag")) );
}

void GEngine::Render(GameObject *go, Camera *camera)
{
    if (!go) { return; }

    go->BeforeRender();
    PushActiveRenderingCamera();
    SetActiveRenderingCamera(camera);
    RenderShadowMaps(go);
    PopActiveRenderingCamera();

    PushActiveRenderingCamera();
    SetActiveRenderingCamera(camera);
    RenderToGBuffer(go, camera);
    RenderToSelectionFramebuffer(go, camera);
    PopActiveRenderingCamera();
}

void GEngine::ApplyStenciledDeferredLightsToGBuffer(GameObject *lightsContainer,
                                                    Camera *camera,
                                                    const AARect &maskRectNDC)
{
    Byte prevStencilValue              = GL::GetStencilValue();
    GL::Function prevStencilFunc       = GL::GetStencilFunc();
    GL::StencilOperation prevStencilOp = GL::GetStencilOp();

    // We have marked from before the zone where we want to apply the effect
    GL::SetStencilOp(GL::StencilOperation::KEEP);
    GL::SetStencilFunc(GL::Function::EQUAL);
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
    GEngine *ge = GEngine::GetInstance();
    return ge ? ge->p_renderingCameras.currentValue : nullptr;
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
    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::Pushable::DEPTH_STATES);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    camera->BindGBuffer();

    GBuffer *gbuffer = camera->GetGBuffer();
    gbuffer->PushDepthStencilTexture();

    GL::Enablei(GL::Enablable::BLEND, 0);
    GL::BlendFunc(GL::BlendFactor::SRC_ALPHA, GL::BlendFactor::ONE_MINUS_SRC_ALPHA);

    auto RenderSky = [this, gbuffer]()
    {
        // Render the sky / background (before so that alphas in scene can be handled)
        GL::Push(GL::BindTarget::SHADER_PROGRAM);

        m_renderSkySP.Get()->Bind();
        gbuffer->ApplyPass(m_renderSkySP.Get(), false);

        GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    };

    bool hasRenderedSky = false;

    // GBuffer Scene rendering
    if (camera->GetRenderPassMask().Contains(RenderPass::SCENE))
    {
        gbuffer->SetAllDrawBuffers();
        gbuffer->SetSceneDepthStencil();
        GL::ClearColorBuffer(Color::Zero);
        GL::ClearStencilDepthBuffers();
        GL::SetDepthMask(true);
        GL::SetDepthFunc(GL::Function::LEQUAL);

        if (!hasRenderedSky)
        {
            RenderSky();
            hasRenderedSky = true;
        }

        // Render scene pass
        RenderWithPassAndMarkStencilForLights(go, RenderPass::SCENE);
        ApplyStenciledDeferredLightsToGBuffer(go, camera);

        // Render scene postprocess
        gbuffer->SetColorDrawBuffer();
        RenderWithPass(go, RenderPass::SCENE_POSTPROCESS);
    }

    // GBuffer Canvas rendering
    if (camera->GetRenderPassMask().Contains(RenderPass::CANVAS))
    {
        gbuffer->SetCanvasDepthStencil();
        GL::ClearDepthBuffer();
        GL::SetDepthMask(true);
        GL::SetDepthFunc(GL::Function::LEQUAL);

        if (!hasRenderedSky)
        {
            gbuffer->SetAllDrawBuffers();
            RenderSky();
            hasRenderedSky = true;
        }

        gbuffer->SetColorDrawBuffer();
        RenderWithPass(go, RenderPass::CANVAS);
        RenderWithPass(go, RenderPass::CANVAS_POSTPROCESS);
    }

    if (camera->GetRenderPassMask().Contains(RenderPass::OVERLAY))
    {
        // GBuffer Overlay rendering
        gbuffer->SetAllDrawBuffers();
        gbuffer->SetOverlayDepthStencil();
        GL::ClearStencilDepthBuffers();
        GL::SetDepthMask(true);
        GL::SetDepthFunc(GL::Function::LEQUAL);
        RenderWithPass(go, RenderPass::OVERLAY);
        RenderWithPass(go, RenderPass::OVERLAY_POSTPROCESS);
    }

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::BLEND_STATES);
    gbuffer->PopDepthStencilTexture();

    if (camera->GetGammaCorrection() != 1.0f)
    {
        ApplyGammaCorrection(camera->GetGBuffer(), camera->GetGammaCorrection());
    }
}

void GEngine::RenderToSelectionFramebuffer(GameObject *go, Camera *camera)
{
    if (camera->GetRenderSelectionBuffer())
    {
        GL::Push(GL::Pushable::DEPTH_STATES);

        camera->BindSelectionFramebuffer();

        // Selection rendering
        camera->GetSelectionFramebuffer()->PrepareNewFrameForRender(go);
        go->Render(RenderPass::SCENE);
        GL::ClearStencilDepthBuffers();
        GL::SetDepthFunc(GL::Function::LEQUAL);
        RenderWithPass(go, RenderPass::CANVAS);

        GL::ClearDepthBuffer();
        RenderWithPass(go, RenderPass::OVERLAY);

        GL::Pop(GL::Pushable::DEPTH_STATES);
    }
}

void GEngine::RenderWithPass(GameObject *go, RenderPass renderPass,
                             bool renderChildren)
{
    Camera *cam = GetActiveRenderingCamera();
    if (cam && cam->MustRenderPass(renderPass))
    {
        RenderWithPassRaw(go, renderPass);
    }
}

void GEngine::RenderWithPassRaw(GameObject *go, RenderPass renderPass,
                                bool renderChildren)
{
    go->Render(renderPass, renderChildren);
}

void GEngine::RenderWithPassAndMarkStencilForLights(GameObject *go,
                                                    RenderPass renderPass)
{
    // Save state
    Byte prevStencilValue                     = GL::GetStencilValue();
    GL::StencilOperation prevStencilOperation = GL::GetStencilOp();

    GL::SetStencilValue(1);
    GL::SetStencilOp(GL::StencilOperation::REPLACE);

    // Render pass
    RenderWithPass(go, renderPass);

    // Restore state
    GL::SetStencilOp(prevStencilOperation);
    GL::SetStencilValue(prevStencilValue);
}

void GEngine::RenderViewportRect(ShaderProgram *sp, const AARect &destRectMask)
{
    GL::Push(GL::BindTarget::SHADER_PROGRAM);

    // Set state, bind and draw
    sp->Bind();
    sp->SetVector2("B_AlbedoUvOffset",           Vector2::Zero, false);
    sp->SetVector2("B_AlbedoUvMultiply",          Vector2::One, false);
    sp->SetVector2("B_destRectMinCoord", destRectMask.GetMin(), false);
    sp->SetVector2("B_destRectMaxCoord", destRectMask.GetMax(), false);

    RenderViewportPlane(); // Renduurrr

    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
}

void GEngine::ApplyGammaCorrection(GBuffer *gbuffer, float gammaCorrection)
{
    GL::Push(GL::BindTarget::SHADER_PROGRAM);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    gbuffer->Bind();

    ShaderProgram *sp = p_renderTextureToViewportSP.Get();
    sp->Bind();
    sp->SetFloat("B_GammaCorrection", gammaCorrection, false);
    gbuffer->ApplyPass(p_renderTextureToViewportSP.Get(), true);

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
}

void GEngine::RenderTexture(Texture2D *texture)
{
    GL::Push(GL::BindTarget::SHADER_PROGRAM);

    ShaderProgram *sp = p_renderTextureToViewportSP.Get();

    sp->Bind();
    sp->SetFloat("B_GammaCorrection", 1.0f, false);
    sp->SetTexture2D(GBuffer::GetColorsTexName(), texture, false);
    GEngine::RenderViewportRect(sp, AARect::NDCRect);

    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
}

void GEngine::RenderWithAllPasses(GameObject *go)
{
    RenderWithPass(go, RenderPass::SCENE);
    RenderWithPass(go, RenderPass::SCENE_POSTPROCESS);
    RenderWithPass(go, RenderPass::CANVAS);
    RenderWithPass(go, RenderPass::CANVAS_POSTPROCESS);
    RenderWithPass(go, RenderPass::OVERLAY);
    RenderWithPass(go, RenderPass::OVERLAY_POSTPROCESS);
}

void GEngine::RenderViewportPlane()
{
    // Save state
    bool prevWireframe = GL::IsWireframe();
    GL::Push(GL::Pushable::DEPTH_STATES);

    // Set state
    GL::SetWireframe(false);
    GL::SetDepthMask(false);
    GL::SetDepthFunc(GL::Function::ALWAYS);

    GL::Render(p_windowPlaneMesh.Get()->GetVAO(), GL::Primitive::TRIANGLES,
               p_windowPlaneMesh.Get()->GetVertexCount());

    // Restore state
    GL::SetWireframe(prevWireframe);
    GL::Pop(GL::Pushable::DEPTH_STATES);
}

GEngine* GEngine::GetInstance()
{
    Application *app = Application::GetInstance();
    return app ? app->GetGEngine() : nullptr;
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

void GEngine::PushActiveRenderingCamera()
{
    if ( GetActiveRenderingCamera() )
    {
        GetActiveRenderingCamera()->EventEmitter<IDestroyListener>::
                                    RegisterListener(this);
    }
    p_renderingCameras.stack.push( GetActiveRenderingCamera() );
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
        if (p_renderingCameras.stack.empty()) { break; }

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
                    GL::GetBoundId(GL::BindTarget::DRAW_FRAMEBUFFER) > 0 );
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

void GEngine::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
    Camera *cam = DCAST<Camera*>(object);
    ASSERT(cam);

    m_stackedCamerasThatHaveBeenDestroyed.Add(cam);
}
