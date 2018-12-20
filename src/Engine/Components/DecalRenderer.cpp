#include "Bang/DecalRenderer.h"

#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GameObject.h"
#include "Bang/Material.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureFactory.h"
#include "Bang/Transform.h"

using namespace Bang;

DecalRenderer::DecalRenderer()
{
    CONSTRUCT_CLASS_ID(DecalRenderer);

    m_cubeMesh = MeshFactory::GetCube();
    GetMaterial()->SetCullFace(GL::CullFaceExt::NONE);
    GetMaterial()->SetShaderProgram(ShaderProgramFactory::GetDecal());
    GetMaterial()->SetRenderPass(RenderPass::SCENE_DECALS);
    SetDepthMask(false);

    SetDecalTexture(TextureFactory::GetBang2048Icon());
}

DecalRenderer::~DecalRenderer()
{
}

void DecalRenderer::OnRender()
{
    Renderer::OnRender();

    ShaderProgram *sp = GetMaterial()->GetShaderProgram();

    GBuffer *gbuffer = GEngine::GetActiveGBuffer();
    gbuffer->BindAttachmentsForReading(sp);

    Matrix4 projMatrix = GetProjectionMatrix();
    sp->SetMatrix4("B_DecalProjectionMatrix", projMatrix);
    sp->SetTexture2D("B_DecalTexture", GetDecalTexture());

    GL::Push(GL::Enablable::DEPTH_TEST);
    GL::Disable(GL::Enablable::DEPTH_TEST);

    GL::Render(m_cubeMesh.Get()->GetVAO(),
               GL::Primitive::TRIANGLES,
               m_cubeMesh.Get()->GetNumVerticesIds());

    GL::Pop(GL::Enablable::DEPTH_TEST);
}

void DecalRenderer::SetIsProjective(bool isProjective)
{
    m_projective = isProjective;
}

void DecalRenderer::SetFieldOfView(float fieldOfView)
{
    m_fieldOfView = fieldOfView;
}

void DecalRenderer::SetZNear(float zNear)
{
    m_zNear = zNear;
}

void DecalRenderer::SetZFar(float zFar)
{
    m_zFar = zFar;
}

void DecalRenderer::SetDecalTexture(Texture2D *decalTexture)
{
    GetMaterial()->SetAlbedoTexture(decalTexture);
}

float DecalRenderer::GetZNear() const
{
    return m_zNear;
}

float DecalRenderer::GetZFar() const
{
    return m_zFar;
}

float DecalRenderer::GetFieldOfView() const
{
    return m_fieldOfView;
}

Vector3 DecalRenderer::GetBoxSize() const
{
    return GetGameObject()->GetTransform()->GetScale();
}

float DecalRenderer::GetAspectRatio() const
{
    Vector3 boxSize = GetBoxSize();
    float aspectRatio = Math::Abs(boxSize.x / Math::Max(boxSize.y, 0.0001f));
    return aspectRatio;
}

bool DecalRenderer::GetIsProjective() const
{
    return m_projective;
}

Texture2D *DecalRenderer::GetDecalTexture() const
{
    return GetMaterial()->GetAlbedoTexture();
}

Matrix4 DecalRenderer::GetProjectionMatrix() const
{
    Matrix4 projMatrix;
    if (GetIsProjective())
    {
        projMatrix = Matrix4::Perspective(
            GetFieldOfView(), GetAspectRatio(), GetZNear(), GetZFar());
    }
    else
    {
        projMatrix = Matrix4::Ortho(-0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0.5f);
    }
    return projMatrix;
}

void DecalRenderer::Reflect()
{
    Renderer::Reflect();

    GetReflectStructPtr()
        ->GetReflectVariablePtr("Depth Mask")
        ->GetHintsPtr()
        ->SetIsShown(false);
    GetReflectStructPtr()
        ->GetReflectVariablePtr("Casts Shadows")
        ->GetHintsPtr()
        ->SetIsShown(false);
    GetReflectStructPtr()
        ->GetReflectVariablePtr("Receives Shadows")
        ->GetHintsPtr()
        ->SetIsShown(false);

    BANG_REFLECT_VAR_MEMBER(
        DecalRenderer, "Projective", SetIsProjective, GetIsProjective);

    BANG_REFLECT_VAR_MEMBER(
        DecalRenderer, "Field of View", SetFieldOfView, GetFieldOfView);
    BANG_REFLECT_VAR_MEMBER(DecalRenderer, "Near plane", SetZNear, GetZNear);
    BANG_REFLECT_VAR_MEMBER(DecalRenderer, "Far plane", SetZFar, GetZFar);
}
