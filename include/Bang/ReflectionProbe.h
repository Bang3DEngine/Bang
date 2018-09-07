#ifndef REFLECTIONPROBE_H
#define REFLECTIONPROBE_H

#include <array>

#include "Bang/Array.h"
#include "Bang/Camera.h"
#include "Bang/Vector3.h"
#include "Bang/Component.h"
#include "Bang/Framebuffer.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class Renderer;

class ReflectionProbe : public Component
{
    COMPONENT(ReflectionProbe)

public:
	ReflectionProbe();
	virtual ~ReflectionProbe();

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
    void SetRestTimeSeconds(float restTimeSeconds);

    bool GetIsBoxed() const;
    int GetRenderSize() const;
    bool GetFilterForIBL() const;
    const Vector3 &GetSize() const;
    float GetRestTimeSeconds() const;
    TextureCubeMap *GetTextureCubeMapDiffuse() const;
    TextureCubeMap *GetTextureCubeMapSpecular() const;
    TextureCubeMap *GetTextureCubeMapWithoutFiltering() const;
    Camera* GetCamera(GL::CubeMapDir cubeMapDir) const;
    const std::array<Camera*, 6> &GetCameras() const;

    const Color& GetCamerasClearColor() const;
    CameraClearMode GetCamerasClearMode() const;
    TextureCubeMap* GetCamerasSkyBoxTexture() const;
    float GetCamerasZNear() const;
    float GetCamerasZFar() const;

    static void SetRendererUniforms(Renderer *renderer);

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_isBoxed = false;
    bool m_filterForIBL = true;
    Vector3 m_size = Vector3::One;
    float m_restTimeSeconds = 0.5f;

    int m_renderSize = -1;
    float m_camerasZNear = -1.0f;
    float m_camerasZFar  = -1.0f;
    Color m_camerasClearColor = -Color::One;
    CameraClearMode m_camerasClearMode = Undef<CameraClearMode>();
    RH<TextureCubeMap> m_camerasSkyBoxTexture;

    std::array<Camera*, 6> m_cameras;
    Time::TimeT m_lastRenderTimeMillis = 0;
    Framebuffer *m_textureCubeMapFB = nullptr;
    RH<TextureCubeMap> p_textureCubeMapWithoutFiltering;
    RH<TextureCubeMap> p_textureCubeMapDiffuse;
    RH<TextureCubeMap> p_textureCubeMapSpecular;

    static ReflectionProbe *GetClosestReflectionProbe(Renderer *renderer);
};

NAMESPACE_BANG_END

#endif // REFLECTIONPROBE_H

