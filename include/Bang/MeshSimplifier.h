#ifndef MESHSIMPLIFIER_H
#define MESHSIMPLIFIER_H

#include "Bang/Map.h"
#include "Bang/Bang.h"
#include "Bang/Mesh.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

class MeshSimplifier
{
public:
    enum class Method
    {
        Clustering,
        QuadricErrorMetrics
    };

    static Array<RH<Mesh>> GetAllMeshLODs(const Mesh *mesh,
                                          Method simplificationMethod);

    MeshSimplifier() = delete;

private:
    struct VertexData // Simple struct to hold vertex data
    {
        Vector3 pos     = Vector3::Zero;
        Vector3 normal  = Vector3::Zero;
        Vector2 uv      = Vector2::Zero;
        Vector3 tangent = Vector3::Zero;
    };
    using VertexCluster = Map<Mesh::VertexId, VertexData>;

    static VertexData GetVertexRepresentativeForCluster(
        const Mesh &mesh,
        const VertexCluster &vertexCluster,
        const Map<Mesh::VertexId, Array<Mesh::TriangleId>> &vertexIdxsToTriIdxs,
        Method simplificationMethod);
};

NAMESPACE_BANG_END

#endif // MESHSIMPLIFIER_H

