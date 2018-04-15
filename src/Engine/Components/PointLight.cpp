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

    m_shadowMapShaderProgram.Set( ShaderProgramFactory::GetPointLightShadowMap() );
    SetLightMaterial(MaterialFactory::GetPointLight().Get());
}

PointLight::~PointLight()
{
    delete m_shadowMapFramebuffer;
    delete m_shadowMapTexCubeMap;
}

void PointLight::SetUniformsBeforeApplyingLight(Material* mat) const
{
    Light::SetUniformsBeforeApplyingLight(mat);

    ShaderProgram *sp = mat->GetShaderProgram();
    if (!sp) { return; }
    ASSERT(GL::IsBound(sp));
    sp->Set("B_LightRange", GetRange());
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
        Gizmos::SetColor(GetColor());
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
    GLId prevBoundFB = GL::GetBoundId(m_shadowMapFramebuffer->GetGLBindTarget());
    GLId prevBoundSP = GL::GetBoundId(m_shadowMapShaderProgram.Get()->GetGLBindTarget());

    // Resize stuff to fit the shadow map size
    Vector2i shadowMapSize(256);
    m_shadowMapFramebuffer->Bind();
    GL::SetViewport(0, 0, shadowMapSize.x, shadowMapSize.y);
    m_shadowMapFramebuffer->Resize(shadowMapSize.x, shadowMapSize.y);
    m_shadowMapTexCubeMap->Resize(shadowMapSize.x);
    m_shadowMapFramebuffer->SetAllDrawBuffers();

    m_shadowMapShaderProgram.Get()->Bind();

    Scene *scene = GetGameObject()->GetScene();
    const Transform *tr = GetGameObject()->GetTransform();
    const Vector3 pos = tr->GetPosition();
    const Matrix4 perspective = Matrix4::Perspective(Math::DegToRad(90.0f),
                                                     1.0f,
                                                     0.05f,
                                                     GetRange());
    Array<Matrix4> cubeMapPVMMatrices;
    cubeMapPVMMatrices.Resize(6);
    cubeMapPVMMatrices[2] = perspective *
                            Matrix4::LookAt(pos, (pos + tr->GetUp()), tr->GetRight());
    cubeMapPVMMatrices[3] = perspective *
                            Matrix4::LookAt(pos, (pos + tr->GetDown()), tr->GetRight());
    cubeMapPVMMatrices[1] = perspective *
                            Matrix4::LookAt(pos, (pos + tr->GetLeft()), tr->GetUp());
    cubeMapPVMMatrices[0] = perspective *
                            Matrix4::LookAt(pos, (pos + tr->GetRight()), tr->GetUp());
    cubeMapPVMMatrices[5] = perspective *
                            Matrix4::LookAt(pos, (pos + tr->GetForward()), tr->GetUp());
    cubeMapPVMMatrices[4] = perspective *
                            Matrix4::LookAt(pos, (pos + tr->GetBack()), tr->GetUp());
    m_shadowMapShaderProgram.Get()->Set("B_PointLightShadowMapMatrices",
                                        cubeMapPVMMatrices);
    m_shadowMapShaderProgram.Get()->Set("B_PointLightPosition", tr->GetPosition());

    // Render shadow map into framebuffer
    GEngine::GetActive()->SetReplacementShader( m_shadowMapShaderProgram.Get() );
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
    GL::Bind(m_shadowMapFramebuffer->GetGLBindTarget(), prevBoundFB);
    GL::Bind(m_shadowMapShaderProgram.Get()->GetGLBindTarget(), prevBoundSP);
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
