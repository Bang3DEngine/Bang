#include "Bang/PointLight.h"

#include "Bang/GL.h"
#include "Bang/AABox.h"
#include "Bang/Scene.h"
#include "Bang/AARect.h"
#include "Bang/Gizmos.h"
#include "Bang/Sphere.h"
#include "Bang/GEngine.h"
#include "Bang/XMLNode.h"
#include "Bang/Texture2D.h"
#include "Bang/Transform.h"
#include "Bang/GLUniforms.h"
#include "Bang/GameObject.h"
#include "Bang/Framebuffer.h"
#include "Bang/IconManager.h"
#include "Bang/MeshRenderer.h"
#include "Bang/MaterialFactory.h"
#include "Bang/ShaderProgramFactory.h"

USING_NAMESPACE_BANG

PointLight::PointLight() : Light()
{
    m_shadowMapFramebuffer = new Framebuffer();

    m_shadowMapTexCubeMap = new TextureCubeMap();

    m_shadowMapFramebuffer->CreateAttachmentTexCubeMap(GL::Attachment::Depth,
                                                       GL::ColorFormat::Depth16);

    GLId prevBoundTex = GL::GetBoundId(GetShadowMapTexture()->GetGLBindTarget());
    GetShadowMapTexture()->Bind();
    GetShadowMapTexture()->SetFilterMode(GL::FilterMode::Bilinear);
    GL::TexParameteri( GetShadowMapTexture()->GetTextureTarget(),
                       GL::TexParameter::TEXTURE_COMPARE_MODE,
                       GL_COMPARE_REF_TO_TEXTURE );
    GL::Bind(GetShadowMapTexture()->GetGLBindTarget(), prevBoundTex);

    m_shadowMapShaderProgram.Set( ShaderProgramFactory::GetPointLightShadowMap() );
    SetLightMaterial(MaterialFactory::GetPointLight().Get());
}

PointLight::~PointLight()
{
    delete m_shadowMapFramebuffer;
    delete m_shadowMapTexCubeMap;
}

void PointLight::SetUniformsBeforeApplyingLight(ShaderProgram* sp) const
{
    Light::SetUniformsBeforeApplyingLight(sp);

    ASSERT(GL::IsBound(sp));
    sp->SetFloat("B_LightRange", GetRange(), false);
    sp->SetFloat("B_PointLightZFar", GetLightZFar(), false);
}

AARect PointLight::GetRenderRect(Camera *cam) const
{
    Transform *tr = GetGameObject()->GetTransform();
    Sphere sphere(tr->GetPosition(), GetRange());
    AABox bbox = AABox::FromSphere(sphere);
    return bbox.GetAABoundingViewportRect(cam);
}

void PointLight::SetRange(float range) { m_range = range; }
float PointLight::GetRange() const { return m_range; }

TextureCubeMap *PointLight::GetShadowMapTexture() const
{
    return m_shadowMapFramebuffer->GetAttachmentTexCubeMap(GL::Attachment::Depth);
}

void PointLight::OnRender(RenderPass rp)
{
    Component::OnRender(rp);
    if (rp == RenderPass::Overlay) // Render gizmos
    {
        Gizmos::Reset();
        Gizmos::SetColor(GetColor().WithAlpha(1.0f));
        Gizmos::SetSelectable(GetGameObject());
        Gizmos::SetPosition( GetGameObject()->GetTransform()->GetPosition() );
        Gizmos::SetScale( Vector3(0.1f) );
        Gizmos::RenderIcon( IconManager::GetLightBulbIcon().Get(), true );
    }
}

void PointLight::RenderShadowMaps_()
{
    // Save previous state
    AARecti prevVP = GL::GetViewportRect();
    const Matrix4 &prevModel = GLUniforms::GetModelMatrix();
    const Matrix4 &prevView  = GLUniforms::GetViewMatrix();
    const Matrix4 &prevProj  = GLUniforms::GetProjectionMatrix();
    GLId prevBoundFB = GL::GetBoundId(GL::BindTarget::Framebuffer);
    GLId prevBoundSP = GL::GetBoundId(GL::BindTarget::ShaderProgram);

    // Resize stuff to fit the shadow map size
    const Vector2i shadowMapSize = GetShadowMapSize();
    m_shadowMapFramebuffer->Bind();
    GL::SetViewport(0, 0, shadowMapSize.x, shadowMapSize.y);
    m_shadowMapFramebuffer->Resize(shadowMapSize.x, shadowMapSize.y);
    m_shadowMapTexCubeMap->Resize(shadowMapSize.x);
    m_shadowMapFramebuffer->SetAllDrawBuffers();

    m_shadowMapShaderProgram.Get()->Bind();

    SetUniformsBeforeApplyingLight(m_shadowMapShaderProgram.Get());
    Array<Matrix4> cubeMapPVMMatrices = GetWorldToShadowMapMatrices();
    m_shadowMapShaderProgram.Get()->SetMatrix4Array("B_WorldToShadowMapMatrices",
                                                    cubeMapPVMMatrices, false);

    // Render shadow map into framebuffer
    Scene *scene = GetGameObject()->GetScene();
    GEngine::GetActive()->SetReplacementShader( m_shadowMapShaderProgram.Get() );
    GL::SetColorMask(false, false, false, false);
    GL::ClearDepthBuffer(1.0f);
    GL::SetDepthFunc(GL::Function::LEqual);
    GEngine::GetActive()->RenderWithPassRaw(scene, RenderPass::Scene);

    // Restore previous state
    GL::SetViewport(prevVP);
    GL::SetColorMask(true, true, true, true);
    GLUniforms::SetModelMatrix(prevModel);
    GLUniforms::SetViewMatrix(prevView);
    GLUniforms::SetProjectionMatrix(prevProj);
    GEngine::GetActive()->SetReplacementShader(nullptr);
    GL::Bind(GL::BindTarget::Framebuffer,   prevBoundFB);
    GL::Bind(GL::BindTarget::ShaderProgram, prevBoundSP);
}

float PointLight::GetLightZFar() const
{
    return GetRange();
}

Array<Matrix4> PointLight::GetWorldToShadowMapMatrices() const
{
    Array<Matrix4> cubeMapPVMMatrices;

    const Transform *tr = GetGameObject()->GetTransform();
    const Vector3 pos  = tr->GetPosition();
    const Matrix4 pers = Matrix4::Perspective(Math::DegToRad(90.0f),
                                              1.0f,
                                              0.05f,
                                              GetLightZFar());
    const Vector3 up = Vector3::Up, down = Vector3::Down, left = Vector3::Left,
          right = Vector3::Right, fwd = Vector3::Forward, back = Vector3::Back;
    cubeMapPVMMatrices.Resize(6);
    cubeMapPVMMatrices[0] = pers * Matrix4::LookAt(pos, (pos + right), down);
    cubeMapPVMMatrices[1] = pers * Matrix4::LookAt(pos, (pos + left),  down);
    cubeMapPVMMatrices[2] = pers * Matrix4::LookAt(pos, (pos + up),    back);
    cubeMapPVMMatrices[3] = pers * Matrix4::LookAt(pos, (pos + down),  fwd);
    cubeMapPVMMatrices[4] = pers * Matrix4::LookAt(pos, (pos + back),  down);
    cubeMapPVMMatrices[5] = pers * Matrix4::LookAt(pos, (pos + fwd),   down);

    return cubeMapPVMMatrices;
}

void PointLight::CloneInto(ICloneable *clone) const
{
    Light::CloneInto(clone);
    PointLight *pl = Cast<PointLight*>(clone);
    pl->SetRange(GetRange());
}

void PointLight::ImportXML(const XMLNode &xmlInfo)
{
    Light::ImportXML(xmlInfo);
    if (xmlInfo.Contains("Range"))
    { SetRange(xmlInfo.Get<float>("Range")); }
}

void PointLight::ExportXML(XMLNode *xmlInfo) const
{
    Light::ExportXML(xmlInfo);
    xmlInfo->Set("Range", GetRange());
}
