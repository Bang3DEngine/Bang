#ifndef MESHSIMPLIFIER_H
#define MESHSIMPLIFIER_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/Mesh.h"
#include "Bang/UMap.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"

namespace Bang
{
template <class>
class ResourceHandle;

class MeshSimplifier
{
public:
    enum class Method
    {
        CLUSTERING,
        QUADRIC_ERROR_METRICS
    };

    static Array<RH<Mesh>> GetAllMeshLODs(const Mesh *mesh,
                                          Method simplificationMethod);

    MeshSimplifier() = delete;

private:
    struct VertexData  // Simple struct to hold vertex data
    {
        Vector3 pos = Vector3::Zero;
        Vector3 normal = Vector3::Zero;
        Vector2 uv = Vector2::Zero;
        Vector3 tangent = Vector3::Zero;
    };
    using VertexCluster = UMap<Mesh::VertexId, VertexData>;

    static VertexData GetVertexRepresentativeForCluster(
        const Mesh &mesh,
        const VertexCluster &vertexCluster,
        const UMap<Mesh::VertexId, Array<Mesh::TriangleId>>
            &vertexIdxsToTriIdxs,
        Method simplificationMethod);
};
}

#endif  // MESHSIMPLIFIER_H
