#ifndef GENGINE_H
#define GENGINE_H

#include <functional>
#include <vector>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEvents.h"
#include "Bang/Light.h"
#include "Bang/Map.tcc"
#include "Bang/MultiObjectGatherer.h"
#include "Bang/ObjectGatherer.tcc"
#include "Bang/ReflectionProbe.h"
#include "Bang/RenderPass.h"
#include "Bang/StackAndValue.h"
#include "Bang/USet.h"

namespace Bang
{
template <class>
class EventEmitter;
class Camera;
class DebugRenderer;
class Framebuffer;
class GBuffer;
class GL;
class GameObject;
class IEventsDestroy;
class Material;
class Mesh;
class RenderFactory;
class Renderer;
class Scene;
class ShaderProgram;
class Texture2D;
class TextureCubeMap;
class TextureUnitManager;

enum class BlurType
{
    GAUSSIAN,
    KAWASE
};

class GEngine : public EventListener<IEventsDestroy>
{
public:
    GEngine();
    virtual ~GEngine() override;

    void Init();

    void Render(GameObject *go);
    void Render(GameObject *go, Camera *camera);
    void RenderTexture(Texture2D *texture);
    void RenderTexture(Texture2D *texture, float gammaCorrection);
    void RenderWithPass(GameObject *go,
                        RenderPass renderPass,
                        bool renderChildren = true);
    void RenderViewportRect(ShaderProgram *sp,
                            const AARect &destRectMask = AARect::NDCRect());
    void RenderToGBuffer(GameObject *go, Camera *camera);
    void RenderViewportPlane();
    void PrepareForForwardRendering(Renderer *rend);

    void ApplyGammaCorrection(GBuffer *gbuffer, float gammaCorrection);
    void BlurTexture(Texture2D *inputTexture,
                     Texture2D *auxiliarTexture,
                     Texture2D *blurredOutputTexture,
                     uint blurRadius,
                     BlurType blurType = BlurType::GAUSSIAN) const;
    void BlurTextureCM(TextureCubeMap *inputTextureCM,
                       TextureCubeMap *auxiliarTextureCM,
                       TextureCubeMap *blurredOutputTextureCM,
                       uint blurRadius) const;
    void FillTexture(Texture2D *texture, const Color &color);
    void CopyTexture(Texture2D *source, Texture2D *destiny);

    void SetReplacementMaterial(Material *material);

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
                                 uint mipMapLevel = 0);

    static GBuffer *GetActiveGBuffer();

    DebugRenderer *GetDebugRenderer() const;
    RenderFactory *GetRenderFactory() const;
    Material *GetReplacementMaterial() const;
    static Camera *GetActiveRenderingCamera();
    const Array<ReflectionProbe *> &GetReflectionProbesFor(Scene *scene) const;

    GL *GetGL() const;
    TextureUnitManager *GetTextureUnitManager() const;

    // IEventsDestroy
    virtual void OnDestroyed(EventEmitter<IEventsDestroy> *object) override;

    static GEngine *GetInstance();

private:
    GL *m_gl = nullptr;
    DebugRenderer *m_debugRenderer = nullptr;
    RenderFactory *m_renderFactory = nullptr;
    TextureUnitManager *m_texUnitManager = nullptr;

    MultiObjectGatherer<ReflectionProbe, true> m_reflProbesCache;
    MultiObjectGatherer<Light, true> m_lightsCache;

    StackAndValue<Camera *> p_renderingCameras;
    USet<Camera *> m_stackedCamerasThatHaveBeenDestroyed;

    AH<ShaderProgram> m_renderSkySP;
    AH<Material> m_replacementMaterial;
    AH<ShaderProgram> m_fillCubeMapFromTexturesSP;
    Framebuffer *m_fillCubeMapFromTexturesFB = nullptr;

    // Forward rendering arrays
    bool m_currentlyForwardRendering = false;
    Array<int> m_currentForwardRenderingLightTypes;
    Array<Color> m_currentForwardRenderingLightColors;
    Array<Vector3> m_currentForwardRenderingLightPositions;
    Array<Vector3> m_currentForwardRenderingLightForwardDirs;
    Array<float> m_currentForwardRenderingLightIntensities;
    Array<float> m_currentForwardRenderingLightRanges;

    Framebuffer *m_auxiliarFramebuffer = nullptr;
    Framebuffer *m_auxiliarFramebufferCM = nullptr;
    AH<ShaderProgram> p_kawaseBlurSP;
    AH<ShaderProgram> p_separableGaussianBlurSP;
    AH<ShaderProgram> p_separableGaussianBlurCubeMapSP;

    AH<Mesh> p_windowPlaneMesh;
    AH<ShaderProgram> p_renderTextureToViewportSP;
    AH<ShaderProgram> p_renderTextureToViewportGammaSP;

    void Render(Renderer *rend);
    void RenderShadowMaps(GameObject *go);
    void RenderTexture_(Texture2D *texture, float gammaCorrection);
    void RenderReflectionProbes(GameObject *go);
    void RenderTransparentPass(GameObject *go);
    void RenderWithPassAndMarkStencilForLights(GameObject *go,
                                               RenderPass renderPass);
    bool CanRenderNow(Renderer *rend, RenderPass renderPass) const;

    void ApplyStenciledDeferredLightsToGBuffer(
        GameObject *lightsContainer,
        Camera *camera,
        const AARect &maskRectNDC = AARect::NDCRect());

    void RetrieveForwardRenderingInformation(GameObject *goToRender);

    friend class Gizmos;
    friend class Window;
    friend class Renderer;
};
}

#endif  // GENGINE_H
