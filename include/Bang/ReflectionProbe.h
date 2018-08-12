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

class ReflectionProbe : public Component
{
    COMPONENT(ReflectionProbe)

public:
	ReflectionProbe();
	virtual ~ReflectionProbe();

    void RenderReflectionProbe();
    TextureCubeMap *GetTextureCubeMap() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

private:
    Framebuffer *m_textureCubeMapFB = nullptr;
    std::array<GameObject*, 6> m_cameraGos;
    RH<TextureCubeMap> p_textureCubeMap;

    Camera *GetCamera(GL::CubeMapDir dir) const;
};

NAMESPACE_BANG_END

#endif // REFLECTIONPROBE_H

