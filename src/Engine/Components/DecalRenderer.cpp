#include "Bang/DecalRenderer.h"

#include "Bang/Assets.h"
#include "Bang/Extensions.h"
#include "Bang/GBuffer.h"
#include "Bang/GEngine.h"
#include "Bang/GameObject.h"
#include "Bang/Material.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/ShaderProgram.h"
#include "Bang/ShaderProgramFactory.h"
#include "Bang/Texture2D.h"
#include "Bang/Texture2D.h"
#include "Bang/TextureFactory.h"
#include "Bang/Transform.h"

using namespace Bang;

DecalRenderer::DecalRenderer()
{
    SET_INSTANCE_CLASS_ID(DecalRenderer);

    m_cubeMesh = MeshFactory::GetCube();
    GetMaterial()->GetShaderProgramProperties().SetCullFace(
        GL::CullFaceExt::NONE);
    GetMaterial()->SetShaderProgram(ShaderProgramFactory::GetDecal());
    GetMaterial()->GetShaderProgramProperties().SetRenderPass(
        RenderPass::SCENE_DECALS);
    SetDepthMask(false);

    SetDecalTexture(TextureFactory::GetBang2048Icon());
}

DecalRenderer::~DecalRenderer()
{
}

void DecalRenderer::OnRender()
{
    Renderer::OnRender();

    ShaderProgram *sp = GetActiveMaterial()->GetShaderProgram();

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
    if (GetIsPerspective())
    {
        // Set new scale to cover all the frustum
        Vector3 newScale;
        float fovRad = Math::DegToRad(GetFieldOfViewDegrees());
        newScale.y = Math::Tan(fovRad) * GetZFar() * 2 * 1.5f;
        newScale.x = newScale.y * GetAspectRatio();
        newScale.z = GetZFar() * 1.5f;
        transformMatrix.SetScale(newScale);
    }
    else
    {
        transformMatrix.SetScale(GetBoxSize());
    }
    return transformMatrix;
}

void DecalRenderer::SetIsPerspective(bool isPerspective)
{
    m_perspective = isPerspective;
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

void DecalRenderer::SetBoxSize(const Vector3 &boxSize)
{
    m_boxSize = boxSize;
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

const Vector3 &DecalRenderer::GetBoxSize() const
{
    return m_boxSize;
}

float DecalRenderer::GetAspectRatio() const
{
    return m_aspectRatio;
}

bool DecalRenderer::GetIsPerspective() const
{
    return m_perspective;
}

Texture2D *DecalRenderer::GetDecalTexture() const
{
    return GetActiveMaterial()->GetAlbedoTexture();
}

Matrix4 DecalRenderer::GetViewMatrix() const
{
    Matrix4 viewMatrix;
    if (GetIsPerspective())
    {
        Transform *tr = GetGameObject()->GetTransform();
        viewMatrix = Matrix4::TranslateMatrix(tr->GetPosition()) *
                     Matrix4::RotateMatrix(tr->GetRotation());
        viewMatrix = viewMatrix.Inversed();
    }
    else
    {
        Transform *tr = GetGameObject()->GetTransform();
        viewMatrix = Matrix4::TranslateMatrix(tr->GetPosition()) *
                     Matrix4::RotateMatrix(tr->GetRotation()) *
                     Matrix4::ScaleMatrix(GetBoxSize());
        viewMatrix = viewMatrix.Inversed();
    }
    return viewMatrix;
}

Matrix4 DecalRenderer::GetProjectionMatrix() const
{
    Matrix4 projMatrix;
    if (GetIsPerspective())
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

    BANG_REFLECT_VAR_ASSET(
        "Texture",
        SetDecalTexture,
        GetDecalTexture,
        Texture2D,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetImageExtensions()));

    BANG_REFLECT_VAR_MEMBER(
        DecalRenderer, "Perspective", SetIsPerspective, GetIsPerspective);

    BANG_REFLECT_VAR_MEMBER_HINTED(
        DecalRenderer,
        "Box Size",
        SetBoxSize,
        GetBoxSize,
        BANG_REFLECT_HINT_MIN_VALUE(Vector3(0)) +
            BANG_REFLECT_HINT_SHOWN(!GetIsPerspective()));

    BANG_REFLECT_VAR_MEMBER_HINTED(
        DecalRenderer,
        "Field of View",
        SetFieldOfViewDegrees,
        GetFieldOfViewDegrees,
        BANG_REFLECT_HINT_MINMAX_VALUE(0.0f, 179.0f) +
            BANG_REFLECT_HINT_SHOWN(GetIsPerspective()));

    BANG_REFLECT_VAR_MEMBER_HINTED(DecalRenderer,
                                   "Aspect Ratio",
                                   SetAspectRatio,
                                   GetAspectRatio,
                                   BANG_REFLECT_HINT_SHOWN(GetIsPerspective()));

    BANG_REFLECT_VAR_MEMBER_HINTED(
        DecalRenderer,
        "Near plane",
        SetZNear,
        GetZNear,
        BANG_REFLECT_HINT_MINMAX_VALUE(0.05f, GetZFar()) +
            BANG_REFLECT_HINT_SHOWN(GetIsPerspective()));

    BANG_REFLECT_VAR_MEMBER_HINTED(
        DecalRenderer,
        "Far plane",
        SetZFar,
        GetZFar,
        BANG_REFLECT_HINT_MIN_VALUE(GetZNear()) +
            BANG_REFLECT_HINT_SHOWN(GetIsPerspective()));
}
