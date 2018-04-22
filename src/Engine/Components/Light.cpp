#include "Bang/Light.h"

#include "Bang/GL.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/XMLNode.h"
#include "Bang/Material.h"
#include "Bang/Renderer.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

Light::Light() { }
Light::~Light() { }

void Light::SetColor(const Color &color) { m_color = color; }
void Light::SetIntensity(float intensity) { m_intensity = intensity; }
void Light::SetShadowBias(float shadowBias) { m_shadowBias = shadowBias; }
void Light::SetShadowType(ShadowType shadowType) { m_shadowType = shadowType; }
void Light::SetShadowMapSize(const Vector2i &shadowMapSize)
{
    m_shadowMapSize = shadowMapSize;
}

Color Light::GetColor() const { return m_color; }
float Light::GetIntensity() const { return m_intensity; }
float Light::GetShadowBias() const { return m_shadowBias; }
Light::ShadowType Light::GetShadowType() const { return m_shadowType; }
const Vector2i &Light::GetShadowMapSize() const { return m_shadowMapSize; }
Texture *Light::GetShadowMapTexture() const { return nullptr; }

void Light::RenderShadowMaps()
{
    if (GetShadowType() != ShadowType::NONE)
    {
        RenderShadowMaps_();
    }
}

void Light::ApplyLight(Camera *camera, const AARect &renderRect) const
{
    // Save state
    GLId prevBoundSP = GL::GetBoundId(GL::BindTarget::ShaderProgram);

    ShaderProgram *lightSP = p_lightScreenPassShaderProgram.Get();

    lightSP->Bind();
    SetUniformsBeforeApplyingLight(lightSP);

    // Intersect with light rect to draw exactly what we need
    GBuffer *gbuffer = camera->GetGBuffer();
    AARect improvedRenderRect = AARect::Intersection(GetRenderRect(camera),
                                                 renderRect);
    gbuffer->ApplyPass(lightSP, true, improvedRenderRect);

    // Restore state
    GL::Bind(GL::BindTarget::ShaderProgram, prevBoundSP);
}

void Light::SetUniformsBeforeApplyingLight(ShaderProgram* sp) const
{
    ASSERT(GL::IsBound(sp))

    Transform *tr = GetGameObject()->GetTransform();
    sp->SetInt    ("B_LightShadowType",    int( GetShadowType() ), false);
    sp->SetFloat  ("B_LightShadowBias",    GetShadowBias(),        false);
    sp->SetFloat  ("B_LightIntensity",     GetIntensity(),         false);
    sp->SetColor  ("B_LightColor",         GetColor(),             false);
    sp->SetVector3("B_LightForwardWorld",  tr->GetForward(),       false);
    sp->SetVector3("B_LightPositionWorld", tr->GetPosition(),      false);
    sp->SetTexture("B_LightShadowMap",     GetShadowMapTexture(),  false);
    sp->SetTexture("B_LightShadowMapSoft", GetShadowMapTexture(),  false);
}

List<GameObject *> Light::GetActiveSceneShadowCasters() const
{
    Set<GameObject*> shadowCastersSet;
    Scene *scene = SceneManager::GetActiveScene();
    List<Renderer*> renderers = scene->GetComponentsInChildren<Renderer>(true);
    for (Renderer *rend : renderers )
    {
        bool isValidShadowCaster = false;
        if (const Material *mat = rend->GetActiveMaterial())
        {
            isValidShadowCaster = (mat->GetRenderPass() == RenderPass::Scene &&
                                   rend->GetCastsShadows());
        }

        if (isValidShadowCaster)
        {
            shadowCastersSet.Add(rend->GetGameObject());
        }
    }

    List<GameObject*> shadowCastersList = shadowCastersSet.GetKeys();
    return shadowCastersList;
}

void Light::SetLightScreenPassShaderProgram(ShaderProgram *sp)
{
    p_lightScreenPassShaderProgram.Set(sp);
}

AARect Light::GetRenderRect(Camera *camera) const
{
    return AARect::NDCRect;
}

void Light::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Light *l = Cast<Light*>(clone);
    l->SetIntensity(GetIntensity());
    l->SetColor(GetColor());
    l->SetShadowBias( GetShadowBias() );
    l->SetShadowType( GetShadowType() );
    l->SetShadowMapSize( GetShadowMapSize() );
}

void Light::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);

    if (xmlInfo.Contains("Intensity"))
    { SetIntensity(xmlInfo.Get<float>("Intensity")); }

    if (xmlInfo.Contains("Color"))
    { SetColor(xmlInfo.Get<Color>("Color")); }

    if (xmlInfo.Contains("ShadowBias"))
    { SetShadowBias(xmlInfo.Get<float>("ShadowBias")); }

    if (xmlInfo.Contains("ShadowType"))
    { SetShadowType(xmlInfo.Get<ShadowType>("ShadowType")); }

    if (xmlInfo.Contains("ShadowMapSize"))
    { SetShadowMapSize(xmlInfo.Get<Vector2i>("ShadowMapSize")); }
}

void Light::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    xmlInfo->Set("Intensity", GetIntensity());
    xmlInfo->Set("Color", GetColor());
    xmlInfo->Set("ShadowBias", GetShadowBias());
    xmlInfo->Set("ShadowType", GetShadowType());
    xmlInfo->Set("ShadowMapSize", GetShadowMapSize());
}
