#ifndef REFLECTIONPROBE_H
#define REFLECTIONPROBE_H

#include <array>

#include "Bang/Array.h"
#include "Bang/Vector3.h"
#include "Bang/Component.h"
#include "Bang/Framebuffer.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class Camera;
FORWARD class Renderer;

class ReflectionProbe : public Component
{
    COMPONENT(ReflectionProbe)

public:
	ReflectionProbe();
	virtual ~ReflectionProbe();

    void RenderReflectionProbe();

    void SetSize(const Vector3 &size);
    void SetIsBoxed(bool isBoxed);
    void SetFilterForIBL(bool filterForIBL);
    void SetRestTimeSeconds(float restTimeSeconds);

    bool GetIsBoxed() const;
    bool GetFilterForIBL() const;
    const Vector3 &GetSize() const;
    float GetRestTimeSeconds() const;
    TextureCubeMap *GetTextureCubeMapDiffuse() const;
    TextureCubeMap *GetTextureCubeMapSpecular() const;
    TextureCubeMap *GetTextureCubeMapWithoutFiltering() const;
    Camera* GetCamera(GL::CubeMapDir cubeMapDir) const;

    static void SetRendererUniforms(Renderer *renderer);

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    bool m_isBoxed = false;
    bool m_filterForIBL = true;
    Vector3 m_size = Vector3::One;
    float m_restTimeSeconds = 0.5f;

    Time::TimeT m_lastRenderTimeMillis = 0;
    Framebuffer *m_textureCubeMapFB = nullptr;
    std::array<GameObject*, 6> m_cameraGos;
    RH<TextureCubeMap> p_textureCubeMapWithoutFiltering;
    RH<TextureCubeMap> p_textureCubeMapDiffuse;
    RH<TextureCubeMap> p_textureCubeMapSpecular;

    static ReflectionProbe *GetClosestReflectionProbe(Renderer *renderer);
};

NAMESPACE_BANG_END

#endif // REFLECTIONPROBE_H

