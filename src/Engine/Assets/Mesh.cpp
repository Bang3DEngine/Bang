#include "Bang/Mesh.h"

#include <sys/types.h>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "Bang/Animation.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Containers.h"
#include "Bang/GL.h"
#include "Bang/Geometry.h"
#include "Bang/IBO.h"
#include "Bang/IToString.h"
#include "Bang/Math.h"
#include "Bang/MeshSimplifier.h"
#include "Bang/MetaNode.h"
#include "Bang/Ray.h"
#include "Bang/Set.h"
#include "Bang/Set.tcc"
#include "Bang/Triangle.h"
#include "Bang/UMap.tcc"
#include "Bang/USet.h"
#include "Bang/USet.tcc"
#include "Bang/VAO.h"
#include "Bang/VBO.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Path;
}

using namespace Bang;

Mesh::Mesh() : m_bBox(Vector3::Zero())
{
    m_vao = new VAO();
}

Mesh::~Mesh()
{
    if (m_vao)
    {
        delete m_vao;
    }

    if (m_vertexIdsIBO)
    {
        delete m_vertexIdsIBO;
    }

    if (m_vertexAttributesVBO)
    {
        delete m_vertexAttributesVBO;
    }
}

void Mesh::SetTrianglesVertexIds(const Array<Mesh::VertexId> &trisVerticesIds)
{
    m_areCornerTablesValid = false;

    m_triangleVertexIds = trisVerticesIds;

    if (m_vertexIdsIBO)
    {
        delete m_vertexIdsIBO;
    }

    m_vertexIdsIBO = new IBO();
    if (trisVerticesIds.Size() >= 1)
    {
        m_vertexIdsIBO->Fill(
            (void *)(&m_triangleVertexIds[0]),
            m_triangleVertexIds.Size() * sizeof(Mesh::VertexId));
    }
    m_vao->SetIBO(m_vertexIdsIBO);  // Bind to VAO
    m_areLodsValid = false;
}

void Mesh::SetBonesIds(const Map<String, uint> &bonesIds)
{
    m_bonesIds = bonesIds;
    for (const auto &pair : bonesIds)
    {
        m_idToBone.Add(pair.second, pair.first);
    }
}

void Mesh::SetPosition(Mesh::VertexId vId, const Vector3 &pos)
{
    m_positionsPool[vId] = pos;
}

void Mesh::UpdateVAOs(bool createIndicesIfNeeded)
{
    if (m_vertexAttributesVBO)
    {
        delete m_vertexAttributesVBO;
    }
    m_vertexAttributesVBO = new VBO();

    bool hasPos = !GetPositionsPool().IsEmpty();
    bool hasNormals = !GetNormalsPool().IsEmpty();
    bool hasUvs = !GetUvsPool().IsEmpty();
    bool hasTangents = !GetTangentsPool().IsEmpty();
    bool hasBones = !GetBonesPool().IsEmpty();

    if (createIndicesIfNeeded && hasPos && GetTrianglesVertexIds().IsEmpty())
    {
        Array<Mesh::VertexId> triVertexIds;
        for (uint i = 0; i < GetPositionsPool().Size(); ++i)
        {
            triVertexIds.PushBack(i);
        }
        SetTrianglesVertexIds(triVertexIds);
    }

    if (hasBones)
    {
        // Pick 4 most relevant bones per vertex
        for (VertexId vid = 0; vid < GetTrianglesVertexIds().Size(); ++vid)
        {
            struct BoneExt : public IToString
            {
                float weight;
                uint id;
                String ToString() const override
                {
                    return "Bone(" + String::ToString(id) + ": " +
                           String::ToString(weight) + ")";
                }
            };

            Array<BoneExt> vertexBonesExt;
            for (const auto &it : GetBonesPool())
            {
                const String &boneName = it.first;
                const Mesh::Bone &bone = it.second;
                if (bone.weights.ContainsKey(vid))
                {
                    ASSERT(GetBonesIds().ContainsKey(boneName));

                    BoneExt boneExt;
                    boneExt.weight = bone.weights.Get(vid);
                    boneExt.id = GetBonesIds().Get(boneName);
                    vertexBonesExt.PushBack(boneExt);
                }
            }

            m_vertexIdToImportantBonesIdsPool.Add(
                vid, std::array<int, 4>({{0, 0, 0, 0}}));
            m_vertexIdToImportantBonesWeightsPool.Add(
                vid,
                // std::array<float, 4>({{0,0,0,0}}));
                std::array<float, 4>({{0, 0, 0, 0}}));

            if (!vertexBonesExt.IsEmpty())
            {
                Containers::Sort(
                    vertexBonesExt.Begin(),
                    vertexBonesExt.End(),
                    [](const BoneExt &lhs, const BoneExt &rhs) -> bool {
                        return lhs.weight > rhs.weight;
                    });

                uint numImportantBones = Math::Min(4u, vertexBonesExt.Size());

                float weightSum = 0.0f;
                for (uint i = 0; i < numImportantBones; ++i)
                {
                    weightSum += vertexBonesExt[i].weight;  // To norm later
                }

                for (uint i = 0; i < numImportantBones; ++i)
                {
                    m_vertexIdToImportantBonesIdsPool.Get(vid)[i] =
                        vertexBonesExt[i].id;
                    m_vertexIdToImportantBonesWeightsPool.Get(vid)[i] =
                        vertexBonesExt[i].weight / weightSum;
                }

                // std::cerr << "id: " <<
                // m_vertexIdToImportantBonesIdsPool.Get(vid) << std::endl;
                // std::cerr << "wei: " <<
                // m_vertexIdToImportantBonesWeightsPool.Get(vid) << std::endl;
            }
        }
    }

    Array<float> interleavedAttributes;
    for (uint i = 0; i < GetPositionsPool().Size(); ++i)
    {
        if (i < GetPositionsPool().Size())
        {
            const Vector3 &position = GetPositionsPool()[i];
            interleavedAttributes.PushBack(position.x);
            interleavedAttributes.PushBack(position.y);
            interleavedAttributes.PushBack(position.z);
        }

        if (i < GetNormalsPool().Size())
        {
            const Vector3 &normal = GetNormalsPool()[i];
            interleavedAttributes.PushBack(normal.x);
            interleavedAttributes.PushBack(normal.y);
            interleavedAttributes.PushBack(normal.z);
        }

        if (i < GetUvsPool().Size())
        {
            const Vector2 &uv = GetUvsPool()[i];
            interleavedAttributes.PushBack(uv.x);
            interleavedAttributes.PushBack(uv.y);
        }

        if (i < GetTangentsPool().Size())
        {
            const Vector3 &tangent = GetTangentsPool()[i];
            interleavedAttributes.PushBack(tangent.x);
            interleavedAttributes.PushBack(tangent.y);
            interleavedAttributes.PushBack(tangent.z);
        }

        if (i < m_vertexIdToImportantBonesIdsPool.Size())
        {
            const auto &vertexIdToImportantBonesIds =
                m_vertexIdToImportantBonesIdsPool.Get(i);
            interleavedAttributes.PushBack(vertexIdToImportantBonesIds[0]);
            interleavedAttributes.PushBack(vertexIdToImportantBonesIds[1]);
            interleavedAttributes.PushBack(vertexIdToImportantBonesIds[2]);
            interleavedAttributes.PushBack(vertexIdToImportantBonesIds[3]);
        }

        if (i < m_vertexIdToImportantBonesWeightsPool.Size())
        {
            const auto &vertexIdToImportantBonesWeights =
                m_vertexIdToImportantBonesWeightsPool.Get(i);
            interleavedAttributes.PushBack(vertexIdToImportantBonesWeights[0]);
            interleavedAttributes.PushBack(vertexIdToImportantBonesWeights[1]);
            interleavedAttributes.PushBack(vertexIdToImportantBonesWeights[2]);
            interleavedAttributes.PushBack(vertexIdToImportantBonesWeights[3]);
        }
    }

    if (interleavedAttributes.Size() >= 1)
    {
        GetVertexAttributesVBO()->CreateAndFill(
            SCAST<void *>(&interleavedAttributes[0]),
            interleavedAttributes.Size() * sizeof(float));
    }

    uint vboStride = GetVBOStride();

    if (hasPos)
    {
        GetVAO()->SetVBO(GetVertexAttributesVBO(),
                         Mesh::DefaultPositionsVBOLocation,
                         3,
                         GL::VertexAttribDataType::FLOAT,
                         false,
                         vboStride,
                         GetVBOPositionsOffset());
    }

    if (hasNormals)
    {
        GetVAO()->SetVBO(GetVertexAttributesVBO(),
                         Mesh::DefaultNormalsVBOLocation,
                         3,
                         GL::VertexAttribDataType::FLOAT,
                         true,
                         vboStride,
                         GetVBONormalsOffset());
    }

    if (hasUvs)
    {
        GetVAO()->SetVBO(GetVertexAttributesVBO(),
                         Mesh::DefaultUvsVBOLocation,
                         2,
                         GL::VertexAttribDataType::FLOAT,
                         false,
                         vboStride,
                         GetVBOUvsOffset());
    }

    if (hasTangents)
    {
        GetVAO()->SetVBO(GetVertexAttributesVBO(),
                         Mesh::DefaultTangentsVBOLocation,
                         3,
                         GL::VertexAttribDataType::FLOAT,
                         false,
                         vboStride,
                         GetVBOTangentsOffset());
    }

    if (hasBones)
    {
        GetVAO()->SetVBO(GetVertexAttributesVBO(),
                         Mesh::DefaultVertexToBonesIdsVBOLocation,
                         4,
                         GL::VertexAttribDataType::FLOAT,
                         false,
                         vboStride,
                         GetVBOBonesIdsOffset());

        GetVAO()->SetVBO(GetVertexAttributesVBO(),
                         Mesh::DefaultVertexToBonesWeightsVBOLocation,
                         4,
                         GL::VertexAttribDataType::FLOAT,
                         false,
                         vboStride,
                         GetVBOBonesWeightsOffset());
    }
}

void Mesh::SetPositionsPool(const Array<Vector3> &positions)
{
    if (positions.Size() != GetPositionsPool().Size())
    {
        m_areCornerTablesValid = false;
    }

    m_positionsPool = positions;
    m_bBox.CreateFromPositions(m_positionsPool);
    m_bSphere.FromBox(m_bBox);
    m_areLodsValid = false;
}

void Mesh::SetNormalsPool(const Array<Vector3> &normals)
{
    m_normalsPool = normals;
    m_areLodsValid = false;
}

void Mesh::SetUvsPool(const Array<Vector2> &uvs)
{
    m_uvsPool = uvs;
    m_areLodsValid = false;
}

void Mesh::SetTangentsPool(const Array<Vector3> &tangents)
{
    m_tangentsPool = tangents;
    m_areLodsValid = false;
}

void Mesh::SetBonesPool(const Map<String, Mesh::Bone> &bones)
{
    m_bonesPool = bones;
}

void Mesh::UpdateVAOsAndTables()
{
    UpdateVAOs();
    UpdateCornerTablesIfNeeded();
}

void Mesh::UpdateVertexNormals()
{
    UpdateCornerTablesIfNeeded();

    Array<Vector3> normalsPool;
    for (VertexId vi = 0; vi < GetNumVertices(); ++vi)
    {
        Vector3 normal = Vector3::Zero();
        const Array<CornerId> &cornerIds = GetCornerIdsFromVertexId(vi);
        for (CornerId cornerId : cornerIds)
        {
            TriangleId triId = GetTriangleIdFromCornerId(cornerId);
            Triangle tri = GetTriangle(triId);
            normal += tri.GetNormal();
        }
        normal = normal / SCAST<float>(cornerIds.Size());
        normalsPool.PushBack(normal);
    }
    SetNormalsPool(normalsPool);
}

struct LexicographicCompare
{
    bool operator()(const Vector3 &lhs, const Vector3 &rhs) const
    {
        for (uint i = 0; i < 3; ++i)
        {
            if (lhs[i] < rhs[i])
            {
                return true;
            }
            else if (lhs[i] > rhs[i])
            {
                return false;
            }
        }
        return false;
    }
};

struct PairVector3Comparator
{
    bool operator()(const std::pair<Vector3, Vector3> &lhs,
                    const std::pair<Vector3, Vector3> &rhs) const
    {
        LexicographicCompare LexicoCompare;
        if (lhs.first != rhs.first)
        {
            return LexicoCompare(lhs.first, rhs.first);
        }
        return LexicoCompare(lhs.second, rhs.second);
    }
};

void Mesh::UpdateCornerTablesIfNeeded()
{
    if (!IsIndexed() || m_areCornerTablesValid)
    {
        return;
    }

    const uint numTriangles = GetNumTriangles();

    m_cornerIdToOppositeCornerId.Clear();
    m_cornerIdToOppositeCornerId.Resize(GetNumCorners());
    for (uint i = 0; i < m_cornerIdToOppositeCornerId.Size(); ++i)
    {
        m_cornerIdToOppositeCornerId[i] = SCAST<uint>(-1);
    }

    struct TriMapInfo
    {
        TriangleId triangleId;
        VertexId firstCornerId;
        VertexId secondCornerId;
    };

    LexicographicCompare LexicoCompare;
    Map<std::pair<Vector3, Vector3>, TriMapInfo, PairVector3Comparator>
        connectedCornerIdPairs;
    for (TriangleId triId = 0; triId < numTriangles; ++triId)
    {
        for (uint i = 0; i < 3; ++i)
        {
            CornerId triFirstCId = (triId * 3) + ((i + 0) % 3);
            CornerId triSecondCId = (triId * 3) + ((i + 1) % 3);
            const VertexId triFirstVId = GetVertexIdFromCornerId(triFirstCId);
            const VertexId triSecondVId = GetVertexIdFromCornerId(triSecondCId);
            ASSERT(triFirstVId < GetNumVertices());
            ASSERT(triSecondVId < GetNumVertices());
            const Vector3 &triFirstVertexPos = GetPositionsPool()[triFirstVId];
            const Vector3 &triSecondVertexPos =
                GetPositionsPool()[triSecondVId];
            const auto orderedCornerPositions =
                (LexicoCompare(triFirstVertexPos, triSecondVertexPos))
                    ? std::make_pair(triFirstVertexPos, triSecondVertexPos)
                    : std::make_pair(triSecondVertexPos, triFirstVertexPos);
            if (connectedCornerIdPairs.ContainsKey(orderedCornerPositions))
            {
                // There was already another tri with this edge:
                // Consequently, this and the other are adjacent
                const TriMapInfo &otherTriInfo =
                    connectedCornerIdPairs[orderedCornerPositions];

                CornerId triOppCId =
                    GetRemainingCornerId(triId, triFirstCId, triSecondCId);
                VertexId otherTriOppCId =
                    GetRemainingCornerId(otherTriInfo.triangleId,
                                         otherTriInfo.firstCornerId,
                                         otherTriInfo.secondCornerId);

                ASSERT(m_cornerIdToOppositeCornerId[triOppCId] ==
                       SCAST<uint>(-1));
                ASSERT(m_cornerIdToOppositeCornerId[otherTriOppCId] ==
                       SCAST<uint>(-1));
                m_cornerIdToOppositeCornerId[triOppCId] = otherTriOppCId;
                m_cornerIdToOppositeCornerId[otherTriOppCId] = triOppCId;
            }
            else
            {
                TriMapInfo triInfo;
                triInfo.triangleId = triId;
                triInfo.firstCornerId = triFirstCId;
                triInfo.secondCornerId = triSecondCId;
                connectedCornerIdPairs[orderedCornerPositions] = triInfo;
            }
        }
    }

    Map<Vector3, Array<VertexId>, LexicographicCompare>
        vertexPositionToVertexIdsInThatPosition;
    for (uint i = 0; i < GetNumVertices(); ++i)
    {
        const Vector3 &vertexPos = GetPositionsPool()[i];
        vertexPositionToVertexIdsInThatPosition[vertexPos].PushBack(i);
    }

    m_vertexIdToCornerIds.Clear();
    m_vertexIdToCornerIds.Resize(GetNumVertices());
    m_vertexIdToSamePositionMinimumVertexId.Resize(GetNumVerticesIds());
    for (CornerId cId = 0; cId < GetNumCorners(); ++cId)
    {
        VertexId vIdOfcId = GetVertexIdFromCornerId(cId);
        const Vector3 &vertexPos = GetPositionsPool()[vIdOfcId];
        const Array<VertexId> &vertexIdsInThisPosition =
            vertexPositionToVertexIdsInThatPosition[vertexPos];
        VertexId minVId = SCAST<uint>(-1);
        for (VertexId vId : vertexIdsInThisPosition)
        {
            m_vertexIdToCornerIds[vId].PushBack(cId);
            minVId = Math::Min(vId, minVId);
        }
        m_vertexIdToSamePositionMinimumVertexId[vIdOfcId] = minVId;
    }

    m_areCornerTablesValid = true;
}

void Mesh::CalculateLODs()
{
    if (!m_areLodsValid)
    {
        m_lodMeshes = MeshSimplifier::GetAllMeshLODs(
            this,
            // MeshSimplifier::Method::CLUSTERING);
            MeshSimplifier::SimplificationMethod::QUADRIC_ERROR_METRICS);
        m_areLodsValid = true;
    }
}

int Mesh::GetNumLODs() const
{
    return GetLODMeshes().Size();
}

AH<Mesh> Mesh::GetLODMesh(int lod) const
{
    if (GetLODMeshes().IsEmpty())
    {
        return AH<Mesh>(const_cast<Mesh *>(this));
    }

    const int clampedLODLevel = Math::Clamp(lod, 0, GetNumLODs() - 1);
    return GetLODMeshes()[clampedLODLevel];
}

const Array<AH<Mesh>> Mesh::GetLODMeshes() const
{
    return m_lodMeshes;
}

uint Mesh::GetNumTriangles() const
{
    return SCAST<uint>(GetTrianglesVertexIds().Size() / 3);
}

uint Mesh::GetNumVerticesIds() const
{
    return IsIndexed() ? GetTrianglesVertexIds().Size() : GetNumVertices();
}

String Mesh::GetBoneName(uint boneId) const
{
    if (m_idToBone.ContainsKey(boneId))
    {
        return m_idToBone.Get(boneId);
    }
    return "";
}

uint Mesh::GetBoneId(const String &boneName) const
{
    if (m_bonesIds.ContainsKey(boneName))
    {
        return m_bonesIds.Get(boneName);
    }
    return SCAST<uint>(-1);
}

Triangle Mesh::GetTriangle(TriangleId triId) const
{
    ASSERT(triId < GetNumTriangles());
    VertexId vId0 = GetTrianglesVertexIds()[triId * 3 + 0];
    VertexId vId1 = GetTrianglesVertexIds()[triId * 3 + 1];
    VertexId vId2 = GetTrianglesVertexIds()[triId * 3 + 2];

    Triangle tri;
    tri.SetPoint(0, GetPositionsPool()[vId0]);
    tri.SetPoint(1, GetPositionsPool()[vId1]);
    tri.SetPoint(2, GetPositionsPool()[vId2]);
    return tri;
}

std::array<Mesh::VertexId, 3> Mesh::GetVertexIdsFromTriangle(
    TriangleId triId) const
{
    ASSERT(triId < GetNumTriangles());
    const VertexId triVertex0Id = GetTrianglesVertexIds()[triId * 3 + 0];
    const VertexId triVertex1Id = GetTrianglesVertexIds()[triId * 3 + 1];
    const VertexId triVertex2Id = GetTrianglesVertexIds()[triId * 3 + 2];
    return {{triVertex0Id, triVertex1Id, triVertex2Id}};
}

std::array<Mesh::CornerId, 3> Mesh::GetCornerIdsFromTriangle(
    TriangleId triId) const
{
    std::array<CornerId, 3> cornerIds;
    cornerIds[0] = GetCornerIdFromTriangle(triId, 0);
    cornerIds[1] = GetCornerIdFromTriangle(triId, 1);
    cornerIds[2] = GetCornerIdFromTriangle(triId, 2);
    return cornerIds;
}

uint Mesh::GetNumVertices() const
{
    return GetPositionsPool().Size();
}

uint Mesh::GetPositionsBytesSize() const
{
    return GetPositionsPool().Size() >= 1 ? (3 * sizeof(float)) : 0;
}

uint Mesh::GetNormalsBytesSize() const
{
    return GetNormalsPool().Size() >= 1 ? (3 * sizeof(float)) : 0;
}

uint Mesh::GetUvsBytesSize() const
{
    return GetUvsPool().Size() >= 1 ? (2 * sizeof(float)) : 0;
}

uint Mesh::GetTangentsBytesSize() const
{
    return GetTangentsPool().Size() >= 1 ? (3 * sizeof(float)) : 0;
}

uint Mesh::GetBonesIdsBytesSize() const
{
    return GetBonesPool().Size() >= 1 ? (4 * sizeof(float)) : 0;
}

uint Mesh::GetBonesWeightsBytesSize() const
{
    return GetBonesPool().Size() >= 1 ? (4 * sizeof(float)) : 0;
}

uint Mesh::GetVBOPositionsOffset() const
{
    return 0;
}

uint Mesh::GetVBONormalsOffset() const
{
    return GetVBOPositionsOffset() + GetPositionsBytesSize();
}

uint Mesh::GetVBOUvsOffset() const
{
    return GetVBONormalsOffset() + GetNormalsBytesSize();
}

uint Mesh::GetVBOTangentsOffset() const
{
    return GetVBOUvsOffset() + GetUvsBytesSize();
}

uint Mesh::GetVBOBonesIdsOffset() const
{
    return GetVBOTangentsOffset() + GetTangentsBytesSize();
}

uint Mesh::GetVBOBonesWeightsOffset() const
{
    return GetVBOBonesIdsOffset() + GetBonesWeightsBytesSize();
}

uint Mesh::GetVBOStride() const
{
    return GetPositionsBytesSize() + GetNormalsBytesSize() + GetUvsBytesSize() +
           GetTangentsBytesSize() + GetBonesIdsBytesSize() +
           GetBonesWeightsBytesSize();
}

bool Mesh::IsIndexed() const
{
    return GetTriangleVertexIdsIBO();
}

uint Mesh::GetNumCorners() const
{
    return GetNumTriangles() * 3;
}

Mesh::VertexId Mesh::GetRemainingVertexId(Mesh::TriangleId triangleId,
                                          Mesh::VertexId oneVertex,
                                          Mesh::VertexId anotherVertex) const
{
    ASSERT(triangleId < GetNumTriangles());
    const VertexId beginVId = (triangleId * 3);
    ASSERT(beginVId < GetNumVerticesIds());

    for (uint i = 0; i < 3; ++i)
    {
        VertexId vId = GetTrianglesVertexIds()[beginVId + i];
        if (vId != oneVertex && vId != anotherVertex)
        {
            return vId;
        }
    }
    return SCAST<uint>(-1);
}

Vector2 Mesh::GetTriangleUvsAtPoint(Mesh::TriangleId triangleId,
                                    const Vector3 &point) const
{
    Triangle tri = GetTriangle(triangleId);

    uint triV0Id = GetTrianglesVertexIds()[triangleId * 3 + 0];
    uint triV1Id = GetTrianglesVertexIds()[triangleId * 3 + 1];
    uint triV2Id = GetTrianglesVertexIds()[triangleId * 3 + 2];
    const Array<Vector2> &uvs = GetUvsPool();
    Vector2 uvs0 = triV0Id < uvs.Size() ? uvs[triV0Id] : Vector2::Zero();
    Vector2 uvs1 = triV1Id < uvs.Size() ? uvs[triV1Id] : Vector2::Zero();
    Vector2 uvs2 = triV2Id < uvs.Size() ? uvs[triV2Id] : Vector2::Zero();

    Vector3 barycentricCoords = tri.GetBarycentricCoordinates(point);
    Vector2 pointUvs = (uvs0 * barycentricCoords[0]) +
                       (uvs1 * barycentricCoords[1]) +
                       (uvs2 * barycentricCoords[2]);
    return pointUvs;
}

Mesh::VertexId Mesh::GetRemainingVertexIdUnique(
    Mesh::TriangleId triangleId,
    Mesh::VertexId oneVertexId,
    Mesh::VertexId anotherVertexId) const
{
    // Takes into account vertex positions...i.e., it considers the vertices
    // in the same position as equal vertices.
    ASSERT(triangleId < GetNumTriangles());
    const VertexId beginVId = (triangleId * 3);
    ASSERT(beginVId < GetNumVerticesIds());
    VertexId uniqueOneVId = GetVertexIdUnique(oneVertexId);
    VertexId uniqueAnotherVId = GetVertexIdUnique(anotherVertexId);
    for (uint i = 0; i < 3; ++i)
    {
        VertexId vId = GetTrianglesVertexIds()[beginVId + i];
        VertexId uniqueVId = GetVertexIdUnique(vId);
        if (uniqueVId != uniqueOneVId && uniqueVId != uniqueAnotherVId)
        {
            return uniqueVId;
        }
    }
    return SCAST<uint>(-1);
}

Mesh::CornerId Mesh::GetRemainingCornerId(Mesh::TriangleId triangleId,
                                          Mesh::CornerId oneCorner,
                                          Mesh::CornerId anotherCorner) const
{
    Mesh::CornerId oneCornerLocalTriId = (oneCorner - triangleId * 3);
    Mesh::CornerId anotherCornerLocalTriId = (anotherCorner - triangleId * 3);
    ASSERT(oneCornerLocalTriId >= 0 && oneCornerLocalTriId < 3);
    ASSERT(anotherCornerLocalTriId >= 0 && anotherCornerLocalTriId < 3);

    uint remainingCornerLocalTriId = SCAST<uint>(-1);
    for (uint i = 0; i < 3; ++i)
    {
        if (i != oneCornerLocalTriId && i != anotherCornerLocalTriId)
        {
            remainingCornerLocalTriId = i;
            break;
        }
    }

    Mesh::CornerId remainingCornerId =
        (triangleId * 3) + remainingCornerLocalTriId;
    return remainingCornerId;
}

Mesh::CornerId Mesh::GetCornerIdFromTriangle(Mesh::TriangleId triangleId,
                                             uint i) const
{
    ASSERT(i >= 0 && i < 3);
    return (triangleId * 3) + i;
}

Mesh::CornerId Mesh::GetCornerIdFromTriangleIdAndVertexId(
    Mesh::TriangleId triangleId,
    Mesh::VertexId vertexId) const
{
    Array<Mesh::CornerId> cIds = GetCornerIdsFromVertexId(vertexId);
    for (Mesh::CornerId cId : cIds)
    {
        if (GetTriangleIdFromCornerId(cId) == triangleId)
        {
            return cId;
        }
    }
    return -1u;
}

Mesh::CornerId Mesh::GetTriangleIdFromCornerId(Mesh::CornerId cornerId) const
{
    return (cornerId / 3);
}

Mesh::VertexId Mesh::GetVertexIdFromCornerId(Mesh::CornerId cId) const
{
    return GetTrianglesVertexIds()[cId];
}

Mesh::VertexId Mesh::GetVertexIdUniqueFromCornerId(Mesh::CornerId cId) const
{
    return GetVertexIdUnique(GetVertexIdFromCornerId(cId));
}

Mesh::CornerId Mesh::GetNextCornerId(Mesh::CornerId cId) const
{
    return (cId / 3) * 3 + ((cId + 1) % 3);
}

Mesh::CornerId Mesh::GetPreviousCornerId(Mesh::CornerId cId) const
{
    return (cId / 3) * 3 + ((cId + 2) % 3);
}

Mesh::CornerId Mesh::GetOppositeCornerId(Mesh::CornerId cId) const
{
    ASSERT(cId < m_cornerIdToOppositeCornerId.Size());
    return m_cornerIdToOppositeCornerId[cId];
}

float Mesh::GetCornerAngleRads(Mesh::CornerId cId) const
{
    ASSERT(cId < GetNumCorners());
    const CornerId prevCId = GetPreviousCornerId(cId);
    const CornerId nextCId = GetNextCornerId(cId);

    const VertexId prevVId = GetVertexIdFromCornerId(prevCId);
    const VertexId vId = GetVertexIdFromCornerId(cId);
    const VertexId nextVId = GetVertexIdFromCornerId(nextCId);

    const Vector3 &vPos = GetPositionsPool()[vId];
    const Vector3 &prevVPos = GetPositionsPool()[prevVId];
    const Vector3 &nextVPos = GetPositionsPool()[nextVId];

    const Vector3 prevVector = (prevVPos - vPos).NormalizedSafe();
    const Vector3 nextVector = (nextVPos - vPos).NormalizedSafe();

    const float dot = Vector3::Dot(prevVector, nextVector);
    const float angleRads = Math::ACos(dot);

    return angleRads;
}

Mesh::VertexId Mesh::GetVertexIdUnique(Mesh::VertexId vId) const
{
    ASSERT(vId < m_vertexIdToSamePositionMinimumVertexId.Size());
    return m_vertexIdToSamePositionMinimumVertexId[vId];
}

const Array<Mesh::CornerId> &Mesh::GetCornerIdsFromVertexId(
    Mesh::VertexId vId) const
{
    ASSERT(vId < m_vertexIdToCornerIds.Size());
    return m_vertexIdToCornerIds[vId];
}

Array<Mesh::CornerId> Mesh::GetNeighborCornerIds(Mesh::CornerId cId) const
{
    Set<CornerId> neighborCornerIdsSet;
    VertexId vId = GetVertexIdFromCornerId(cId);
    const Array<CornerId> &sameVertexCornerIds = GetCornerIdsFromVertexId(vId);
    for (CornerId sameVertexCornerId : sameVertexCornerIds)
    {
        neighborCornerIdsSet.Add(GetNextCornerId(sameVertexCornerId));
        neighborCornerIdsSet.Add(GetPreviousCornerId(sameVertexCornerId));
    }

    Array<CornerId> neighborCornerIds = neighborCornerIdsSet.GetKeys();
    return neighborCornerIds;
}

Array<Mesh::VertexId> Mesh::GetNeighborVertexIds(Mesh::VertexId vId) const
{
    USet<VertexId> neighborVertexIds;
    Array<CornerId> vertexCornerIds = GetCornerIdsFromVertexId(vId);
    for (CornerId vertexCId : vertexCornerIds)
    {
        Array<CornerId> neighborCornerIds = GetNeighborCornerIds(vertexCId);
        for (CornerId neighborCornerId : neighborCornerIds)
        {
            neighborVertexIds.Add(GetVertexIdFromCornerId(neighborCornerId));
        }
    }
    return neighborVertexIds.GetKeys();
}

Array<Mesh::VertexId> Mesh::GetNeighborUniqueVertexIds(Mesh::VertexId vId) const
{
    USet<VertexId> neighborUniqueVertexIds;
    Array<VertexId> neighborVertexIds = GetNeighborVertexIds(vId);
    for (VertexId nvid : neighborVertexIds)
    {
        neighborUniqueVertexIds.Add(GetVertexIdUnique(nvid));
    }
    return neighborUniqueVertexIds.GetKeys();
}

Array<Mesh::TriangleId> Mesh::GetAdjacentTriangleIds(
    Mesh::TriangleId triId) const
{
    Array<TriangleId> neighborTriangleIds;

    const auto triCorners = GetCornerIdsFromTriangle(triId);
    for (CornerId cornerId : triCorners)
    {
        CornerId oppositeCornerId = GetOppositeCornerId(cornerId);
        if (oppositeCornerId != SCAST<uint>(-1))
        {
            neighborTriangleIds.PushBack(
                GetTriangleIdFromCornerId(oppositeCornerId));
        }
    }

    return neighborTriangleIds;
}

Array<Mesh::TriangleId> Mesh::GetNeighborTriangleIdsFromVertexId(
    VertexId vId) const
{
    Array<TriangleId> neighborTriangleIds;
    Array<CornerId> cIds = GetCornerIdsFromVertexId(vId);
    for (CornerId cId : cIds)
    {
        neighborTriangleIds.PushBack(GetTriangleIdFromCornerId(cId));
    }
    return neighborTriangleIds;
}

float Mesh::GetVertexGaussianCurvature(Mesh::VertexId centralVId) const
{
    float trisAreasSum = 0.0f;
    float cornerAnglesSum = 0.0f;
    Array<CornerId> vertexCornerIds = GetCornerIdsFromVertexId(centralVId);
    for (CornerId cId : vertexCornerIds)
    {
        float cornerAngle = GetCornerAngleRads(cId);
        float cornerArea =
            GetTriangle(GetTriangleIdFromCornerId(cId)).GetArea();
        cornerAnglesSum += cornerAngle;
        trisAreasSum += cornerArea;
    }

    trisAreasSum /= 3;
    float gaussianCurvature = ((2 * Math::Pi) - cornerAnglesSum) / trisAreasSum;
    return gaussianCurvature;
}

void Mesh::GetNeighborCotangentWeights(
    Mesh::VertexId centralVId,
    Map<Mesh::VertexId, float> *edgesCotangentsScalar,
    Map<Mesh::VertexId, Vector3> *edgesCotangentsVector,
    Map<Mesh::VertexId, float> *triAreas) const
{
    edgesCotangentsScalar->Clear();
    edgesCotangentsVector->Clear();
    triAreas->Clear();

    uint numProcessedTris = 0;
    USet<CornerId> processedCornerIds;
    Mesh::VertexId centralVUniqueId = GetVertexIdUnique(centralVId);
    Vector3 centralVertexPos = GetPositionsPool()[centralVUniqueId];
    Array<CornerId> vertexCornerIds =
        GetCornerIdsFromVertexId(centralVUniqueId);
    for (CornerId centralCId : vertexCornerIds)
    {
        ASSERT(centralVUniqueId == GetVertexIdUniqueFromCornerId(centralCId));

        CornerId firstCId = GetPreviousCornerId(centralCId);
        ASSERT(GetTriangleIdFromCornerId(centralCId) ==
               GetTriangleIdFromCornerId(firstCId));

        if (!processedCornerIds.Contains(centralCId))
        {
            CornerId oppositeCId = GetOppositeCornerId(firstCId);
            if (oppositeCId != SCAST<uint>(-1))
            {
                ASSERT(GetTriangleIdFromCornerId(centralCId) !=
                       GetTriangleIdFromCornerId(oppositeCId));

                ++numProcessedTris;

                float firstCAngle = GetCornerAngleRads(firstCId);
                float oppCAngle = GetCornerAngleRads(oppositeCId);

                TriangleId triId = GetTriangleIdFromCornerId(firstCId);
                VertexId otherVIdOnEdge = GetRemainingVertexIdUnique(
                    triId, centralVId, GetVertexIdFromCornerId(firstCId));
                ASSERT(otherVIdOnEdge !=
                       GetVertexIdUniqueFromCornerId(oppositeCId));
                ASSERT(otherVIdOnEdge !=
                       GetVertexIdUniqueFromCornerId(centralCId));
                ASSERT(otherVIdOnEdge !=
                       GetVertexIdUniqueFromCornerId(firstCId));

                Vector3 otherVertexOnEdgePos =
                    GetPositionsPool()[otherVIdOnEdge];
                float edgeCotScalar = (1.0f / Math::Tan(firstCAngle) +
                                       1.0f / Math::Tan(oppCAngle)) *
                                      0.5f;
                Vector3 edgeCotVector =
                    edgeCotScalar * (centralVertexPos - otherVertexOnEdgePos);

                edgesCotangentsScalar->Add(otherVIdOnEdge, edgeCotScalar);
                edgesCotangentsVector->Add(otherVIdOnEdge, edgeCotVector);
                triAreas->Add(otherVIdOnEdge, GetTriangle(triId).GetArea());

                processedCornerIds.Add(centralCId);
                processedCornerIds.Add(oppositeCId);
            }
        }
    }
}

float Mesh::GetVertexMeanCurvature(Mesh::VertexId centralVId) const
{
    float trisAreasSum = 0.0f;
    Vector3 edgesCotangentsVectorSum = Vector3::Zero();

    Map<VertexId, Vector3> edgesCotangentsVector;
    Map<VertexId, float> edgesCotangentsScalar;
    Map<VertexId, float> triAreas;
    GetNeighborCotangentWeights(
        centralVId, &edgesCotangentsScalar, &edgesCotangentsVector, &triAreas);

    for (const auto &pair : edgesCotangentsVector)
    {
        edgesCotangentsVectorSum += pair.second;
    }
    for (const auto &pair : triAreas)
    {
        trisAreasSum += pair.second;
    }
    trisAreasSum /= 3.0f;

    Vector3 laplaceBeltrami =
        (1.0f / (trisAreasSum)) * edgesCotangentsVectorSum;
    float meanCurvature = 0.5f * laplaceBeltrami.Length();
    return meanCurvature;
}

bool Mesh::IsBoundaryVertex(Mesh::VertexId vId) const
{
    Array<Mesh::CornerId> cIds = GetCornerIdsFromVertexId(vId);
    for (Mesh::CornerId cId : cIds)
    {
        if (GetOppositeCornerId(GetNextCornerId(cId)) == -1u ||
            GetOppositeCornerId(GetPreviousCornerId(cId)) == -1u)
        {
            return true;
        }
    }
    return false;
}

bool Mesh::IsBoundaryEdge(Mesh::VertexId vId0, Mesh::VertexId vId1) const
{
    Mesh::TriangleId triId = GetCommonTriangle(vId0, vId1);
    if (triId != -1u)
    {
        Mesh::CornerId cId0 = GetCornerIdFromTriangleIdAndVertexId(triId, vId0);
        Mesh::CornerId cId1 = GetCornerIdFromTriangleIdAndVertexId(triId, vId1);
        if (cId0 != -1u && cId1 != -1u)
        {
            Mesh::CornerId cIdOther = GetRemainingCornerId(triId, cId0, cId1);
            return (GetOppositeCornerId(cIdOther) == -1u);
        }
    }
    return false;
}

Mesh::TriangleId Mesh::GetCommonTriangle(Mesh::VertexId vId0,
                                         Mesh::VertexId vId1) const
{
    Array<Mesh::TriangleId> triIds0 = GetNeighborTriangleIdsFromVertexId(vId0);
    Array<Mesh::TriangleId> triIds1 = GetNeighborTriangleIdsFromVertexId(vId1);
    for (Mesh::TriangleId triId0 : triIds0)
    {
        for (Mesh::TriangleId triId1 : triIds1)
        {
            if (triId0 == triId1)
            {
                return triId0;
            }
        }
    }
    return SCAST<Mesh::TriangleId>(-1);
}

bool Mesh::HasCornerTablesUpdated() const
{
    return m_cornerIdToOppositeCornerId.Size() >= 1;
}

VAO *Mesh::GetVAO() const
{
    return m_vao;
}
IBO *Mesh::GetTriangleVertexIdsIBO() const
{
    return m_vertexIdsIBO;
}
VBO *Mesh::GetVertexAttributesVBO() const
{
    return m_vertexAttributesVBO;
}
const AABox &Mesh::GetAABBox() const
{
    return m_bBox;
}
const Sphere &Mesh::GetBoundingSphere() const
{
    return m_bSphere;
}
const Map<String, Mesh::Bone> &Mesh::GetBonesPool() const
{
    return m_bonesPool;
}
const Array<Mesh::VertexId> &Mesh::GetTrianglesVertexIds() const
{
    return m_triangleVertexIds;
}
const Array<Vector3> &Mesh::GetPositionsPool() const
{
    return m_positionsPool;
}
const Array<Vector3> &Mesh::GetNormalsPool() const
{
    return m_normalsPool;
}
const Array<Vector2> &Mesh::GetUvsPool() const
{
    return m_uvsPool;
}
const Array<Vector3> &Mesh::GetTangentsPool() const
{
    return m_tangentsPool;
}
const Map<String, uint> &Mesh::GetBonesIds() const
{
    return m_bonesIds;
}

UMap<Mesh::VertexId, Array<Mesh::TriangleId>> Mesh::GetVertexIdsToTriangleIds()
    const
{
    UMap<VertexId, Array<Mesh::TriangleId>> vertexIdsToTriIds;
    for (uint ti = 0; ti < GetNumTriangles(); ++ti)
    {
        std::array<Mesh::VertexId, 3> tiVerticesIds =
            GetVertexIdsFromTriangle(ti);
        for (Mesh::VertexId tivi : tiVerticesIds)
        {
            if (!vertexIdsToTriIds.ContainsKey(tivi))
            {
                vertexIdsToTriIds.Add(tivi, {});
            }
            vertexIdsToTriIds.Get(tivi).PushBack(ti);
        }
    }
    return vertexIdsToTriIds;
}

void Mesh::CloneInto(ICloneable *clone, bool cloneGUID) const
{
    Serializable::CloneInto(clone, cloneGUID);

    Mesh *mClone = SCAST<Mesh *>(clone);

    mClone->m_bBox = m_bBox;
    mClone->m_bSphere = m_bSphere;

    if (mClone->m_vao)
    {
        delete mClone->m_vao;
    }

    mClone->m_vao = new VAO();
    mClone->SetPositionsPool(GetPositionsPool());
    mClone->SetNormalsPool(GetNormalsPool());
    mClone->SetUvsPool(GetUvsPool());
    mClone->SetTangentsPool(GetTangentsPool());
    mClone->SetBonesPool(GetBonesPool());
    mClone->SetTrianglesVertexIds(GetTrianglesVertexIds());
    mClone->SetBonesIds(GetBonesIds());
    mClone->UpdateVAOs();
}

void Mesh::Import(const Path &meshFilepath)
{
    ASSERT_MSG(false,
               "Load the Model, and from there retrieve the mesh. "
               "Or use MeshFactory.");
    BANG_UNUSED(meshFilepath);
}

void Mesh::ImportMeta(const MetaNode &metaNode)
{
    Asset::ImportMeta(metaNode);
}

void Mesh::ExportMeta(MetaNode *metaNode) const
{
    Asset::ExportMeta(metaNode);
}
