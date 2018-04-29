#include "Bang/MeshSimplifier.h"

#include <utility>

#include "Bang/Map.h"
#include "Bang/Set.h"
#include "Bang/Mesh.h"
#include "Bang/Debug.h"
#include "Bang/Plane.h"
#include "Bang/Octree.h"
#include "Bang/Triangle.h"
#include "Bang/Resources.h"
#include "Bang/DebugRenderer.h"

USING_NAMESPACE_BANG

using OctreeData = std::pair<Mesh::VertexId, Vector3>;

struct ClassifyPoints
{
    bool operator()(const AABox &aaBox, const OctreeData &data)
    {
        return aaBox.Contains(data.second);
    }
};

Array<RH<Mesh>> MeshSimplifier::GetAllMeshLODs(const Mesh *mesh,
                                               Method simplificationMethod)
{
    if (!mesh) { return Array<RH<Mesh>>(); }

    Array<OctreeData> octreeData; // Retrieve all the octree data
    {
        for (int i = 0; i < mesh->GetVertexCount(); ++i)
        {
            const int vIndex = (mesh->GetVertexIndicesIBO() ?
                                        mesh->GetVertexIndices()[i] : i);
            const Vector3 &position = mesh->GetPositionsPool()[vIndex];
            octreeData.PushBack( std::make_pair(vIndex, position) );
        }
    }

    using SimplOctree = Octree<OctreeData, ClassifyPoints>;

    constexpr int MaxOctreeDepth = 12;
    constexpr float PaddingPercent = 0.05f;
    SimplOctree octree;
    AABox meshAABox = mesh->GetAABBox();
    octree.SetAABox( AABox(meshAABox.GetMin() - meshAABox.GetSize() * PaddingPercent,
                           meshAABox.GetMax() + meshAABox.GetSize() * PaddingPercent) );
    octree.Fill(octreeData, MaxOctreeDepth);

    // Compute useful connectivity info for later
    using VertexIdPair = std::pair<Mesh::VertexId, Mesh::VertexId>;
    Map<Mesh::VertexId, Set<VertexIdPair> >
                vertexIndexToTriangleOtherVerticesIndices;
    for (uint tri = 0; tri < mesh->GetNumTriangles(); ++tri)
    {
        const std::array<Mesh::VertexId, 3> triVertexIndices =
                                mesh->GetTriangleVertexIndices(tri);
        for (uint i = 0; i < 3; ++i)
        {
            const Mesh::VertexId currentVertexIndex =
                                        triVertexIndices[ ((i + 0) % 3) ];
            const Mesh::VertexId otherVertexIndex0  =
                                        triVertexIndices[ ((i + 1) % 3) ];
            const Mesh::VertexId otherVertexIndex1  =
                                        triVertexIndices[ ((i + 2) % 3) ];

            // We use minmax to have ordered pair
            // (so that order does not matter in set)
            const VertexIdPair otherTriVertexIndices =
                    std::minmax(otherVertexIndex0, otherVertexIndex1);

            vertexIndexToTriangleOtherVerticesIndices[currentVertexIndex].
                    Add(otherTriVertexIndices);
        }
    }

    Map<Mesh::VertexId, Array<Mesh::TriangleId>> vertexIdxsToTriIdxs;
    if (simplificationMethod == Method::QuadricErrorMetrics)
    {
        vertexIdxsToTriIdxs = mesh->GetVertexIndicesToTriangleIndices();
    }

    // For each level of detail
    Array< RH<Mesh> > simplifiedMeshesArray;
    for (int level = 0; level <= MaxOctreeDepth; ++level)
    {
        // Get the octree nodes at that level (and leaves pruned before)
        Array<const SimplOctree*> octreeChildrenInLevel =
                                      octree.GetChildrenAtLevel(level, true);

        // Get vertex clusters from octree in this level (vertex clusters are
        // just the collection of vertices at each octree node)
        using ClusterId = uint;
        Array<VertexCluster> vertexClusters;
        Map<Mesh::VertexId, ClusterId> vertexIndexToClusterIndex;

        // Make clusters for each octree node in this level...
        for (const SimplOctree *octInLevel : octreeChildrenInLevel)
        {
            const Array<OctreeData> octLevelData = octInLevel->GetElementsRecursive();

            // We will treat here a very common corner case when arriving to
            // maximum subdivision levels. It can happen that we have all these
            // clustered vertices with the same pos, but different normals/uvs.
            // If we clustered them, we would later average them, and not
            // thus not respecting the original model discontinuity.
            // Consequently, in this case, we must just not cluster them.
            bool allVerticesHaveSamePosition = true;
            {
                const Vector3 &firstPos = octLevelData.Begin()->second;
                for (const OctreeData &octData : octLevelData)
                {
                    const Vector3 &octDataPos = octData.second;
                    if (Vector3::SqDistance(firstPos, octDataPos) > 10e-5)
                    {
                        allVerticesHaveSamePosition = false;
                        break;
                    }
                }
            }
            bool splitAllVerticesIntoClusters = allVerticesHaveSamePosition;

            // if (!allVerticesHaveSamePosition)
            {
                // Normal case.
                // Create the vertex cluster from the octree node.
                // Preserving shape algorithm here also.
                // Split also vertices whose normal lays on a different axis quadrants
                // of the 8 possible.
                // So, we can have up to 8 subclusters here.

                for (int i = 0; i < 1; ++i)
                {
                    Vector3 nDir;
                    switch (i)
                    {
                        case 0: nDir = Vector3(-1,-1,-1); break;
                        case 1: nDir = Vector3(-1,-1, 1); break;
                        case 2: nDir = Vector3(-1, 1,-1); break;
                        case 3: nDir = Vector3(-1, 1, 1); break;
                        case 4: nDir = Vector3( 1,-1,-1); break;
                        case 5: nDir = Vector3( 1,-1, 1); break;
                        case 6: nDir = Vector3( 1, 1,-1); break;
                        case 7: nDir = Vector3( 1, 1, 1); break;
                    }
                    // nDir is the direction the normals of the vertices must be
                    // aligned to form part of this cluster

                    VertexCluster vertexCluster;
                    for (const OctreeData &octData : octLevelData)
                    {
                        const Mesh::VertexId vertexIndex = octData.first;
                        if (!vertexCluster.ContainsKey(vertexIndex))
                        {
                            Vector3 vNormal = mesh->GetNormalsPool()[vertexIndex];
                            if ( (Math::Sign(vNormal.x) == Math::Sign(nDir.x)) &&
                                 (Math::Sign(vNormal.y) == Math::Sign(nDir.y)) &&
                                 (Math::Sign(vNormal.z) == Math::Sign(nDir.z)))
                            {
                                VertexData vData;
                                vData.pos     = mesh->GetPositionsPool()[vertexIndex];
                                vData.normal  = vNormal;
                                vData.uv      = mesh->GetUvsPool()[vertexIndex];
                                if (vertexIndex < mesh->GetTangentsPool().Size())
                                {
                                    vData.tangent = mesh->GetTangentsPool()[vertexIndex];
                                }

                                vertexIndexToClusterIndex[vertexIndex] = vertexClusters.Size();

                                if (splitAllVerticesIntoClusters)
                                {
                                    VertexCluster singleVertexCluster;
                                    singleVertexCluster.Add(vertexIndex, vData);
                                    vertexClusters.PushBack(singleVertexCluster);
                                }
                                else
                                {
                                    vertexCluster.Add(vertexIndex, vData);
                                }
                            }
                        }
                    }
                    if (!splitAllVerticesIntoClusters)
                    {
                        vertexClusters.PushBack(vertexCluster);
                    }
                }
            }
        }

        // Now actually simplify the mesh. For each cluster of vertices we will
        // extract one single vertex averaging all the components of the cluster.
        Array<Vector3> positionsLOD;
        Array<Vector3> normalsLOD;
        Array<Vector2> uvsLOD;
        Array<Vector3> tangentsLOD;
        RH<Mesh> simplifiedMesh = Resources::Create<Mesh>();
        for (const VertexCluster &vertexCluster : vertexClusters)
        {
            if (vertexCluster.IsEmpty()) { continue; }

            VertexData vertexRepresentativeData =
               MeshSimplifier::GetVertexRepresentativeForCluster(
                                    *mesh,
                                    vertexCluster,
                                    vertexIdxsToTriIdxs,
                                    simplificationMethod);
            positionsLOD.PushBack(vertexRepresentativeData.pos);
            normalsLOD.PushBack(vertexRepresentativeData.normal);
            uvsLOD.PushBack(vertexRepresentativeData.uv);
            tangentsLOD.PushBack(vertexRepresentativeData.tangent);
        }
        simplifiedMesh.Get()->LoadPositionsPool(positionsLOD);
        simplifiedMesh.Get()->LoadNormalsPool(normalsLOD);
        simplifiedMesh.Get()->LoadUvsPool(uvsLOD);
        simplifiedMesh.Get()->LoadTangentsPool(tangentsLOD);

        // All is left is determining the face vertex indices...For this we will
        // determine which combinations of 3 vertex clusters form up triangles.
        // We know that 3 vertex clusters form a triangle if there exists a
        // triangle of the original mesh formed by one vertex of each of the 3
        // vertex clusters.

        // First: for each vertex index, know inside which vertex cluster its
        //        contained.
        // We have stored this in a variable before.

        // Second: for each vertex index, know which other two vertices form up
        //         a triangle with it. We have computed this before too

        // Third:  for each vertex cluster, create a set of the pair of clusters
        //         it forms a triangle with. We can do this with the prev info.
        using ClusterIdPair = std::pair<ClusterId, ClusterId>;
        Map<ClusterId, Set<ClusterIdPair> >
                clusterIdToOtherClusterIdsThatFormATriangleWithIt;
        auto &m = clusterIdToOtherClusterIdsThatFormATriangleWithIt;

        // For each cluster
        for (ClusterId cId = 0; cId < vertexClusters.Size(); ++cId)
        {
            // For each vertex in each vertex cluster
            const VertexCluster &vertexCluster = vertexClusters[cId];
            for (const auto &pair : vertexCluster)
            {
                Mesh::VertexId vertexInClusterIndex = pair.first;
                const ClusterId vCId =
                        vertexIndexToClusterIndex[vertexInClusterIndex];
                const Set<VertexIdPair> &otherVertexIndicesThatFormATri =
                        vertexIndexToTriangleOtherVerticesIndices.
                            Get(vertexInClusterIndex);

                // For each vertex pair that forms a tri with each vertex in each
                // vertex cluster
                for (const VertexIdPair &otherTriVertexIndices :
                                            otherVertexIndicesThatFormATri)
                {
                    // We use minmax to have ordered pair
                    // (so that order does not matter in set)
                    const VertexIdPair otherVertexIndicesThatFormATri =
                                std::minmax(otherTriVertexIndices.first,
                                            otherTriVertexIndices.second);
                    const VertexIdPair &vp = otherVertexIndicesThatFormATri;
                    const ClusterIdPair otherClusterIndicesThatFormATri =
                        std::minmax(vertexIndexToClusterIndex[vp.first],
                                    vertexIndexToClusterIndex[vp.second]);
                    m[vCId].Add(otherClusterIndicesThatFormATri);
                }
            }
        }

        // Finally calculate vertexIndices with all previous info!
        Array<Mesh::VertexId> vertexClusterIndices;
        for (ClusterId cId = 0; cId < vertexClusters.Size(); ++cId)
        {
            const Set<ClusterIdPair> &otherClusterIdsThatFormATriWithThis =
                    clusterIdToOtherClusterIdsThatFormATriangleWithIt[cId];
            for (const ClusterIdPair &otherTriVerticesClusterIdPair :
                                   otherClusterIdsThatFormATriWithThis)
            {
                const ClusterId otherCId0 = otherTriVerticesClusterIdPair.first;
                const ClusterId otherCId1 = otherTriVerticesClusterIdPair.second;
                // This if is to avoid triplicates vvv
                if (cId < otherCId0 && (otherCId0 < otherCId1) && cId < otherCId1)
                {
                    // ASSERT(cId       < simplifiedMesh.Get()->GetPositionsPool().Size());
                    // ASSERT(otherCId0 < simplifiedMesh.Get()->GetPositionsPool().Size());
                    // ASSERT(otherCId1 < simplifiedMesh.Get()->GetPositionsPool().Size());
                    // Debug_Peek(vertexClusters.Size());
                    // Debug_Peek(simplifiedMesh.Get()->GetPositionsPool().Size());

                    // Decide triangle winding based on its normal
                    std::array<Mesh::VertexId, 3> triClusterIds =
                                        {{cId, otherCId0, otherCId1}};
                    const Array<Vector3> &clusterPositions =
                                      simplifiedMesh.Get()->GetPositionsPool();
                    const Array<Vector3> &clusterNormals =
                                      simplifiedMesh.Get()->GetNormalsPool();
                    const Vector3 &pos0   = clusterPositions[ triClusterIds[0] ];
                    const Vector3 &pos1   = clusterPositions[ triClusterIds[1] ];
                    const Vector3 &pos2   = clusterPositions[ triClusterIds[2] ];
                    const Vector3 &norm0  = clusterNormals[ triClusterIds[0] ];
                    const Vector3 &norm1  = clusterNormals[ triClusterIds[1] ];
                    const Vector3 &norm2  = clusterNormals[ triClusterIds[2] ];
                    const Vector3 &normal = (norm0 + norm1 + norm2) / 3.0f;
                    if (Vector3::Dot(
                            Vector3::Cross(pos1-pos0, pos2-pos0), normal) < 0)
                    {
                        std::swap(triClusterIds[1], triClusterIds[2]);
                    }

                    // Add triangle indices
                    vertexClusterIndices.PushBack(triClusterIds[0]);
                    vertexClusterIndices.PushBack(triClusterIds[1]);
                    vertexClusterIndices.PushBack(triClusterIds[2]);
                }
            }
        }
        simplifiedMesh.Get()->LoadVertexIndices(vertexClusterIndices);
        Debug_Log("Level " << level << ": " << vertexClusterIndices.Size() <<
                  "/" << mesh->GetVertexCount());

        simplifiedMeshesArray.PushBack(simplifiedMesh);

        if (vertexClusterIndices.Size() == mesh->GetVertexCount())
        {
            // This was the max level, going further makes no sense
            break;
        }
    }
    return simplifiedMeshesArray;
}

MeshSimplifier::VertexData MeshSimplifier::GetVertexRepresentativeForCluster(
    const Mesh &mesh,
    const VertexCluster &vertexCluster,
    const Map<Mesh::VertexId, Array<Mesh::TriangleId>> &vertexIdxsToTriIdxs,
    Method simplificationMethod)
{
    ASSERT(!vertexCluster.IsEmpty());

    VertexData vertexRepresentativeData;
    switch (simplificationMethod)
    {
        case Method::Clustering:
        {
        vertexRepresentativeData.pos     = Vector3::Zero;
        vertexRepresentativeData.normal  = Vector3::Zero;
        vertexRepresentativeData.uv      = Vector2::Zero;
        vertexRepresentativeData.tangent = Vector3::Zero;
        for (const auto &pair : vertexCluster)
        {
            const VertexData &vData = pair.second;
            vertexRepresentativeData.pos     += vData.pos;
            vertexRepresentativeData.normal  += vData.normal;
            vertexRepresentativeData.uv      += vData.uv;
            vertexRepresentativeData.tangent += vData.tangent;
        }

        const float vertexClusterSize = vertexCluster.Size();
        vertexRepresentativeData.pos     /= vertexClusterSize;
        vertexRepresentativeData.normal  /= vertexClusterSize;
        vertexRepresentativeData.uv      /= vertexClusterSize;
        vertexRepresentativeData.tangent /= vertexClusterSize;
        }
        break;

        case Method::QuadricErrorMetrics:
        {
        // To get the position use quadric error metrics
        Matrix4 verticesTotalQuadricMatrix = Matrix4(0.0f);
        for (const auto &pair : vertexCluster)
        {
            const Mesh::VertexId &vId = pair.first;
            const VertexData &vData = pair.second;
            //*
            Vector3 n = vData.normal.NormalizedSafe();
            Vector4 planeVector = Vector4(n.x, n.y, n.z,
                                          Vector3::Dot(-n, vData.pos));
            const Vector4 &pv = planeVector;
            const float a = pv.x, b = pv.y, c = pv.z, d = pv.w;
            const Matrix4 vertexQuadricMatrix =
                    Matrix4(a*a, a*b, a*c, a*d,
                            b*a, b*b, b*c, b*d,
                            c*a, c*b, c*c, c*d,
                            d*a, d*b, d*c, d*d);
            verticesTotalQuadricMatrix += vertexQuadricMatrix;
            /*/
            const Array<Mesh::TriangleId> &neighborTriIds =
                                                vertexIdxsToTriIdxs.Get(vId);
            for (Mesh::TriangleId triId : neighborTriIds)
            {
                const std::array<Mesh::VertexId, 3> triVertsIds =
                                        mesh.GetTriangleVertexIndices(triId);
                const Mesh::VertexId triVId0 = triVertsIds[0];
                const Mesh::VertexId triVId1 = triVertsIds[1];
                const Mesh::VertexId triVId2 = triVertsIds[2];
                const Vector3 &triP0 = mesh.GetPositionsPool()[triVId0];
                const Vector3 &triP1 = mesh.GetPositionsPool()[triVId1];
                const Vector3 &triP2 = mesh.GetPositionsPool()[triVId2];
                const Triangle neighborTri = Triangle(triP0, triP1, triP2);
                const Plane neighborTriPlane = neighborTri.GetPlane();
                const Vector3 &n = neighborTriPlane.GetNormal();
                const float a = n.x;
                const float b = n.y;
                const float c = n.z;
                const float d = -(a*triP0.x + b*triP0.y + c*triP0.z);
                const Matrix4 vertexQuadricMatrix =
                        Matrix4(a*a, a*b, a*c, a*d,
                                b*a, b*b, b*c, b*d,
                                c*a, c*b, c*c, c*d,
                                d*a, d*b, d*c, d*d);
                verticesTotalQuadricMatrix += vertexQuadricMatrix;
            }
            //*/
        }
        verticesTotalQuadricMatrix[0][3] = 0.0f;
        verticesTotalQuadricMatrix[1][3] = 0.0f;
        verticesTotalQuadricMatrix[2][3] = 0.0f;
        verticesTotalQuadricMatrix[3][3] = 1.0f;
        bool isInvertible;
        Vector3 minimumQuadricErrorPosition =
                (verticesTotalQuadricMatrix.Inversed(0.1f, &isInvertible) *
                 Vector4(0,0,0,1)).xyz();

        // To get normal, uvs, etc. use the clustering method.
        vertexRepresentativeData =
                GetVertexRepresentativeForCluster(mesh, vertexCluster,
                                                  vertexIdxsToTriIdxs,
                                                  Method::Clustering);
        if (isInvertible)
        {
            static int invertibles = 0;
            // Debug_Peek(++invertibles);
            // Debug_Peek(verticesTotalQuadricMatrix);
            vertexRepresentativeData.pos = minimumQuadricErrorPosition;
        }

        }
        break;
    }

    return vertexRepresentativeData;
}
