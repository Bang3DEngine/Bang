#include "Bang/Light.h"

#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Camera.h"
#include "Bang/FastDynamicCast.h"
#include "Bang/GBuffer.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/ICloneable.h"
#include "Bang/Material.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/RenderPass.h"
#include "Bang/Renderer.h"
#include "Bang/ShaderProgram.h"
#include "Bang/StreamOperators.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureCubeMap.h"
#include "Bang/Transform.h"

namespace Bang
{
class Texture2D;
class TextureCubeMap;
}  // namespace Bang

using namespace Bang;

Light::Light(){CONSTRUCT_CLASS_ID(Light)}

Light::~Light()
{
}

void Light::SetColor(const Color &color)
{
    m_color = color;
}
void Light::SetIntensity(float intensity)
{
    m_intensity = intensity;
}
void Light::SetShadowBias(float shadowBias)
{
    m_shadowBias = shadowBias;
}
void Light::SetShadowType(ShadowType shadowType)
{
    m_shadowType = shadowType;
}
void Light::SetShadowMapSize(const Vector2i &shadowMapSize)
{
    m_shadowMapSize = shadowMapSize;
}

const Color &Light::GetColor() const
{
    return m_color;
}
float Light::GetIntensity() const
{
    return m_intensity;
}
float Light::GetShadowBias() const
{
    return m_shadowBias;
}
Light::ShadowType Light::GetShadowType() const
{
    return m_shadowType;
}
const Vector2i &Light::GetShadowMapSize() const
{
    return m_shadowMapSize;
}
Texture *Light::GetShadowMapTexture() const
{
    return nullptr;
}

void Light::RenderShadowMaps(GameObject *go)
{
    if (GetShadowType() != ShadowType::NONE)
    {
        RenderShadowMaps_(go);
    }
}

void Light::ApplyLight(Camera *camera, const AARect &renderRect) const
{
    GL::Push(GL::BindTarget::SHADER_PROGRAM);

    ShaderProgram *lightSP = p_lightScreenPassShaderProgram.Get();
    lightSP->Bind();
    SetUniformsBeforeApplyingLight(lightSP);

    // Intersect with light rect to draw exactly what we need
    GBuffer *gbuffer = camera->GetGBuffer();
    AARect improvedRenderRect =
        AARect::Intersection(GetRenderRect(camera), renderRect);
    // Additive blend
    gbuffer->ApplyPassBlend(lightSP,
                            GL::BlendFactor::ONE,
                            GL::BlendFactor::ONE,
                            improvedRenderRect);

    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
}

void Light::SetUniformsBeforeApplyingLight(ShaderProgram *sp) const
{
    ASSERT(GL::IsBound(sp))

    Transform *tr = GetGameObject()->GetTransform();
    sp->SetInt("B_LightShadowType", int(GetShadowType()));
    sp->SetFloat("B_LightShadowBias", GetShadowBias());
    sp->SetFloat("B_LightIntensity", GetIntensity());
    sp->SetColor("B_LightColor", GetColor());
    sp->SetVector3("B_LightForwardWorld", tr->GetForward());
    sp->SetVector3("B_LightPositionWorld", tr->GetPosition());
    if (DCAST<Texture2D *>(GetShadowMapTexture()))
    {
        sp->SetTexture2D("B_LightShadowMap",
                         SCAST<Texture2D *>(GetShadowMapTexture()));
        sp->SetTexture2D("B_LightShadowMapSoft",
                         SCAST<Texture2D *>(GetShadowMapTexture()));
    }
    else
    {
        sp->SetTextureCubeMap("B_LightShadowMap",
                              SCAST<TextureCubeMap *>(GetShadowMapTexture()));
        sp->SetTextureCubeMap("B_LightShadowMapSoft",
                              SCAST<TextureCubeMap *>(GetShadowMapTexture()));
    }
}

Array<Renderer *> Light::GetShadowCastersIn(GameObject *go) const
{
    Array<Renderer *> validShadowCastersRends;

    Array<Renderer *> shadowCastersRends =
        go->GetComponentsInDescendantsAndThis<Renderer>();
    for (Renderer *rend : shadowCastersRends)
    {
        if (rend->IsActiveRecursively() && rend->GetCastsShadows())
        {
            bool isValidShadowCaster = false;
            if (const Material *mat = rend->GetActiveMaterial())
            {
                isValidShadowCaster =
                    (mat->GetRenderPass() == RenderPass::SCENE &&
                     rend->GetCastsShadows());
            }

            if (isValidShadowCaster)
            {
                validShadowCastersRends.PushBack(rend);
            }
        }
    }

    return validShadowCastersRends;
}

void Light::SetLightScreenPassShaderProgram(ShaderProgram *sp)
{
    p_lightScreenPassShaderProgram.Set(sp);
}

AARect Light::GetRenderRect(Camera *camera) const
{
    BANG_UNUSED(camera);
    return AARect::NDCRect;
}

void Light::Reflect()
{
    Serializable::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(Light,
                                   "Intensity",
                                   SetIntensity,
                                   GetIntensity,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    BANG_REFLECT_VAR_MEMBER(Light, "Color", SetColor, GetColor);
    BANG_REFLECT_VAR_MEMBER_HINTED(Light,
                                   "Shadow Bias",
                                   SetShadowBias,
                                   GetShadowBias,
                                   BANG_REFLECT_HINT_SLIDER(0.0f, 0.1f));
    BANG_REFLECT_VAR_MEMBER_ENUM(
        Light, "Shadow Type", SetShadowType, GetShadowType);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Shadow Type", "None", Light::ShadowType::NONE);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Shadow Type", "Hard", Light::ShadowType::HARD);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Shadow Type", "Soft", Light::ShadowType::SOFT);
    ReflectVar<uint>("Shadow Map Size",
                     [this](uint size) { SetShadowMapSize(Vector2i(size)); },
                     [this]() -> uint { return GetShadowMapSize().x; },
                     BANG_REFLECT_HINT_MIN_VALUE(1));
}
