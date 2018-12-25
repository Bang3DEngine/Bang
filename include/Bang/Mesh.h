#ifndef MESH_H
#define MESH_H

#include <array>
#include <functional>

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/ICloneable.h"
#include "Bang/Map.h"
#include "Bang/Map.tcc"
#include "Bang/Matrix4.tcc"
#include "Bang/MetaNode.h"
#include "Bang/Ray.h"
#include "Bang/Sphere.h"
#include "Bang/String.h"
#include "Bang/Texture2D.h"
#include "Bang/Transformation.h"
#include "Bang/Triangle.h"
#include "Bang/UMap.h"

namespace Bang
{
class Animation;
class IBO;
class Path;
class VAO;
class VBO;

class Mesh : public Asset
{
    ASSET(Mesh)

public:
    using TriangleId = uint;
    using CornerId = uint;
    using VertexId = uint;
    struct Bone
    {
        Map<Mesh::VertexId, float> weights;
        Transformation rootSpaceToBoneBindSpaceTransformation;
    };

    static constexpr uint DefaultPositionsVBOLocation = 0;
    static constexpr uint DefaultNormalsVBOLocation = 1;
    static constexpr uint DefaultUvsVBOLocation = 2;
    static constexpr uint DefaultTangentsVBOLocation = 3;
    static constexpr uint DefaultVertexToBonesIdsVBOLocation = 4;
    static constexpr uint DefaultVertexToBonesWeightsVBOLocation = 5;

    void SetPositionsPool(const Array<Vector3> &positions);
    void SetNormalsPool(const Array<Vector3> &normals);
    void SetUvsPool(const Array<Vector2> &uvs);
    void SetTangentsPool(const Array<Vector3> &tangents);
    void SetBonesPool(const Map<String, Mesh::Bone> &bones);
    void SetTrianglesVertexIds(const Array<VertexId> &triangleVertexIds);
    void SetBonesIds(const Map<String, uint> &bonesIds);
    void SetPosition(Mesh::VertexId vId, const Vector3 &pos);

    void UpdateVAOs(bool createIndicesIfNeeded = true);
    void UpdateCornerTablesIfNeeded();
    void UpdateVertexNormals();
    void UpdateVAOsAndTables();

    void CalculateLODs();
    int GetNumLODs() const;
    AH<Mesh> GetLODMesh(int lod) const;
    const Array<AH<Mesh>> GetLODMeshes() const;
    String GetBoneName(uint boneId) const;
    uint GetBoneId(const String &boneName) const;

    VAO *GetVAO() const;
    IBO *GetTriangleVertexIdsIBO() const;
    VBO *GetVertexAttributesVBO() const;

    uint GetPositionsBytesSize() const;
    uint GetNormalsBytesSize() const;
    uint GetUvsBytesSize() const;
    uint GetTangentsBytesSize() const;
    uint GetBonesIdsBytesSize() const;
    uint GetBonesWeightsBytesSize() const;
    uint GetVBOPositionsOffset() const;
    uint GetVBONormalsOffset() const;
    uint GetVBOUvsOffset() const;
    uint GetVBOTangentsOffset() const;
    uint GetVBOBonesIdsOffset() const;
    uint GetVBOBonesWeightsOffset() const;
    uint GetVBOStride() const;

    // Triangles related
    bool IsIndexed() const;
    uint GetNumCorners() const;
    uint GetNumVertices() const;
    uint GetNumTriangles() const;
    uint GetNumVerticesIds() const;
    Triangle GetTriangle(Mesh::TriangleId triId) const;
    std::array<Mesh::VertexId, 3> GetVertexIdsFromTriangle(
        Mesh::TriangleId triId) const;
    VertexId GetRemainingVertexId(Mesh::TriangleId triangleId,
                                  Mesh::VertexId oneVertex,
                                  Mesh::VertexId anotherVertex) const;
    Vector2 GetTriangleUvsAtPoint(Mesh::TriangleId triangleId,
                                  const Vector3 &point) const;
    Mesh::VertexId GetRemainingVertexIdUnique(
        Mesh::TriangleId triangleId,
        Mesh::VertexId oneVertex,
        Mesh::VertexId anotherVertex) const;
    Mesh::CornerId GetRemainingCornerId(Mesh::TriangleId triangleId,
                                        Mesh::CornerId oneCorner,
                                        Mesh::CornerId anotherCorner) const;
    Mesh::CornerId GetCornerIdFromTriangle(Mesh::TriangleId triangleId,
                                           uint i) const;
    Mesh::CornerId GetCornerIdFromTriangleIdAndVertexId(
        Mesh::TriangleId triangleId,
        Mesh::VertexId vertexId) const;
    std::array<CornerId, 3> GetCornerIdsFromTriangle(
        TriangleId triangleId) const;
    Mesh::CornerId GetTriangleIdFromCornerId(Mesh::CornerId cornerId) const;
    Mesh::VertexId GetVertexIdFromCornerId(Mesh::CornerId cId) const;
    Mesh::VertexId GetVertexIdUniqueFromCornerId(Mesh::CornerId cId) const;
    Mesh::VertexId GetVertexIdUnique(Mesh::VertexId vId) const;
    Mesh::CornerId GetNextCornerId(Mesh::CornerId cId) const;
    Mesh::CornerId GetPreviousCornerId(Mesh::CornerId cId) const;
    Mesh::CornerId GetOppositeCornerId(Mesh::CornerId cId) const;
    float GetCornerAngleRads(Mesh::CornerId cId) const;
    const Array<Mesh::CornerId> &GetCornerIdsFromVertexId(
        Mesh::VertexId vId) const;
    Array<Mesh::CornerId> GetNeighborCornerIds(Mesh::CornerId cId) const;
    Array<Mesh::VertexId> GetNeighborVertexIds(Mesh::VertexId vId) const;
    Array<Mesh::VertexId> GetNeighborUniqueVertexIds(Mesh::VertexId vId) const;
    Array<Mesh::TriangleId> GetAdjacentTriangleIds(
        Mesh::TriangleId triId) const;
    Array<Mesh::TriangleId> GetNeighborTriangleIdsFromVertexId(
        Mesh::VertexId vId) const;
    float GetVertexGaussianCurvature(Mesh::VertexId centralVId) const;
    float GetVertexMeanCurvature(Mesh::VertexId centralVId) const;
    void GetNeighborCotangentWeights(
        Mesh::VertexId centralVId,
        Map<Mesh::VertexId, float> *edgesCotangentsScalar,
        Map<Mesh::VertexId, Vector3> *edgesCotangentsVector,
        Map<Mesh::VertexId, float> *triAreas) const;
    bool IsBoundaryVertex(Mesh::VertexId vId) const;
    bool IsBoundaryEdge(Mesh::VertexId vId0, Mesh::VertexId vId1) const;
    Mesh::TriangleId GetCommonTriangle(Mesh::VertexId vId0,
                                       Mesh::VertexId vId1) const;
    bool HasCornerTablesUpdated() const;

    const AABox &GetAABBox() const;
    const Sphere &GetBoundingSphere() const;
    const Array<Mesh::VertexId> &GetTrianglesVertexIds() const;
    const Array<Vector3> &GetPositionsPool() const;
    const Array<Vector3> &GetNormalsPool() const;
    const Array<Vector2> &GetUvsPool() const;
    const Array<Vector3> &GetTangentsPool() const;
    const Map<String, uint> &GetBonesIds() const;
    const Map<String, Mesh::Bone> &GetBonesPool() const;
    const Path &GetModelFilepath() const;

    UMap<Mesh::VertexId, Array<Mesh::TriangleId>> GetVertexIdsToTriangleIds()
        const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone, bool cloneGUID) const override;

    // Asset
    void Import(const Path &meshFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_areLodsValid = false;
    Array<AH<Mesh>> m_lodMeshes;
    Array<Vector3> m_positionsPool;
    Array<Vector3> m_normalsPool;
    Array<Vector2> m_uvsPool;
    Array<Vector3> m_tangentsPool;
    Array<VertexId> m_triangleVertexIds;

    AH<Animation> m_animations;
    Map<String, Bone> m_bonesPool;
    Map<uint, String> m_idToBone;
    Map<String, uint> m_bonesIds;
    Map<VertexId, std::array<int, 4>> m_vertexIdToImportantBonesIdsPool;
    Map<VertexId, std::array<float, 4>> m_vertexIdToImportantBonesWeightsPool;

    // (i, j, k) hold the opposite corners of the corners (0, 1, 2) of the
    // triangle 3*(i/3)
    bool m_areCornerTablesValid = false;
    Array<CornerId> m_cornerIdToOppositeCornerId;
    Array<Array<CornerId>> m_vertexIdToCornerIds;
    Array<VertexId> m_vertexIdToSamePositionMinimumVertexId;

    mutable VAO *m_vao = nullptr;
    IBO *m_vertexIdsIBO = nullptr;
    VBO *m_vertexAttributesVBO = nullptr;

    AABox m_bBox;
    Sphere m_bSphere;

    Mesh();
    virtual ~Mesh() override;
};
}  // namespace Bang

#endif  // MESH_H
