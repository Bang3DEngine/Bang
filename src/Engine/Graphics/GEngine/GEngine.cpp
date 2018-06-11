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
#include "Bang/PointLight.h"
#include "Bang/Application.h"
#include "Bang/MeshFactory.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/RectTransform.h"
#include "Bang/UILayoutManager.h"
#include "Bang/TextureUnitManager.h"
#include "Bang/ShaderProgramFactory.h"

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

void GEngine::Render(Scene *scene)
{
    if (scene)
    {
        scene->BeforeRender();
        RenderShadowMaps(scene);
        RenderToGBuffer(scene, GetActiveRenderingCamera());
    }
}

void GEngine::Render(Scene *scene, Camera *camera)
{
    if (scene)
    {
        PushActiveRenderingCamera();
        SetActiveRenderingCamera(camera);

        Render(scene);

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

    List<Light*> lights = lightsContainer->GetComponentsInChildren<Light>();
    for (Light *light : lights)
    {
        if (!light || !light->IsEnabled(true)) { continue; }
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
    List<Light*> lights = go->GetComponentsInChildren<Light>(true);
    for (Light *light : lights)
    {
        if (!light->IsActive()) { continue; }

        uint lightType = 0;
        Transform *lightTR = light->GetGameObject()->GetTransform();
        float range = 0.0f;
        if (PointLight *pl = DCAST<PointLight*>(light))
        {
            range = pl->GetRange();
            lightType = 1;
        }

        m_currentForwardRenderingLightTypes.PushBack(lightType);
        m_currentForwardRenderingLightColors.PushBack(light->GetColor());
        m_currentForwardRenderingLightPositions.PushBack(lightTR->GetPosition());
        m_currentForwardRenderingLightForwardDirs.PushBack(lightTR->GetForward());
        m_currentForwardRenderingLightIntensities.PushBack(light->GetIntensity());
        m_currentForwardRenderingLightRanges.PushBack(range);

        if (++i == 128) { break; }
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
                  m_currentForwardRenderingLightColors, false);
            sp->SetVector3Array("B_ForwardRenderingLightPositions",
                  m_currentForwardRenderingLightPositions, false);
            sp->SetVector3Array("B_ForwardRenderingLightForwardDirs",
                  m_currentForwardRenderingLightForwardDirs, false);
            sp->SetFloatArray("B_ForwardRenderingLightIntensities",
                  m_currentForwardRenderingLightIntensities, false);
            sp->SetFloatArray("B_ForwardRenderingLightRanges",
                  m_currentForwardRenderingLightRanges, false);
            sp->SetIntArray("B_ForwardRenderingLightTypes",
                  m_currentForwardRenderingLightTypes, false);
        }
        sp->SetInt("B_ForwardRenderingLightNumber", numLights, false);
    }
}

void GEngine::SetReplacementMaterial(Material *material)
{
    m_replacementMaterial.Set(material);
}

void GEngine::SetRenderRoutine(GEngine::RenderRoutine renderRoutine)
{
    m_renderRoutine = renderRoutine;
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

void GEngine::RenderToGBuffer(GameObject *go, Camera *camera)
{
    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::BLEND_STATES);
    GL::Push(GL::Pushable::DEPTH_STATES);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    GL::SetViewport(0, 0, camera->GetRenderSize().x, camera->GetRenderSize().y);

    GBuffer *gbuffer = camera->GetGBuffer();

    auto RenderSky = [this, gbuffer]() // Lambda to render the sky / background
    {
        GL::Push(GL::BindTarget::SHADER_PROGRAM);
        m_renderSkySP.Get()->Bind();
        gbuffer->ApplyPass(m_renderSkySP.Get(), false);
        GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    };

    bool needsForwardRendering = camera->MustRenderPass(RenderPass::SCENE_TRANSPARENT);
    if (needsForwardRendering)
    {
        RetrieveForwardRenderingInformation(go);
    }

    gbuffer->Bind();
    gbuffer->PushDepthStencilTexture();

    bool hasRenderedSky = false;

    // GBuffer Scene rendering
    if (camera->MustRenderPass(RenderPass::SCENE) ||
        camera->MustRenderPass(RenderPass::SCENE_TRANSPARENT))
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
        if (camera->MustRenderPass(RenderPass::SCENE))
        {
            RenderWithPassAndMarkStencilForLights(go, RenderPass::SCENE);
            ApplyStenciledDeferredLightsToGBuffer(go, camera);
        }

        // Render scene transparent
        if (camera->MustRenderPass(RenderPass::SCENE_TRANSPARENT))
        {
            gbuffer->SetColorDrawBuffer();
            RenderTransparentPass(go);
        }

        // Render scene postprocess
        if (camera->MustRenderPass(RenderPass::SCENE))
        {
            gbuffer->SetColorDrawBuffer();
            RenderWithPass(go, RenderPass::SCENE_POSTPROCESS);
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
        if (camera->MustRenderPass(RenderPass::CANVAS_POSTPROCESS))
        {
            RenderWithPass(go, RenderPass::CANVAS_POSTPROCESS);
        }
    }

    if (camera->MustRenderPass(RenderPass::OVERLAY))
    {
        // GBuffer Overlay rendering
        gbuffer->SetAllDrawBuffers();
        gbuffer->SetOverlayDepthStencil();
        GL::ClearStencilDepthBuffers();
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
        ApplyGammaCorrection(camera->GetGBuffer(), camera->GetGammaCorrection());
    }

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::BLEND_STATES);
    GL::Pop(GL::Pushable::VIEWPORT);
    gbuffer->PopDepthStencilTexture();
}

void GEngine::RenderWithPass(GameObject *go, RenderPass renderPass,
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

bool GEngine::CanRenderNow(Renderer *rend, RenderPass renderPass) const
{
    if (!rend->IsVisible())
    {
        return false;
    }

    Material *mat = GetReplacementMaterial() ? GetReplacementMaterial() :
                                               rend->GetActiveMaterial();
    return (mat && (mat->GetRenderPass() == renderPass));
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
    RenderTransparentPass(go);
    RenderWithPass(go, RenderPass::SCENE_POSTPROCESS);
    RenderWithPass(go, RenderPass::CANVAS);
    RenderWithPass(go, RenderPass::CANVAS_POSTPROCESS);
    RenderWithPass(go, RenderPass::OVERLAY);
    RenderWithPass(go, RenderPass::OVERLAY_POSTPROCESS);
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
    List<GameObject*> goChildren = go->GetChildrenRecursively();
    goChildren.Sort(
    [camPos](const GameObject *lhs, const GameObject *rhs) -> bool
    {
        const Transform *lhsTrans = lhs->GetTransform();
        const Transform *rhsTrans = rhs->GetTransform();
        if (!lhsTrans || !rhsTrans)
        {
            return false;
        }

        const Vector3 lhsPos = lhsTrans->GetPosition();
        const Vector3 rhsPos = rhsTrans->GetPosition();
        const Vector3 lhsCamPosDiff = (lhsPos - camPos);
        const Vector3 rhsCamPosDiff = (rhsPos - camPos);
        const float lhsDistToCamSq = Vector3::Dot(lhsCamPosDiff, lhsCamPosDiff);
        const float rhsDistToCamSq = Vector3::Dot(rhsCamPosDiff, rhsCamPosDiff);
        return lhsDistToCamSq > rhsDistToCamSq;
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
        GetActiveRenderingCamera()->EventEmitter<IEventsDestroy>::
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
    if (!m_renderRoutine)
    {
        // If we have a replacement shader currently, change the renderer sp
        RH<Material> previousRendMat;
        previousRendMat.Set(rend->GetActiveMaterial());
        if (GetReplacementMaterial())
        {
            rend->EventEmitter<IEventsRendererChanged>::SetEmitEvents(false);
            rend->SetMaterial( GetReplacementMaterial() );
            rend->EventEmitter<IEventsRendererChanged>::SetEmitEvents(true);
        }

        // Render with the renderer!
        Camera *activeCamera = GetActiveRenderingCamera();
        ASSERT(activeCamera);

        ASSERT( GL::IsBound(activeCamera->GetGBuffer()) ||
                GL::GetBoundId(GL::BindTarget::DRAW_FRAMEBUFFER) > 0 );

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
            rend->SetMaterial(previousRendMat.Get());
            rend->EventEmitter<IEventsRendererChanged>::SetEmitEvents(true);
        }
    }
    else
    {
        m_renderRoutine(rend);
    }
}

GL *GEngine::GetGL() const { return m_gl; }
TextureUnitManager *GEngine::GetTextureUnitManager() const
{
    return m_texUnitManager;
}

void GEngine::OnDestroyed(EventEmitter<IEventsDestroy> *object)
{
    Camera *cam = DCAST<Camera*>(object);
    ASSERT(cam);

    m_stackedCamerasThatHaveBeenDestroyed.Add(cam);
}
