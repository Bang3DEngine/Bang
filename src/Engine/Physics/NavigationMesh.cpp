#include "Bang/NavigationMesh.h"

#include "Bang/AARect.h"
#include "Bang/GameObject.h"
#include "Bang/Physics.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/Random.h"
#include "Bang/Transform.h"
#include "PxPhysicsAPI.h"

using namespace Bang;

NavigationMesh::NavigationMesh()
{
}

NavigationMesh::~NavigationMesh()
{
}

void NavigationMesh::OnStart()
{
    Component::OnStart();

    RecomputeCollisions();
}

Array<Vector3> NavigationMesh::GetPath(const Vector3 &origin,
                                       const Vector3 &destiny) const
{
    Vector2i originCell = GetClosestCellTo(origin);
    Vector2i destinyCell = GetClosestCellTo(destiny);
    ASSERT(IsCellInsideGrid(originCell.x, originCell.y));
    ASSERT(IsCellInsideGrid(destinyCell.x, destinyCell.y));

    bool solutionFound = false;
    Array<Vector3> pathPositions;
    pathPositions.PushBack(GetCellCenter(originCell.x, originCell.y));
    {
        constexpr float INF = std::numeric_limits<float>::infinity();
        const uint N = GetDivisions();

        Array<Array<float>> weights = Array<Array<float>>(N, Array<float>(N));
        for (uint i = 0; i < N; ++i)
        {
            for (uint j = 0; j < N; ++j)
            {
                weights[i][j] = IsCellColliding(j, i) ? INF : 0.0f;
            }
        }

        struct AStarNode
        {
            Vector2i idx;
            float cost;
            AStarNode(const Vector2i &i, float c) : idx(i), cost(c)
            {
            }
            bool operator<(const AStarNode &rhs) const
            {
                return this->cost > rhs.cost;
            }
            bool operator==(const AStarNode &rhs) const
            {
                return this->idx == rhs.idx;
            }
        };

        AStarNode originNode(originCell, 0.0f);
        AStarNode destinyNode(destinyCell, 0.0f);

        Array<Array<float>> costs(N, Array<float>(N, INF));
        costs[originCell.y][originCell.x] = 0.0f;

        std::priority_queue<AStarNode> nodesToVisit;
        nodesToVisit.push(originNode);

        bool solutionFound = false;
        while (!nodesToVisit.empty())
        {
            AStarNode cur = nodesToVisit.top();
            if (cur == destinyNode)
            {
                solutionFound = true;
                break;
            }
            nodesToVisit.pop();

            float heuristicCost;
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (!(i == 0 && j == 0))
                    {
                        uint nx = SCAST<uint>(cur.idx.x + j);
                        uint ny = SCAST<uint>(cur.idx.y + i);
                        if (IsCellInsideGrid(nx, ny) &&
                            !IsCellColliding(nx, ny))
                        {
                            float newCost =
                                costs[cur.idx.y][cur.idx.x] + weights[ny][nx];
                            if (newCost < costs[ny][nx])
                            {
                                heuristicCost = Vector2::Distance(
                                    Vector2(nx, ny),
                                    Vector2(destinyCell.x, destinyCell.y));

                                float priority = newCost + heuristicCost;
                                nodesToVisit.push(
                                    AStarNode(Vector2i(nx, ny), priority));

                                costs[ny][nx] = newCost;
                                pathPositions.PushBack(GetCellCenter(nx, ny));
                            }
                        }
                    }
                }
            }
        }
    }

    if (!solutionFound)
    {
        pathPositions.Clear();
    }

    return pathPositions;
}

void NavigationMesh::RecomputeCollisions()
{
    m_collisions =
        Array<Array<bool>>(GetDivisions(), Array<bool>(GetDivisions(), false));

    if (!GetGameObject())
    {
        return;
    }

    if (PxSceneContainer *pxSceneCont =
            Physics::GetInstance()->GetPxSceneContainerFromScene(
                GetGameObject()->GetScene()))
    {
        Array<Collider *> colliders = pxSceneCont->GetColliders();
        for (uint i = 0; i < GetDivisions(); ++i)
        {
            for (uint j = 0; j < GetDivisions(); ++j)
            {
                bool colliding = false;
                {
                    for (Collider *collider : colliders)
                    {
                        physx::PxBoxGeometry cellBoxGeometry;
                        cellBoxGeometry.halfExtents =
                            Physics::GetPxVec3FromVector3(
                                GetCellSize().x1y() * Vector3(1, 0.001f, 1) *
                                0.5f);

                        physx::PxTransform cellPlaneTransform;
                        cellPlaneTransform.p =
                            Physics::GetPxVec3FromVector3(GetCellCenter(i, j));
                        cellPlaneTransform.q = physx::PxQuat(physx::PxIdentity);

                        if (Physics::Overlap(
                                collider, cellBoxGeometry, cellPlaneTransform))
                        {
                            colliding = true;
                            break;
                        }
                    }
                }
                m_collisions[i][j] = colliding;
            }
        }
    }
}

void NavigationMesh::SetDivisions(uint divisions)
{
    if (divisions != GetDivisions())
    {
        m_divisions = divisions;
        RecomputeCollisions();
    }
}

const Array<Array<bool>> &NavigationMesh::GetCollisions() const
{
    return m_collisions;
}

bool NavigationMesh::IsCellColliding(uint xi, uint yi) const
{
    return m_collisions[xi][yi];
}

bool NavigationMesh::IsCellInsideGrid(uint xi, uint yi) const
{
    return (xi < GetDivisions()) && (yi < GetDivisions());
}

Vector3 NavigationMesh::GetCellCenter(uint xi, uint yi) const
{
    Vector2 cellSize = GetCellSize();
    return GetGameObject()->GetTransform()->GetPosition() -
           GetGridSize().x0y() * 0.5f +
           Vector3(xi * cellSize.x, 0, yi * cellSize.y) + cellSize.x0y() * 0.5f;
}

Vector3 NavigationMesh::GetGridCenter() const
{
    return GetGameObject()->GetTransform()->GetPosition();
}

Vector2 NavigationMesh::GetCellSize() const
{
    return GetGridSize() / SCAST<float>(GetDivisions());
}

Vector2 NavigationMesh::GetGridSize() const
{
    return GetGameObject()->GetTransform()->GetScale().xz();
}

uint NavigationMesh::GetDivisions() const
{
    return m_divisions;
}

void NavigationMesh::Reflect()
{
    Component::Reflect();

    ReflectVarMember<NavigationMesh, uint>(
        "Divisions",
        &NavigationMesh::SetDivisions,
        &NavigationMesh::GetDivisions,
        this,
        BANG_REFLECT_HINT_MIN_VALUE(2) + BANG_REFLECT_HINT_STEP_VALUE(1.0f));

    BANG_REFLECT_BUTTON(NavigationMesh, "Recompute collisions", [this]() {
        RecomputeCollisions();
    });
}

Vector2i NavigationMesh::GetClosestCellTo(const Vector3 &position) const
{
    Vector2 gridSizeHalf = GetGridSize() * 0.5f;
    AARect gridRect(
        -gridSizeHalf.x, -gridSizeHalf.y, gridSizeHalf.x, gridSizeHalf.y);
    gridRect += GetGridCenter().xy();

    Vector2 pos2 = position.xz();
    if (!gridRect.Contains(pos2))
    {
        Vector2 closestPoint = gridRect.GetClosestPointInAARect(pos2);
        pos2 = closestPoint + (closestPoint - pos2).Normalized() * 0.001f;
    }
    ASSERT(gridRect.Contains(pos2));

    Vector2i cellIdx = Vector2i((pos2 - gridRect.GetMin()) / GetCellSize());
    ASSERT(IsCellInsideGrid(cellIdx.x, cellIdx.y));

    return cellIdx;
}
