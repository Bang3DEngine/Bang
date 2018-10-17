#ifndef REFLECTIONPROBE_H
#define REFLECTIONPROBE_H

#include <array>

#include "Bang/BangDefines.h"
#include "Bang/Camera.h"
#include "Bang/Color.h"
#include "Bang/Component.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/GL.h"
#include "Bang/MetaNode.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"
#include "Bang/Time.h"

namespace Bang
{
class Framebuffer;
class ICloneable;
class Renderer;
class ShaderProgram;
class TextureCubeMap;

class ReflectionProbe : public Component
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(ReflectionProbe)

public:
    ReflectionProbe();
    virtual ~ReflectionProbe() override;

    void RenderReflectionProbe(bool force = false);

    void SetCamerasClearColor(const Color &clearColor);
    void SetCamerasSkyBoxTexture(TextureCubeMap *skybox);
    void SetCamerasClearMode(CameraClearMode clearMode);
    void SetCamerasZNear(float zNear);
    void SetCamerasZFar(float zFar);
    void SetRenderSize(int size);
    void SetSize(const Vector3 &size);
    void SetIsBoxed(bool isBoxed);
    void SetFilterForIBL(bool filterForIBL);
    void SetRestTimeSeconds(double restTimeSeconds);

    bool GetIsBoxed() const;
    int GetRenderSize() const;
    bool GetFilterForIBL() const;
    const Vector3 &GetSize() const;
    const Time &GetRestTime() const;
    TextureCubeMap *GetTextureCubeMapDiffuse() const;
    TextureCubeMap *GetTextureCubeMapSpecular() const;
    TextureCubeMap *GetTextureCubeMapWithoutFiltering() const;
    Camera *GetCamera(GL::CubeMapDir cubeMapDir) const;
    const std::array<Camera *, 6> &GetCameras() const;

    const Color &GetCamerasClearColor() const;
    CameraClearMode GetCamerasClearMode() const;
    TextureCubeMap *GetCamerasSkyBoxTexture() const;
    float GetCamerasZNear() const;
    float GetCamerasZFar() const;

    static void SetRendererUniforms(Renderer *renderer, ShaderProgram *sp);

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_isBoxed = false;
    bool m_filterForIBL = true;
    Vector3 m_size = Vector3::One;
    Time m_restTime;

    int m_renderSize = -1;
    float m_camerasZNear = -1.0f;
    float m_camerasZFar = -1.0f;
    Color m_camerasClearColor = -Color::One;
    CameraClearMode m_camerasClearMode = Undef<CameraClearMode>();
    RH<TextureCubeMap> m_camerasSkyBoxTexture;

    Time m_lastRenderTime;
    std::array<Camera *, 6> m_cameras;
    Framebuffer *m_textureCubeMapFB = nullptr;
    RH<TextureCubeMap> p_textureCubeMapWithoutFiltering;
    RH<TextureCubeMap> p_textureCubeMapDiffuse;
    RH<TextureCubeMap> p_textureCubeMapSpecular;

    static ReflectionProbe *GetClosestReflectionProbe(Renderer *renderer);
};
}

#endif  // REFLECTIONPROBE_H
