#ifndef GENGINE_H
#define GENGINE_H

#include "Bang/USet.h"
#include "Bang/Array.h"
#include "Bang/AARect.h"
#include "Bang/RenderPass.h"
#include "Bang/StackAndValue.h"
#include "Bang/ResourceHandle.h"
#include "Bang/IEventsDestroy.h"

NAMESPACE_BANG_BEGIN

FORWARD class GL;
FORWARD class Light;
FORWARD class Scene;
FORWARD class Camera;
FORWARD class Texture;
FORWARD class GBuffer;
FORWARD class Renderer;
FORWARD class Texture2D;
FORWARD class Framebuffer;
FORWARD class ShaderProgram;
FORWARD class TextureUnitManager;
FORWARD class SelectionFramebuffer;

class GEngine : public EventListener<IEventsDestroy>
{
public:
    GEngine();
    virtual ~GEngine();

    void Init();

    void Render(GameObject *go, Camera *camera);
    void RenderTexture(Texture2D *texture);
    void RenderWithPass(GameObject *go, RenderPass renderPass,
                        bool renderChildren = true);
    void RenderViewportRect(ShaderProgram *sp,
                            const AARect &destRectMask = AARect::NDCRect);
    void RenderViewportPlane();

    void ApplyGammaCorrection(GBuffer *gbuffer, float gammaCorrection);

    void SetReplacementMaterial(Material *material);

    static GBuffer *GetActiveGBuffer();
    Material* GetReplacementMaterial() const;
    static Camera *GetActiveRenderingCamera();
    static SelectionFramebuffer *GetActiveSelectionFramebuffer();

    GL *GetGL() const;
    TextureUnitManager *GetTextureUnitManager() const;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    static GEngine* GetInstance();

private:
    GL *m_gl = nullptr;
    RH<ShaderProgram> m_renderSkySP;
    RH<Material> m_replacementMaterial;
    StackAndValue<Camera*> p_renderingCameras;
    USet<Camera*> m_stackedCamerasThatHaveBeenDestroyed;
    TextureUnitManager *m_texUnitManager = nullptr;

    // Forward rendering arrays
    bool m_currentlyForwardRendering = false;
    Array<int> m_currentForwardRenderingLightTypes;
    Array<Color> m_currentForwardRenderingLightColors;
    Array<Vector3> m_currentForwardRenderingLightPositions;
    Array<Vector3> m_currentForwardRenderingLightForwardDirs;
    Array<float> m_currentForwardRenderingLightIntensities;
    Array<float> m_currentForwardRenderingLightRanges;

    RH<Mesh> p_windowPlaneMesh;
    RH<ShaderProgram> p_renderTextureToViewportSP;

    void Render(Renderer *rend);
    void RenderShadowMaps(GameObject *go);
    void RenderWithAllPasses(GameObject *go);
    void RenderTransparentPass(GameObject *go);
    void RenderToGBuffer(GameObject *go, Camera *camera);
    void RenderToSelectionFramebuffer(GameObject *go, Camera *camera);
    void RenderWithPassAndMarkStencilForLights(GameObject *go, RenderPass renderPass);
    bool CanRenderNow(Renderer *rend, RenderPass renderPass) const;

    void ApplyStenciledDeferredLightsToGBuffer(
                                GameObject *lightsContainer,
                                Camera *camera,
                                const AARect &maskRectNDC = AARect::NDCRect);

    void RetrieveForwardRenderingInformation(GameObject *goToRender);
    void PrepareForForwardRendering(Renderer *rend);

    void PushActiveRenderingCamera();
    void SetActiveRenderingCamera(Camera *camera);
    void PopActiveRenderingCamera();

    friend class Gizmos;
    friend class Window;
    friend class Renderer;
};

NAMESPACE_BANG_END

#endif // GENGINE_H
