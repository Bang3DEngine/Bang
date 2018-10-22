#ifndef MESH_H
#define MESH_H

#include <array>
#include <functional>

#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/ICloneable.h"
#include "Bang/Map.h"
#include "Bang/Map.tcc"
#include "Bang/Matrix4.tcc"
#include "Bang/MetaNode.h"
#include "Bang/ResourceHandle.h"
#include "Bang/Sphere.h"
#include "Bang/String.h"
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
        Matrix4 rootNodeSpaceToBoneBindSpace;
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

    void UpdateVAOs();
    void UpdateCornerTablesIfNeeded();
    void UpdateVertexNormals();
    void UpdateVAOsAndTables();

    void CalculateLODs();
    int GetNumLODs() const;
    RH<Mesh> GetLODMesh(int lod) const;
    const Array<RH<Mesh>> GetLODMeshes() const;
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
    Triangle GetTriangle(TriangleId triId) const;
    std::array<VertexId, 3> GetVertexIdsFromTriangle(TriangleId triId) const;
    VertexId GetRemainingVertexId(TriangleId triangleId,
                                  VertexId oneVertex,
                                  VertexId anotherVertex) const;
    VertexId GetRemainingVertexIdUnique(TriangleId triangleId,
                                        VertexId oneVertex,
                                        VertexId anotherVertex) const;
    CornerId GetCornerIdFromTriangle(TriangleId triangleId, uint i) const;
    std::array<CornerId, 3> GetCornerIdsFromTriangle(
        TriangleId triangleId) const;
    CornerId GetTriangleIdFromCornerId(CornerId cornerId) const;
    VertexId GetVertexIdFromCornerId(CornerId cId) const;
    VertexId GetVertexIdUniqueFromCornerId(CornerId cId) const;
    VertexId GetVertexIdUnique(VertexId vId) const;
    CornerId GetNextCornerId(CornerId cId) const;
    CornerId GetPreviousCornerId(CornerId cId) const;
    CornerId GetOppositeCornerId(CornerId cId) const;
    float GetCornerAngleRads(CornerId cId) const;
    const Array<CornerId> &GetCornerIdsFromVertexId(VertexId vId) const;
    Array<CornerId> GetNeighborCornerIds(CornerId cId) const;
    Array<VertexId> GetNeighborVertexIds(VertexId vId) const;
    Array<TriangleId> GetAdjacentTriangleIds(TriangleId triId) const;
    Array<TriangleId> GetNeighborTriangleIdsFromVertexId(VertexId vId) const;
    float GetVertexGaussianCurvature(VertexId centralVId);
    float GetVertexMeanCurvature(VertexId centralVId);
    bool HasCornerTablesComputed() const;

    const AABox &GetAABBox() const;
    const Sphere &GetBoundingSphere() const;
    const Array<VertexId> &GetTrianglesVertexIds() const;
    const Array<Vector3> &GetPositionsPool() const;
    const Array<Vector3> &GetNormalsPool() const;
    const Array<Vector2> &GetUvsPool() const;
    const Array<Vector3> &GetTangentsPool() const;
    const Map<String, uint> &GetBonesIds() const;
    const Map<String, Mesh::Bone> &GetBonesPool() const;
    const Path &GetModelFilepath() const;

    UMap<VertexId, Array<TriangleId>> GetVertexIdsToTriangleIds() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Resource
    void Import(const Path &meshFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_areLodsValid = false;
    Array<RH<Mesh>> m_lodMeshes;
    Array<Vector3> m_positionsPool;
    Array<Vector3> m_normalsPool;
    Array<Vector2> m_uvsPool;
    Array<Vector3> m_tangentsPool;
    Array<VertexId> m_triangleVertexIds;

    RH<Animation> m_animations;
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
}

#endif  // MESH_H
