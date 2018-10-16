#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/ComponentClassIds.h"
#include "Bang/ComponentMacros.h"
#include "Bang/Light.h"
#include "Bang/MetaNode.h"
#include "Bang/ResourceHandle.h"
#include "Bang/ShaderProgram.h"
#include "Bang/String.h"
#include "Bang/TextureCubeMap.h"


NAMESPACE_BANG_BEGIN

FORWARD class Camera;
FORWARD class Framebuffer;
FORWARD class GameObject;
FORWARD class ICloneable;
FORWARD class Material;
FORWARD class ShaderProgram;

class PointLight : public Light
{
    COMPONENT_WITH_FAST_DYNAMIC_CAST(PointLight)

public:
    void SetRange(float range);
    float GetRange() const;

    // Light
    TextureCubeMap* GetShadowMapTexture() const override;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

protected:
    float m_range = 1.0f;
    RH<Material> m_shadowMapMaterial;
    Framebuffer *m_shadowMapFramebuffer = nullptr;

    PointLight();
    virtual ~PointLight() override;

    float GetLightZFar() const;

    // Light
    void RenderShadowMaps_(GameObject *go) override;
    Array<Matrix4> GetWorldToShadowMapMatrices() const;
    virtual void SetUniformsBeforeApplyingLight(ShaderProgram* sp) const override;

    // Renderer
    AARect GetRenderRect(Camera *cam) const override;
};

NAMESPACE_BANG_END

#endif // POINTLIGHT_H
