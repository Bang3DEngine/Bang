#ifndef GENGINE_H
#define GENGINE_H

#include "Bang/USet.h"
#include "Bang/Array.h"
#include "Bang/Flags.h"
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
FORWARD class TextureCubeMap;
FORWARD class ReflectionProbe;
FORWARD class TextureUnitManager;

class GEngine : public EventListener<IEventsDestroy>
{
public:
    using RenderRoutine = std::function<void(Renderer*)>;

    GEngine();
    virtual ~GEngine();

    void Init();

    void Render(Scene *scene);
    void Render(Scene *scene, Camera *camera);
    void RenderTexture(Texture2D *texture);
    void RenderWithPass(GameObject *go,
                        RenderPass renderPass,
                        bool renderChildren = true);
    void RenderViewportRect(ShaderProgram *sp,
                            const AARect &destRectMask = AARect::NDCRect);
    void RenderToGBuffer(GameObject *go, Camera *camera);
    void RenderViewportPlane();

    void ApplyGammaCorrection(GBuffer *gbuffer,
                              float gammaCorrection);

    List<ReflectionProbe*> GetCurrentReflectionProbes() const;

    void SetReplacementMaterial(Material *material);
    void SetRenderRoutine(RenderRoutine renderRoutine);

    void PushActiveRenderingCamera();
    void SetActiveRenderingCamera(Camera *camera);
    void PopActiveRenderingCamera();

    void FillCubeMapFromTextures(TextureCubeMap *texCMToFill,
                                 Texture2D *topTexture,
                                 Texture2D *botTexture,
                                 Texture2D *leftTexture,
                                 Texture2D *rightTexture,
                                 Texture2D *frontTexture,
                                 Texture2D *backTexture,
                                 int mipMapLevel = 0);

    static GBuffer *GetActiveGBuffer();
    Material* GetReplacementMaterial() const;
    static Camera *GetActiveRenderingCamera();

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
    RenderRoutine m_renderRoutine;

    RH<ShaderProgram> m_fillCubeMapFromTexturesSP;
    Framebuffer *m_fillCubeMapFromTexturesFB = nullptr;

    List<ReflectionProbe*> m_currentReflectionProbes;

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
    void RenderReflectionProbes(GameObject *go);
    void RenderWithAllPasses(GameObject *go);
    void RenderTransparentPass(GameObject *go);
    void RenderWithPassAndMarkStencilForLights(GameObject *go, RenderPass renderPass);
    bool CanRenderNow(Renderer *rend, RenderPass renderPass) const;

    void ApplyStenciledDeferredLightsToGBuffer(
                                GameObject *lightsContainer,
                                Camera *camera,
                                const AARect &maskRectNDC = AARect::NDCRect);

    void RetrieveForwardRenderingInformation(GameObject *goToRender);
    void PrepareForForwardRendering(Renderer *rend);

    friend class Gizmos;
    friend class Window;
    friend class Renderer;
};

NAMESPACE_BANG_END

#endif // GENGINE_H
