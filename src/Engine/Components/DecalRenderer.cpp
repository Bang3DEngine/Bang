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

    GL::Render(m_cubeMesh.Get()->GetVAO(),
               GL::Primitive::TRIANGLES,
               m_cubeMesh.Get()->GetNumVerticesIds());
}

void DecalRenderer::SetDecalTexture(Texture2D *decalTexture)
{
    GetMaterial()->SetAlbedoTexture(decalTexture);
}

Vector3 DecalRenderer::GetBoxSize() const
{
    return GetGameObject()->GetTransform()->GetScale();
}

Texture2D *DecalRenderer::GetDecalTexture() const
{
    return GetMaterial()->GetAlbedoTexture();
}

Matrix4 DecalRenderer::GetProjectionMatrix() const
{
    Matrix4 projMatrix;
    projMatrix = Matrix4::Ortho(-0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0.5f);
    return projMatrix;
}
