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
    sp->SetMatrix4("B_DecalViewMatrix", GetViewMatrix());
    sp->SetMatrix4("B_DecalProjectionMatrix", projMatrix);
    sp->SetTexture2D("B_DecalTexture", GetDecalTexture());

    GL::Push(GL::Enablable::DEPTH_TEST);
    GL::Disable(GL::Enablable::DEPTH_TEST);

    GL::Render(m_cubeMesh.Get()->GetVAO(),
               GL::Primitive::TRIANGLES,
               m_cubeMesh.Get()->GetNumVerticesIds());

    GL::Pop(GL::Enablable::DEPTH_TEST);
}

Matrix4 DecalRenderer::GetModelMatrixUniform() const
{
    Matrix4 transformMatrix = Renderer::GetModelMatrixUniform();
    if (GetIsProjective())
    {
        // Set new scale to cover all the frustum
        Vector3 newScale;
        float fovRad = Math::DegToRad(GetFieldOfViewDegrees());
        newScale.y = Math::Tan(fovRad) * GetZFar() * 2 * 1.5f;
        newScale.x = newScale.y * GetAspectRatio();
        newScale.z = GetZFar() * 1.5f;
        transformMatrix.SetScale(newScale);
    }
    return transformMatrix;
}

void DecalRenderer::SetIsProjective(bool isProjective)
{
    m_projective = isProjective;
}

void DecalRenderer::SetFieldOfViewDegrees(float fieldOfViewDegrees)
{
    m_fieldOfViewDegrees = fieldOfViewDegrees;
}

void DecalRenderer::SetAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
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

float DecalRenderer::GetFieldOfViewDegrees() const
{
    return m_fieldOfViewDegrees;
}

Vector3 DecalRenderer::GetBoxSize() const
{
    return GetGameObject()->GetTransform()->GetScale();
}

float DecalRenderer::GetAspectRatio() const
{
    return m_aspectRatio;
}

bool DecalRenderer::GetIsProjective() const
{
    return m_projective;
}

Texture2D *DecalRenderer::GetDecalTexture() const
{
    return GetMaterial()->GetAlbedoTexture();
}

Matrix4 DecalRenderer::GetViewMatrix() const
{
    Matrix4 viewMatrix;
    if (GetIsProjective())
    {
        Transform *tr = GetGameObject()->GetTransform();
        viewMatrix = Matrix4::TranslateMatrix(tr->GetPosition()) *
                     Matrix4::RotateMatrix(tr->GetRotation());
        viewMatrix = viewMatrix.Inversed();
    }
    else
    {
        viewMatrix = GetModelMatrixUniform().Inversed();
    }
    return viewMatrix;
}

Matrix4 DecalRenderer::GetProjectionMatrix() const
{
    Matrix4 projMatrix;
    if (GetIsProjective())
    {
        projMatrix =
            Matrix4::Perspective(Math::DegToRad(GetFieldOfViewDegrees()),
                                 GetAspectRatio(),
                                 GetZNear(),
                                 GetZFar());
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
    GetReflectStructPtr()
        ->GetReflectVariablePtr("Material")
        ->GetHintsPtr()
        ->SetIsShown(false);

    BANG_REFLECT_VAR_MEMBER(
        DecalRenderer, "Projective", SetIsProjective, GetIsProjective);

    BANG_REFLECT_VAR_MEMBER_HINTED(
        DecalRenderer,
        "Field of View",
        SetFieldOfViewDegrees,
        GetFieldOfViewDegrees,
        BANG_REFLECT_HINT_MINMAX_VALUE(0.0f, 179.0f));
    BANG_REFLECT_VAR_MEMBER(
        DecalRenderer, "Aspect Ratio", SetAspectRatio, GetAspectRatio);
    BANG_REFLECT_VAR_MEMBER_HINTED(
        DecalRenderer,
        "Near plane",
        SetZNear,
        GetZNear,
        BANG_REFLECT_HINT_MINMAX_VALUE(0.05f, GetZFar()));
    BANG_REFLECT_VAR_MEMBER_HINTED(
        DecalRenderer,
        "Far plane",
        SetZFar,
        GetZFar,
        BANG_REFLECT_HINT_MIN_VALUE(GetZNear() + 0.001f));
}
