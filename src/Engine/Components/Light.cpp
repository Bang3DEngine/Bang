#include "Bang/Light.h"

#include "Bang/GL.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Camera.h"
#include "Bang/GBuffer.h"
#include "Bang/MetaNode.h"
#include "Bang/Material.h"
#include "Bang/Renderer.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/GLUniforms.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"

USING_NAMESPACE_BANG

Light::Light()
{
    CONSTRUCT_CLASS_ID(Light)
}

Light::~Light()
{
}

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
    GL::Push(GL::BindTarget::SHADER_PROGRAM);

    ShaderProgram *lightSP = p_lightScreenPassShaderProgram.Get();
    lightSP->Bind();
    SetUniformsBeforeApplyingLight(lightSP);

    // Intersect with light rect to draw exactly what we need
    GBuffer *gbuffer = camera->GetGBuffer();
    AARect improvedRenderRect = AARect::Intersection(GetRenderRect(camera),
                                                     renderRect);
    // Additive blend
    gbuffer->ApplyPassBlend(lightSP,
                            GL::BlendFactor::ONE,
                            GL::BlendFactor::ONE,
                            improvedRenderRect);

    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
}

void Light::SetUniformsBeforeApplyingLight(ShaderProgram* sp) const
{
    ASSERT(GL::IsBound(sp))

    Transform *tr = GetGameObject()->GetTransform();
    sp->SetInt    ("B_LightShadowType",    int( GetShadowType() ));
    sp->SetFloat  ("B_LightShadowBias",    GetShadowBias());
    sp->SetFloat  ("B_LightIntensity",     GetIntensity());
    sp->SetColor  ("B_LightColor",         GetColor());
    sp->SetVector3("B_LightForwardWorld",  tr->GetForward());
    sp->SetVector3("B_LightPositionWorld", tr->GetPosition());
    if (DCAST<Texture2D*>(GetShadowMapTexture()))
    {
        sp->SetTexture2D("B_LightShadowMap",
                         SCAST<Texture2D*>(GetShadowMapTexture()));
        sp->SetTexture2D("B_LightShadowMapSoft",
                         SCAST<Texture2D*>(GetShadowMapTexture()));
    }
    else
    {
        sp->SetTextureCubeMap("B_LightShadowMap",
                              SCAST<TextureCubeMap*>(GetShadowMapTexture()));
        sp->SetTextureCubeMap("B_LightShadowMapSoft",
                              SCAST<TextureCubeMap*>(GetShadowMapTexture()));
    }
}

List<GameObject*> Light::GetActiveSceneShadowCasters() const
{
    USet<GameObject*> shadowCastersSet;
    Scene *scene = SceneManager::GetActiveScene();
    Array<Renderer*> renderers =
                        scene->GetComponentsInDescendantsAndThis<Renderer>();
    for (Renderer *rend : renderers )
    {
        if (rend->IsActive() && rend->GetCastsShadows())
        {
            bool isValidShadowCaster = false;
            if (const Material *mat = rend->GetActiveMaterial())
            {
                isValidShadowCaster = (mat->GetRenderPass() == RenderPass::SCENE &&
                                       rend->GetCastsShadows());
            }

            if (isValidShadowCaster)
            {
                shadowCastersSet.Add(rend->GetGameObject());
            }
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

void Light::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("Intensity"))
    { SetIntensity(metaNode.Get<float>("Intensity")); }

    if (metaNode.Contains("Color"))
    { SetColor(metaNode.Get<Color>("Color")); }

    if (metaNode.Contains("ShadowBias"))
    { SetShadowBias(metaNode.Get<float>("ShadowBias")); }

    if (metaNode.Contains("ShadowType"))
    { SetShadowType(metaNode.Get<ShadowType>("ShadowType")); }

    if (metaNode.Contains("ShadowMapSize"))
    { SetShadowMapSize(metaNode.Get<Vector2i>("ShadowMapSize")); }
}

void Light::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("Intensity", GetIntensity());
    metaNode->Set("Color", GetColor());
    metaNode->Set("ShadowBias", GetShadowBias());
    metaNode->Set("ShadowType", GetShadowType());
    metaNode->Set("ShadowMapSize", GetShadowMapSize());
}
