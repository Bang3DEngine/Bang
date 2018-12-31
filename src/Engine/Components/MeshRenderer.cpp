#include "Bang/MeshRenderer.h"

#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/ClassDB.h"
#include "Bang/Extensions.h"
#include "Bang/GL.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/Geometry.h"
#include "Bang/ICloneable.h"
#include "Bang/Material.h"
#include "Bang/MaterialFactory.h"
#include "Bang/Math.h"
#include "Bang/Mesh.h"
#include "Bang/MeshFactory.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Plane.h"
#include "Bang/ReflectionProbe.h"
#include "Bang/ShaderProgram.h"
#include "Bang/Transform.h"
#include "Bang/Triangle.h"

using namespace Bang;

MeshRenderer::MeshRenderer()
{
    SET_INSTANCE_CLASS_ID(MeshRenderer);
    SetRenderPrimitive(GL::Primitive::TRIANGLES);
    SetMaterial(MaterialFactory::GetDefault().Get());
    SetMesh(MeshFactory::GetCube().Get());
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::SetMesh(Mesh *m)
{
    if (GetSharedMesh() != m)
    {
        p_sharedMesh.Set(m);
        p_mesh.Set(nullptr);
    }
}

Mesh *MeshRenderer::GetMesh() const
{
    if (!p_mesh)
    {
        if (GetSharedMesh())
        {
            p_mesh = Assets::Clone<Mesh>(GetSharedMesh());
        }
    }
    return p_mesh.Get();
}

Mesh *MeshRenderer::GetSharedMesh() const
{
    return p_sharedMesh.Get();
}

void MeshRenderer::Bind()
{
    Renderer::Bind();
}

void MeshRenderer::SetUniformsOnBind(ShaderProgram *sp)
{
    Renderer::SetUniformsOnBind(sp);

    sp->SetBool("B_HasBoneAnimations", false);
    ReflectionProbe::SetRendererUniforms(this, sp);
}

void MeshRenderer::SetCurrentLOD(int lod)
{
    int maxLOD = GetActiveMesh() ? GetActiveMesh()->GetNumLODs() - 1 : 0;
    m_currentLOD = Math::Clamp(lod, 0, maxLOD);
}

void MeshRenderer::SetAutoLOD(bool autoLOD)
{
    m_autoLOD = autoLOD;
}

bool MeshRenderer::GetAutoLOD() const
{
    return m_autoLOD;
}

int MeshRenderer::GetCurrentLOD() const
{
    return m_currentLOD;
}

Mesh *MeshRenderer::GetCurrentLODActiveMesh() const
{
    return GetActiveMesh() ? GetActiveMesh()->GetLODMesh(GetCurrentLOD()).Get()
                           : nullptr;
}

void MeshRenderer::IntersectRay(const Ray &ray,
                                bool *outIntersected,
                                Vector3 *outIntersectionPoint,
                                uint *outTriId) const
{
    IntersectRay_(ray, nullptr, outIntersected, outIntersectionPoint, outTriId);
}

void MeshRenderer::IntersectRay(const Ray &ray,
                                Texture2D *textureToFilterBy,
                                bool *outIntersected,
                                Vector3 *outIntersectionPoint,
                                uint *outTriId) const
{
    IntersectRay_(
        ray, textureToFilterBy, outIntersected, outIntersectionPoint, outTriId);
}

bool IsFilteredByTexture(const MeshRenderer *mr,
                         Texture2D *textureToFilterBy,
                         const Vector3 &point,
                         uint triId)
{
    if (textureToFilterBy || mr->GetActiveMaterial())
    {
        Mesh *mesh = mr->GetActiveMesh();
        Texture2D *tex = textureToFilterBy
                             ? textureToFilterBy
                             : mr->GetActiveMaterial()->GetAlbedoTexture();
        if (!tex)
        {
            return false;
        }

        Vector2 pointUvs = mesh->GetTriangleUvsAtPoint(triId, point);
        pointUvs.y = (1.0f - pointUvs.y);

        Vector2i pixelCoords(Vector2(tex->GetImage().GetSize()) * pointUvs);

        if (tex && pixelCoords >= Vector2i::Zero() &&
            pixelCoords < tex->GetImage().GetSize())
        {
            Color pixelColor =
                tex->GetImage().GetPixel(pixelCoords.x, pixelCoords.y);
            if (pixelColor.a <= tex->GetAlphaCutoff())
            {
                return true;
            }
        }
    }
    return false;
}

void MeshRenderer::IntersectRay_(const Ray &ray,
                                 Texture2D *textureToFilterBy,
                                 bool *outIntersected,
                                 Vector3 *outIntersectionPoint,
                                 uint *outTriId) const
{
    Mesh *mesh = GetActiveMesh();
    if (!mesh)
    {
        if (outIntersected)
        {
            *outIntersected = false;
        }
        return;
    }

    bool intersectedATri = false;
    Mesh::TriangleId intersectedTriId = -1u;
    float minIntersectionDist = Math::Infinity<float>();

    bool intersected = false;
    float dist = Math::Infinity<float>();

    Geometry::IntersectRayAABox(
        ray, GetGameObject()->GetAABBoxWorld(), &intersected, &dist);
    if (intersected && dist < minIntersectionDist)
    {
        Transform *tr = GetGameObject()->GetTransform();
        const Matrix4 &localToWorldInv = tr->GetWorldToLocalMatrix();
        const Ray localRay = localToWorldInv * ray;

        float minLocalMRDist = Math::Infinity<float>();
        for (Mesh::TriangleId triId = 0; triId < mesh->GetNumTriangles();
             ++triId)
        {
            const Triangle tri = mesh->GetTriangle(triId);

            Geometry::IntersectRayPlane(
                localRay, tri.GetPlane(), &intersected, &dist);
            if (intersected && dist < minLocalMRDist)
            {
                Geometry::IntersectRayTriangle(
                    localRay, tri, &intersected, &dist);

                if (intersected && dist < minLocalMRDist)
                {
                    bool filteredByTexture = false;
                    if (textureToFilterBy)
                    {
                        Vector3 localMRPoint = localRay.GetPoint(dist);
                        filteredByTexture = IsFilteredByTexture(
                            this, textureToFilterBy, localMRPoint, triId);
                    }

                    if (!filteredByTexture)
                    {
                        minLocalMRDist = dist;
                        intersectedATri = true;
                        intersectedTriId = triId;
                    }
                }
            }
        }

        if (intersectedATri)
        {
            const Matrix4 &localToWorld = tr->GetLocalToWorldMatrix();
            const Vector3 minLocalMRPoint = localRay.GetPoint(minLocalMRDist);
            const Vector3 minWorldMRPoint =
                localToWorld.TransformedPoint(minLocalMRPoint);

            if (outIntersectionPoint)
            {
                *outIntersectionPoint = minWorldMRPoint;
            }

            if (outTriId)
            {
                *outTriId = intersectedTriId;
            }
        }
    }

    if (outIntersected)
    {
        *outIntersected = intersectedATri;
    }
}

AABox MeshRenderer::GetAABBox() const
{
    return GetActiveMesh() ? GetActiveMesh()->GetAABBox() : AABox::Empty();
}

void MeshRenderer::OnRender()
{
    Renderer::OnRender();

    if (Mesh *baseMeshToRender = GetActiveMesh())
    {
        Mesh *lodMeshToRender =
            baseMeshToRender->GetLODMesh(GetCurrentLOD()).Get();
        GL::Render(lodMeshToRender->GetVAO(),
                   GetRenderPrimitive(),
                   lodMeshToRender->GetNumVerticesIds());
    }
}

Mesh *MeshRenderer::GetActiveMesh() const
{
    if (p_mesh)
    {
        return GetMesh();
    }
    return GetSharedMesh();
}

void MeshRenderer::Reflect()
{
    Renderer::Reflect();

    BANG_REFLECT_VAR_ASSET(
        "Mesh",
        SetMesh,
        GetSharedMesh,
        Mesh,
        BANG_REFLECT_HINT_EXTENSIONS(Extensions::GetMeshExtension()) +
            BANG_REFLECT_HINT_ZOOMABLE_PREVIEW(true));
}
