#ifndef MESH_H
#define MESH_H

#include "Bang/UMap.h"
#include "Bang/AABox.h"
#include "Bang/Array.h"
#include "Bang/Asset.h"
#include "Bang/Sphere.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Animation.h"

NAMESPACE_BANG_BEGIN

FORWARD class IBO;
FORWARD class VAO;
FORWARD class VBO;

class Mesh : public Asset,
             public ICloneable
{
    ASSET(Mesh)

public:
    using TriangleId = uint;
    using VertexId = uint;
    struct Bone
    {
        Map<Mesh::VertexId, float> weights;
        Matrix4 rootNodeSpaceToBoneBindSpace;
    };

    static constexpr uint DefaultPositionsVBOLocation = 0;
    static constexpr uint DefaultNormalsVBOLocation   = 1;
    static constexpr uint DefaultUvsVBOLocation       = 2;
    static constexpr uint DefaultTangentsVBOLocation  = 3;
    static constexpr uint DefaultVertexToBonesIndicesVBOLocation = 4;
    static constexpr uint DefaultVertexToBonesWeightsVBOLocation = 5;

    void SetPositionsPool(const Array<Vector3>& positions);
    void SetNormalsPool(const Array<Vector3>& normals);
    void SetUvsPool(const Array<Vector2>& uvs);
    void SetTangentsPool(const Array<Vector3>& tangents);
    void SetBonesPool(const Map<String, Mesh::Bone> &bones);
    void SetVertexIndices(const Array<VertexId>& vertexIndices);
    void SetBonesIndices(const Map<String, uint>& bonesIndices);

    void UpdateVAOs();
    void CalculateVertexNormals();

    void CalculateLODs();
    int GetNumLODs() const;
    RH<Mesh> GetLODMesh(int lod) const;
    const Array<RH<Mesh>> GetLODMeshes() const;
    uint GetNumTriangles() const;
    String GetBoneName(uint boneIndex) const;
    uint GetBoneIndex(const String &boneName) const;
    std::array<VertexId, 3> GetTriangleVertexIndices(int triIndex) const;

    VAO *GetVAO() const;
    IBO *GetVertexIndicesIBO() const;
    VBO *GetVertexAttributesVBO() const;
    int GetNumVertices() const;

    uint GetPositionsBytesSize() const;
    uint GetNormalsBytesSize() const;
    uint GetUvsBytesSize() const;
    uint GetTangentsBytesSize() const;
    uint GetBonesIndicesBytesSize() const;
    uint GetBonesWeightsBytesSize() const;
    uint GetVBOPositionsOffset() const;
    uint GetVBONormalsOffset() const;
    uint GetVBOUvsOffset() const;
    uint GetVBOTangentsOffset() const;
    uint GetVBOBonesIndicesOffset() const;
    uint GetVBOBonesWeightsOffset() const;
    uint GetVBOStride() const;

    const AABox& GetAABBox() const;
    const Sphere& GetBoundingSphere() const;
    const Array<VertexId>& GetVertexIndices() const;
    const Array<Vector3>& GetPositionsPool() const;
    const Array<Vector3>& GetNormalsPool() const;
    const Array<Vector2>& GetUvsPool() const;
    const Array<Vector3>& GetTangentsPool() const;
    const Map<String, uint>& GetBonesIndices() const;
    const Map<String, Mesh::Bone>& GetBonesPool() const;
    const Path &GetModelFilepath() const;

    UMap<VertexId, Array<TriangleId>> GetVertexIndicesToTriangleIndices() const;

    // ICloneable
    virtual void CloneInto(ICloneable *clone) const override;

    // Resource
    void Import(const Path &meshFilepath) override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

private:
    bool m_areLodsValid = false;
    Array< RH<Mesh> > m_lodMeshes;
    Array<VertexId> m_vertexIndices;
    Array<Vector3> m_positionsPool;
    Array<Vector3> m_normalsPool;
    Array<Vector2> m_uvsPool;
    Array<Vector3> m_tangentsPool;

    Map<String, Bone> m_bonesPool;
    Map<String, uint> m_bonesIndices;
    Map<uint, String> m_indexToBone;
    Map<VertexId, std::array<int,   4> > m_vertexIdToImportantBonesIndicesPool;
    Map<VertexId, std::array<float, 4> > m_vertexIdToImportantBonesWeightsPool;
    RH<Animation> m_animations;

    mutable VAO *m_vao = nullptr;
    IBO *m_vertexIndicesIBO = nullptr;
    VBO *m_vertexAttributesVBO = nullptr;

    AABox m_bBox;
    Sphere m_bSphere;

    Mesh();
    virtual ~Mesh();
};

NAMESPACE_BANG_END

#endif // MESH_H
