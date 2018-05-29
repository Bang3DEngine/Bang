#ifndef GENGINE_H
#define GENGINE_H

#include "Bang/USet.h"
#include "Bang/AARect.h"
#include "Bang/RenderPass.h"
#include "Bang/StackAndValue.h"
#include "Bang/ResourceHandle.h"
#include "Bang/IDestroyListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class GL;
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

class GEngine : public IDestroyListener
{
public:
    GEngine();
    virtual ~GEngine();

    void Init();

    void Render(Renderer *rend);
    void RenderRaw(Renderer *rend);
    void Render(GameObject *go, Camera *camera);
    void RenderTexture(Texture2D *texture);
    void RenderWithAllPasses(GameObject *go);
    void RenderWithPass(GameObject *go, RenderPass renderPass,
                        bool renderChildren = true);
    void RenderWithPassRaw(GameObject *go, RenderPass renderPass,
                           bool renderChildren = true);
    void RenderViewportRect(ShaderProgram *sp,
                            const AARect &destRectMask = AARect::NDCRect);
    void ApplyGammaCorrection(GBuffer *gbuffer, float gammaCorrection);
    void RenderViewportPlane();

    void ApplyStenciledDeferredLightsToGBuffer(GameObject *lightsContainer,
                                               Camera *camera,
                                               const AARect &maskRectNDC = AARect::NDCRect);

    void SetReplacementMaterial(Material *material);
    Material* GetReplacementMaterial() const;

    static GBuffer *GetActiveGBuffer();
    static Camera *GetActiveRenderingCamera();
    static SelectionFramebuffer *GetActiveSelectionFramebuffer();

    GL *GetGL() const;
    TextureUnitManager *GetTextureUnitManager() const;

    // IDestroyListener
    virtual void OnDestroyed(EventEmitter<IDestroyListener> *object) override;

    static GEngine* GetInstance();

private:
    GL *m_gl = nullptr;
    RH<ShaderProgram> m_renderSkySP;
    RH<Material> m_replacementMaterial;
    StackAndValue<Camera*> p_renderingCameras;
    USet<Camera*> m_stackedCamerasThatHaveBeenDestroyed;
    TextureUnitManager *m_texUnitManager = nullptr;

    RH<Mesh> p_windowPlaneMesh;
    RH<ShaderProgram> p_renderTextureToViewportSP;

    void RenderShadowMaps(GameObject *go);
    void RenderToGBuffer(GameObject *go, Camera *camera);
    void RenderToSelectionFramebuffer(GameObject *go, Camera *camera);
    void RenderWithPassAndMarkStencilForLights(GameObject *go, RenderPass renderPass);

    void PushActiveRenderingCamera();
    void SetActiveRenderingCamera(Camera *camera);
    void PopActiveRenderingCamera();

    friend class Window;
};

NAMESPACE_BANG_END

#endif // GENGINE_H
