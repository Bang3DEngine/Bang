#include "Bang/MeshSimplifier.h"

#include <algorithm>
#include <array>
#include <functional>
#include <ostream>
#include <utility>

#include "Bang/AABox.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/AssetHandle.h"
#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Debug.h"
#include "Bang/Math.h"
#include "Bang/Matrix4.h"
#include "Bang/Matrix4.tcc"
#include "Bang/Mesh.h"
#include "Bang/Octree.h"
#include "Bang/Octree.tcc"
#include "Bang/Plane.h"
#include "Bang/Set.h"
#include "Bang/Set.tcc"
#include "Bang/Triangle.h"
#include "Bang/UMap.tcc"
#include "Bang/Vector4.h"

using namespace Bang;

using OctreeData = std::pair<Mesh::VertexId, Vector3>;

struct ClassifyPoints
{
    bool operator()(const AABox &aaBox, const OctreeData &data)
    {
        const Vector3 &pos = data.second;
        return pos > aaBox.GetMin() && pos <= aaBox.GetMax();
    }
};

void MeshSimplifier::ApplySmoothIteration(Mesh *mesh,
                                          SmoothMethod smoothMethod,
                                          float smoothFactor,
                                          uint steps)
{
    mesh->UpdateCornerTablesIfNeeded();

    float nextSmoothFactor = smoothFactor;
    for (uint i = 0; i < steps; ++i)
    {
        const Array<Vector3> prevPositions = mesh->GetPositionsPool();
        Array<Vector3> newPositions = Array<Vector3>(prevPositions.Size());
        for (Mesh::VertexId vId = 0; vId < mesh->GetPositionsPool().Size();
             ++vId)
        {
            Vector3 vertexPos = prevPositions[vId];
            Vector3 vDisplacement = Vector3::Zero();

            const Array<Mesh::VertexId> neighborVIds =
                mesh->GetNeighborVertexIds(vId);
            const uint numNeighbors = neighborVIds.Size();

            for (Mesh::VertexId nVId : neighborVIds)
            {
                Vector3 neighborVertexPos = prevPositions[nVId];
                vDisplacement += (neighborVertexPos - vertexPos);
            }
            vDisplacement /= SCAST<float>(numNeighbors);

            vertexPos += vDisplacement * nextSmoothFactor;
            newPositions[vId] = vertexPos;
        }

        mesh->SetPositionsPool(newPositions);

        if (smoothMethod == SmoothMethod::TAUBIN)
        {
            nextSmoothFactor *= -1.0f;
        }

        if (smoothMethod == SmoothMethod::BILAPLACE)
        {
            nextSmoothFactor *= -1.0f;
            float sign = Math::Sign(nextSmoothFactor);
            nextSmoothFactor = smoothFactor * sign * (sign > 0 ? 1.5f : 1.0f);
        }
    }

    mesh->UpdateVAOs();
}

Array<AH<Mesh>> MeshSimplifier::GetAllMeshLODs(
    const Mesh *mesh,
    SimplificationMethod simplificationMethod)
{
    if (!mesh)
    {
        return Array<AH<Mesh>>();
    }

    Array<OctreeData> octreeData;  // Retrieve all the octree data
    {
        for (uint i = 0; i < mesh->GetNumVerticesIds(); ++i)
        {
            const int vIndex = (mesh->GetTriangleVertexIdsIBO()
                                    ? mesh->GetTrianglesVertexIds()[i]
                                    : i);
            const Vector3 &position = mesh->GetPositionsPool()[vIndex];
            octreeData.PushBack(std::make_pair(vIndex, position));
        }
    }

    using SimplOctree = Octree<OctreeData, ClassifyPoints>;

    constexpr int MaxOctreeDepth = 12;
    constexpr float PaddingPercent = 0.1f;
    SimplOctree octree;
    AABox meshAABox = mesh->GetAABBox();
    octree.SetAABox(
        AABox(meshAABox.GetMin() - meshAABox.GetSize() * PaddingPercent,
              meshAABox.GetMax() + meshAABox.GetSize() * PaddingPercent));
    octree.Fill(octreeData, MaxOctreeDepth);

    // Compute useful connectivity info for later
    using VertexIdPair = std::pair<Mesh::VertexId, Mesh::VertexId>;
    UMap<Mesh::VertexId, Set<VertexIdPair>>
        vertexIndexToTriangleOtherVerticesIndices;
    for (uint tri = 0; tri < mesh->GetNumTriangles(); ++tri)
    {
        const std::array<Mesh::VertexId, 3> triVertexIndices =
            mesh->GetVertexIdsFromTriangle(tri);
        for (uint i = 0; i < 3; ++i)
        {
            const Mesh::VertexId currentVertexIndex =
                triVertexIndices[((i + 0) % 3)];
            const Mesh::VertexId otherVertexIndex0 =
                triVertexIndices[((i + 1) % 3)];
            const Mesh::VertexId otherVertexIndex1 =
                triVertexIndices[((i + 2) % 3)];
            // We use minmax to have ordered pair (so that order does not matter
            // in set)
            const VertexIdPair otherTriVertexIndices =
                std::minmax(otherVertexIndex0, otherVertexIndex1);

            if (!vertexIndexToTriangleOtherVerticesIndices.ContainsKey(
                    currentVertexIndex))
            {
                vertexIndexToTriangleOtherVerticesIndices[currentVertexIndex] =
                    Set<VertexIdPair>();
            }
            vertexIndexToTriangleOtherVerticesIndices[currentVertexIndex].Add(
                otherTriVertexIndices);
        }
    }

    UMap<Mesh::VertexId, Array<Mesh::TriangleId>> vertexIdxsToTriIdxs;
    if (simplificationMethod == SimplificationMethod::QUADRIC_ERROR_METRICS)
    {
        vertexIdxsToTriIdxs = mesh->GetVertexIdsToTriangleIds();
    }

    int octreeDepth = octree.GetDepth();
    int numLevelsToGenerate = Math::Min(octreeDepth, 6);
    Array<int> levelsToGenerate;
    for (int i = 0; i < numLevelsToGenerate; ++i)
    {
        levelsToGenerate.PushBack((octreeDepth / numLevelsToGenerate) * i);
    }

    // For each level of detail
    Array<AH<Mesh>> simplifiedMeshesArray;
    for (int level : levelsToGenerate)
    {
        // Get the octree nodes at that level (and leaves pruned before)
        Array<const SimplOctree *> octreeNodesInLevel =
            // octree.GetChildrenAtLevel(level, true);
            octree.GetChildrenAtLevel(level, false);

        // Get vertex clusters from octree in this level (vertex clusters are
        // just the collection of vertices at each octree node)
        using ClusterId = uint;
        Array<VertexCluster> vertexClusters;
        UMap<Mesh::VertexId, ClusterId> vertexIndexToClusterIndex;

        // Make clusters for each octree node in this level...
        for (const SimplOctree *octNodeInLevel : octreeNodesInLevel)
        {
            const Array<OctreeData> octNodeLevelData =
                octNodeInLevel->GetElementsRecursive();

            // Create the vertex cluster from the octree node.
            // Preserving shape algorithm here also.
            // Split also vertices whose normal lays on a different axis
            // quadrants of the 8 possible.
            // So, we can have up to 8 subclusters here.
            const bool shapePreserving = true;
            for (int i = 0; i < (shapePreserving ? 8 : 1); ++i)
            {
                Vector3 nDir;
                switch (i)
                {
                    case 0: nDir = Vector3(-1, -1, -1); break;
                    case 1: nDir = Vector3(-1, -1, 1); break;
                    case 2: nDir = Vector3(-1, 1, -1); break;
                    case 3: nDir = Vector3(-1, 1, 1); break;
                    case 4: nDir = Vector3(1, -1, -1); break;
                    case 5: nDir = Vector3(1, -1, 1); break;
                    case 6: nDir = Vector3(1, 1, -1); break;
                    case 7: nDir = Vector3(1, 1, 1); break;
                }
                // nDir is the direction the normals of the vertices must be
                // aligned to form part of this cluster

                VertexCluster vertexCluster;
                for (const OctreeData &octNodeData : octNodeLevelData)
                {
                    const Mesh::VertexId vertexIndex = octNodeData.first;
                    Vector3 vNormal;
                    if (vertexIndex < mesh->GetNormalsPool().Size())
                    {
                        vNormal = mesh->GetNormalsPool()[vertexIndex];
                    }

                    bool coincidingNormal =
                        (Math::Sign(vNormal.x) == Math::Sign(nDir.x)) &&
                        (Math::Sign(vNormal.y) == Math::Sign(nDir.y)) &&
                        (Math::Sign(vNormal.z) == Math::Sign(nDir.z));
                    if (!shapePreserving || coincidingNormal)
                    {
                        if (!vertexCluster.ContainsKey(vertexIndex) &&
                            !vertexIndexToClusterIndex.ContainsKey(vertexIndex))
                        {
                            VertexData vData;
                            if (vertexIndex < mesh->GetPositionsPool().Size())
                            {
                                vData.pos =
                                    mesh->GetPositionsPool()[vertexIndex];
                            }

                            vData.normal = vNormal;

                            if (vertexIndex < mesh->GetUvsPool().Size())
                            {
                                vData.uv = mesh->GetUvsPool()[vertexIndex];
                            }

                            if (vertexIndex < mesh->GetTangentsPool().Size())
                            {
                                vData.tangent =
                                    mesh->GetTangentsPool()[vertexIndex];
                            }

                            ASSERT(!vertexIndexToClusterIndex.ContainsKey(
                                vertexIndex));
                            vertexIndexToClusterIndex[vertexIndex] =
                                vertexClusters.Size();
                            vertexCluster.Add(vertexIndex, vData);
                        }
                    }
                }

                if (!vertexCluster.IsEmpty())
                {
                    vertexClusters.PushBack(vertexCluster);
                }
            }
        }

        // Now actually simplify the mesh. For each cluster of vertices we will
        // extract one single vertex averaging all the components of the
        // cluster.
        Array<Vector3> positionsLOD;
        Array<Vector3> normalsLOD;
        Array<Vector2> uvsLOD;
        Array<Vector3> tangentsLOD;
        AH<Mesh> simplifiedMesh = Assets::Create<Mesh>();
        for (const VertexCluster &vertexCluster : vertexClusters)
        {
            ASSERT(!vertexCluster.IsEmpty());

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
        simplifiedMesh.Get()->SetPositionsPool(positionsLOD);
        simplifiedMesh.Get()->SetNormalsPool(normalsLOD);
        simplifiedMesh.Get()->SetUvsPool(uvsLOD);
        simplifiedMesh.Get()->SetTangentsPool(tangentsLOD);
        ASSERT(positionsLOD.Size() == vertexClusters.Size());
        ASSERT(normalsLOD.Size() == vertexClusters.Size());
        ASSERT(uvsLOD.Size() == vertexClusters.Size());
        ASSERT(tangentsLOD.Size() == vertexClusters.Size());

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
        UMap<ClusterId, Set<ClusterIdPair>>
            clusterIdToOtherClusterIdsThatFormATriangleWithIt;

        // For each cluster
        for (ClusterId cId = 0; cId < vertexClusters.Size(); ++cId)
        {
            // For each vertex in each vertex cluster
            const VertexCluster &vertexCluster = vertexClusters[cId];
            for (const auto &pair : vertexCluster)
            {
                const Mesh::VertexId &vertexInClusterIndex = pair.first;

                ASSERT(vertexIndexToClusterIndex.ContainsKey(
                    vertexInClusterIndex));
                ASSERT(vertexIndexToTriangleOtherVerticesIndices.ContainsKey(
                    vertexInClusterIndex));

                const ClusterId vCId =
                    // cId;
                    vertexIndexToClusterIndex[vertexInClusterIndex];
                ASSERT(vCId == cId);

                const Set<VertexIdPair> &otherVertexIndicesThatFormATri =
                    vertexIndexToTriangleOtherVerticesIndices.Get(
                        vertexInClusterIndex);

                // For each vertex pair that forms a tri with each vertex in
                // each vertex cluster
                for (const VertexIdPair &otherTriVertexIndices :
                     otherVertexIndicesThatFormATri)
                {
                    // We use minmax so that order doesnt matter in set
                    const VertexIdPair otherVertexIndicesThatFormATri =
                        std::minmax(otherTriVertexIndices.first,
                                    otherTriVertexIndices.second);
                    const VertexIdPair &vp = otherVertexIndicesThatFormATri;

                    if (!clusterIdToOtherClusterIdsThatFormATriangleWithIt
                             .ContainsKey(vCId))
                    {
                        clusterIdToOtherClusterIdsThatFormATriangleWithIt
                            [vCId] = Set<ClusterIdPair>();
                    }

                    if (!vertexIndexToClusterIndex.ContainsKey(vp.first) ||
                        !vertexIndexToClusterIndex.ContainsKey(vp.second))
                    {
                        continue;
                    }

                    ASSERT(clusterIdToOtherClusterIdsThatFormATriangleWithIt
                               .ContainsKey(vCId));
                    ASSERT(vertexIndexToClusterIndex.ContainsKey(vp.first));
                    ASSERT(vertexIndexToClusterIndex.ContainsKey(vp.second));
                    ASSERT(vertexIndexToClusterIndex[vp.first] <
                           vertexClusters.Size());
                    ASSERT(vertexIndexToClusterIndex[vp.second] <
                           vertexClusters.Size());
                    const ClusterIdPair otherClusterIndicesThatFormATri =
                        std::minmax(vertexIndexToClusterIndex[vp.first],
                                    vertexIndexToClusterIndex[vp.second]);
                    clusterIdToOtherClusterIdsThatFormATriangleWithIt[vCId].Add(
                        otherClusterIndicesThatFormATri);
                }
            }
        }

        // Finally calculate vertexIndices with all previous info!
        Array<Mesh::VertexId> vertexClusterTriVertsIndices;
        for (ClusterId cId = 0; cId < vertexClusters.Size(); ++cId)
        {
            if (!clusterIdToOtherClusterIdsThatFormATriangleWithIt.ContainsKey(
                    cId))
            {
                continue;
            }

            ASSERT(
                clusterIdToOtherClusterIdsThatFormATriangleWithIt.ContainsKey(
                    cId));
            const Set<ClusterIdPair> &otherClusterIdsThatFormATriWithThis =
                clusterIdToOtherClusterIdsThatFormATriangleWithIt[cId];
            for (const ClusterIdPair &otherTriVerticesClusterIdPair :
                 otherClusterIdsThatFormATriWithThis)
            {
                const ClusterId otherCId0 = otherTriVerticesClusterIdPair.first;
                const ClusterId otherCId1 =
                    otherTriVerticesClusterIdPair.second;
                // This if is to avoid triplicates vvv
                if (cId < otherCId0 && (otherCId0 < otherCId1) &&
                    cId < otherCId1)
                {
                    ASSERT(cId < vertexClusters.Size());
                    ASSERT(otherCId0 < vertexClusters.Size());
                    ASSERT(otherCId1 < vertexClusters.Size());

                    // Decide triangle winding based on its normal
                    std::array<Mesh::VertexId, 3> triClusterIds = {
                        {cId, otherCId0, otherCId1}};
                    ASSERT(triClusterIds[0] < positionsLOD.Size());
                    ASSERT(triClusterIds[1] < positionsLOD.Size());
                    ASSERT(triClusterIds[2] < positionsLOD.Size());
                    ASSERT(triClusterIds[0] < normalsLOD.Size());
                    ASSERT(triClusterIds[1] < normalsLOD.Size());
                    ASSERT(triClusterIds[2] < normalsLOD.Size());
                    const Vector3 &pos0 = positionsLOD[triClusterIds[0]];
                    const Vector3 &pos1 = positionsLOD[triClusterIds[1]];
                    const Vector3 &pos2 = positionsLOD[triClusterIds[2]];
                    const Vector3 &norm0 = normalsLOD[triClusterIds[0]];
                    const Vector3 &norm1 = normalsLOD[triClusterIds[1]];
                    const Vector3 &norm2 = normalsLOD[triClusterIds[2]];
                    const Vector3 &normal = (norm0 + norm1 + norm2) / 3.0f;
                    if (Vector3::Dot(Vector3::Cross(pos1 - pos0, pos2 - pos0),
                                     normal) < 0)
                    {
                        std::swap(triClusterIds[1], triClusterIds[2]);
                    }

                    // Add triangle indices
                    vertexClusterTriVertsIndices.PushBack(triClusterIds[0]);
                    vertexClusterTriVertsIndices.PushBack(triClusterIds[1]);
                    vertexClusterTriVertsIndices.PushBack(triClusterIds[2]);
                }
            }
        }

        simplifiedMesh.Get()->SetTrianglesVertexIds(
            vertexClusterTriVertsIndices);
        simplifiedMesh.Get()->UpdateVertexNormals();
        simplifiedMesh.Get()->UpdateVAOsAndTables();

        Debug_Log("Level " << level << ": "
                           << vertexClusterTriVertsIndices.Size()
                           << "/"
                           << mesh->GetNumVerticesIds());

        simplifiedMeshesArray.PushBack(simplifiedMesh);

        if (vertexClusterTriVertsIndices.Size() == mesh->GetNumVerticesIds())
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
    const UMap<Mesh::VertexId, Array<Mesh::TriangleId>> &vertexIdxsToTriIdxs,
    SimplificationMethod simplificationMethod)
{
    VertexData vertexRepresentativeData;
    vertexRepresentativeData.pos = Vector3::Zero();
    vertexRepresentativeData.normal = Vector3::Zero();
    vertexRepresentativeData.uv = Vector2::Zero();
    vertexRepresentativeData.tangent = Vector3::Zero();

    if (vertexCluster.IsEmpty())
    {
        return vertexRepresentativeData;
    }

    switch (simplificationMethod)
    {
        case SimplificationMethod::CLUSTERING:
        {
            for (const auto &pair : vertexCluster)
            {
                const VertexData &vData = pair.second;
                vertexRepresentativeData.pos += vData.pos;
                vertexRepresentativeData.normal += vData.normal;
                vertexRepresentativeData.uv += vData.uv;
                vertexRepresentativeData.tangent += vData.tangent;
            }

            const float vertexClusterSize = SCAST<float>(vertexCluster.Size());
            vertexRepresentativeData.pos /= vertexClusterSize;
            vertexRepresentativeData.normal /= vertexClusterSize;
            vertexRepresentativeData.uv /= vertexClusterSize;
            vertexRepresentativeData.tangent /= vertexClusterSize;
        }
        break;

        case SimplificationMethod::QUADRIC_ERROR_METRICS:
        {
            // To get the position use quadric error metrics
            int numTrisComputed = 0;
            Vector3 vertexPosMean = Vector3::Zero();
            Matrix4 verticesTotalQuadricMatrix = Matrix4(0.0f);
            Set<Mesh::TriangleId> visitedTriangles;
            AABox clusterAABox;
            for (const auto &pair : vertexCluster)
            {
                const Mesh::VertexId &vId = pair.first;
                const VertexData &vData = pair.second;
                vertexPosMean += vData.pos;
                clusterAABox.AddPoint(vData.pos);
                /*
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
                    if (visitedTriangles.Contains(triId))
                    {
                        continue;
                    }

                    visitedTriangles.Add(triId);
                    ++numTrisComputed;

                    const std::array<Mesh::VertexId, 3> triVertsIds =
                        mesh.GetVertexIdsFromTriangle(triId);
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
                    const float d = -(a * triP0.x + b * triP0.y + c * triP0.z);
                    const Matrix4 vertexQuadricMatrix = Matrix4(a * a,
                                                                a * b,
                                                                a * c,
                                                                a * d,
                                                                b * a,
                                                                b * b,
                                                                b * c,
                                                                b * d,
                                                                c * a,
                                                                c * b,
                                                                c * c,
                                                                c * d,
                                                                d * a,
                                                                d * b,
                                                                d * c,
                                                                d * d);
                    verticesTotalQuadricMatrix += vertexQuadricMatrix;
                }
                //*/
            }
            vertexPosMean /= float(numTrisComputed);

            // Regularization
            /*
            */
            // verticesTotalQuadricMatrix[0][0] += 1.0f;
            // verticesTotalQuadricMatrix[1][1] += 1.0f;
            // verticesTotalQuadricMatrix[2][2] += 1.0f;
            // verticesTotalQuadricMatrix[3][0] -= vertexPosMean.x;
            // verticesTotalQuadricMatrix[3][1] -= vertexPosMean.y;
            // verticesTotalQuadricMatrix[3][2] -= vertexPosMean.z;
            verticesTotalQuadricMatrix[0][3] = 0.0f;
            verticesTotalQuadricMatrix[1][3] = 0.0f;
            verticesTotalQuadricMatrix[2][3] = 0.0f;
            verticesTotalQuadricMatrix[3][3] = 1.0f;
            // Debug_Peek(verticesTotalQuadricMatrix);

            bool isInvertible;
            Vector3 minimumQuadricErrorPosition =
                (verticesTotalQuadricMatrix.Inversed(0.1f, &isInvertible) *
                 Vector4(0, 0, 0, 1))
                    .xyz();

            // To get normal, uvs, etc. use the clustering method.
            vertexRepresentativeData = GetVertexRepresentativeForCluster(
                mesh,
                vertexCluster,
                vertexIdxsToTriIdxs,
                SimplificationMethod::CLUSTERING);
            if (isInvertible &&
                clusterAABox.Contains(minimumQuadricErrorPosition))
            {
                vertexRepresentativeData.pos = minimumQuadricErrorPosition;
            }
        }
        break;
    }

    return vertexRepresentativeData;
}
