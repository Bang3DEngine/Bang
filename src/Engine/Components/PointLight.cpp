#include "Bang/PointLight.h"

#include <GL/glew.h>

#include "Bang/AABox.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Camera.h"
#include "Bang/ClassDB.h"
#include "Bang/Framebuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GL.h"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/Material.h"
#include "Bang/Math.h"
#include "Bang/Matrix4.tcc"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/RenderPass.h"
#include "Bang/Renderer.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Sphere.h"
#include "Bang/Texture2D.h"
#include "Bang/Transform.h"

using namespace Bang;

PointLight::PointLight() : Light()
{
    SET_INSTANCE_CLASS_ID(PointLight)

    m_shadowMapFramebuffer = new Framebuffer();
    m_shadowMapFramebuffer->CreateAttachmentTexCubeMap(
        GL::Attachment::COLOR0, GL::ColorFormat::RGBA32F);
    m_shadowMapFramebuffer->CreateAttachmentTexCubeMap(
        GL::Attachment::DEPTH, GL::ColorFormat::DEPTH16);

    m_blurredShadowMapTexCM = Assets::Create<TextureCubeMap>();
    m_blurredShadowMapTexCM.Get()->SetFormat(GL::ColorFormat::RGBA32F);
    m_blurredShadowMapTexCM.Get()->SetWrapMode(GL::WrapMode::CLAMP_TO_EDGE);
    m_blurredShadowMapTexCM.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
    m_blurredShadowMapTexCM.Get()->CreateEmpty(1);

    m_blurAuxiliarShadowMapTexCM = Assets::Create<TextureCubeMap>();
    m_blurAuxiliarShadowMapTexCM.Get()->SetFormat(GL::ColorFormat::RGBA32F);
    m_blurAuxiliarShadowMapTexCM.Get()->SetWrapMode(
        GL::WrapMode::CLAMP_TO_EDGE);
    m_blurAuxiliarShadowMapTexCM.Get()->SetFilterMode(GL::FilterMode::BILINEAR);
    m_blurAuxiliarShadowMapTexCM.Get()->CreateEmpty(1);

    SetShadowMapShaderProgram(ShaderProgramFactory::GetPointLightShadowMap());
    SetLightScreenPassShaderProgram(
        ShaderProgramFactory::GetPointLightDeferredScreenPass());
}

PointLight::~PointLight()
{
    delete m_shadowMapFramebuffer;
}

void PointLight::SetUniformsBeforeApplyingLight(ShaderProgram *sp) const
{
    Light::SetUniformsBeforeApplyingLight(sp);

    ASSERT(GL::IsBound(sp));
    sp->SetFloat("B_LightRange", GetRange());
}

AARect PointLight::GetRenderRect(Camera *cam) const
{
    Transform *tr = GetGameObject()->GetTransform();
    Sphere sphere(tr->GetPosition(), Math::Pow(GetRange(), 2.0f));
    AABox bbox = AABox::FromSphere(sphere);
    return cam->GetViewportBoundingAARectNDC(bbox);
}

void PointLight::SetRange(float range)
{
    m_range = range;
}

void PointLight::SetShadowNearPlane(float nearPlane)
{
    m_shadowNearPlane = nearPlane;
}

float PointLight::GetRange() const
{
    return m_range;
}

float PointLight::GetShadowMapNearDistance() const
{
    return m_shadowNearPlane;
}

float PointLight::GetShadowMapFarDistance() const
{
    return GetRange();
}

TextureCubeMap *PointLight::GetShadowMapTexture() const
{
    return GetShadowSoftness() > 0
               ? m_blurredShadowMapTexCM.Get()
               : m_shadowMapFramebuffer->GetAttachmentTexCubeMap(
                     GL::Attachment::COLOR0);
}

void PointLight::Reflect()
{
    Light::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(PointLight,
                                   "Range",
                                   SetRange,
                                   GetRange,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));

    BANG_REFLECT_VAR_MEMBER_HINTED(PointLight,
                                   "Shadow near plane",
                                   SetShadowNearPlane,
                                   GetShadowMapNearDistance,
                                   BANG_REFLECT_HINT_SLIDER(0.05f, 5.0f) +
                                       BANG_REFLECT_HINT_STEP_VALUE(0.05f));
}

void PointLight::RenderShadowMaps_(GameObject *go)
{
    GL::Push(GL::Pushable::VIEWPORT);
    GL::Push(GL::Pushable::COLOR_MASK);
    GL::Push(GL::Pushable::ALL_MATRICES);
    GL::Push(GL::Pushable::DEPTH_STATES);
    GL::Push(GL::Pushable::VIEWPROJ_MODE);
    GL::Push(GL::BindTarget::SHADER_PROGRAM);
    GL::Push(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);

    GEngine *ge = GEngine::GetInstance();
    ge->PushActiveRenderingCamera();

    // Resize stuff to fit the shadow map size
    const Vector2i shadowMapSize = GetShadowMapSize();
    m_shadowMapFramebuffer->Bind();
    m_shadowMapFramebuffer->Resize(shadowMapSize.x, shadowMapSize.y);
    m_shadowMapFramebuffer->SetAllDrawBuffers();

    // Set up viewport
    GL::SetViewport(0, 0, shadowMapSize.x, shadowMapSize.y);

    Array<Matrix4> cubeMapPVMMatrices = GetWorldToShadowMapMatrices();
    GetShadowMapShaderProgram()->SetMatrix4Array("B_WorldToShadowMapMatrices",
                                                 cubeMapPVMMatrices);

    // Render shadow map into framebuffer
    GL::SetDepthMask(true);
    GL::ClearDepthBuffer(1.0f);
    GL::SetDepthFunc(GL::Function::LEQUAL);
    float limit = Math::Exp(GetShadowExponentConstant());
    GL::ClearColorBuffer(Color(limit));

    float rangeLimit = Math::Pow(GetRange(), 1.0f);
    const Vector3 pointLightPos =
        GetGameObject()->GetTransform()->GetPosition();
    const Array<Renderer *> shadowCastersRenderers = GetShadowCastersIn(go);
    for (Renderer *shadowCasterRend : shadowCastersRenderers)
    {
        AABox shadowCasterAABoxWorld = shadowCasterRend->GetGameObject()
                                           ->GetTransform()
                                           ->GetLocalToWorldMatrix() *
                                       shadowCasterRend->GetAABBox();
        Vector3 closestPointInAABox =
            shadowCasterAABoxWorld.GetClosestPointInAABB(pointLightPos);
        bool isCompletelyOutside =
            Vector3::Distance(closestPointInAABox, pointLightPos) > rangeLimit;
        if (!isCompletelyOutside)
        {
            shadowCasterRend->OnRender(RenderPass::SCENE_OPAQUE);
        }
    }

    // Blur shadow map
    if (GetShadowSoftness() > 0)
    {
        TextureCubeMap *shadowMapTexCM =
            m_shadowMapFramebuffer->GetAttachmentTexCubeMap(
                GL::Attachment::COLOR0);
        GEngine::GetInstance()->BlurTextureCM(
            shadowMapTexCM,
            m_blurAuxiliarShadowMapTexCM.Get(),
            m_blurredShadowMapTexCM.Get(),
            GetShadowSoftness());
    }

    ge->PopActiveRenderingCamera();

    GL::Pop(GL::Pushable::FRAMEBUFFER_AND_READ_DRAW_ATTACHMENTS);
    GL::Pop(GL::BindTarget::SHADER_PROGRAM);
    GL::Pop(GL::Pushable::VIEWPROJ_MODE);
    GL::Pop(GL::Pushable::ALL_MATRICES);
    GL::Pop(GL::Pushable::DEPTH_STATES);
    GL::Pop(GL::Pushable::COLOR_MASK);
    GL::Pop(GL::Pushable::VIEWPORT);
}

float PointLight::GetLightZFar() const
{
    return GetRange();
}

Array<Matrix4> PointLight::GetWorldToShadowMapMatrices() const
{
    Array<Matrix4> cubeMapPVMMatrices;

    const Transform *tr = GetGameObject()->GetTransform();
    const Vector3 pos = tr->GetPosition();
    const Matrix4 pers = Matrix4::Perspective(Math::DegToRad(90.0f),
                                              1.0f,
                                              GetShadowMapNearDistance(),
                                              GetShadowMapFarDistance());
    const Vector3 up = Vector3::Up(), down = Vector3::Down(),
                  left = Vector3::Left(), right = Vector3::Right(),
                  fwd = Vector3::Forward(), back = Vector3::Back();
    cubeMapPVMMatrices.Resize(6);
    cubeMapPVMMatrices[0] = pers * Matrix4::LookAt(pos, (pos + right), down);
    cubeMapPVMMatrices[1] = pers * Matrix4::LookAt(pos, (pos + left), down);
    cubeMapPVMMatrices[2] = pers * Matrix4::LookAt(pos, (pos + up), back);
    cubeMapPVMMatrices[3] = pers * Matrix4::LookAt(pos, (pos + down), fwd);
    cubeMapPVMMatrices[4] = pers * Matrix4::LookAt(pos, (pos + back), down);
    cubeMapPVMMatrices[5] = pers * Matrix4::LookAt(pos, (pos + fwd), down);

    return cubeMapPVMMatrices;
}
