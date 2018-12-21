#include "Bang/NavigationMesh.h"

#include "Bang/AARect.h"
#include "Bang/Collider.h"
#include "Bang/GameObject.h"
#include "Bang/Physics.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/Random.h"
#include "Bang/Transform.h"
#include "PxPhysicsAPI.h"

using namespace Bang;

NavigationMesh::NavigationMesh()
{
    SET_INSTANCE_CLASS_ID(NavigationMesh);

    SetNumCells(10);
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
    ASSERT(
        IsCellInsideGrid(SCAST<uint>(originCell.x), SCAST<uint>(originCell.y)));
    ASSERT(IsCellInsideGrid(SCAST<uint>(destinyCell.x),
                            SCAST<uint>(destinyCell.y)));

    bool solutionFound = false;
    const uint N = GetNumCells();
    Array<Array<Vector2i>> previousCells =
        Array<Array<Vector2i>>(N, Array<Vector2i>(N, Vector2i(-1)));
    Array<Vector3> pathPositions;
    {
        constexpr float INF = std::numeric_limits<float>::infinity();
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
        costs[SCAST<uint>(originCell.y)][SCAST<uint>(originCell.x)] = 0.0f;

        std::priority_queue<AStarNode> nodesToVisit;
        nodesToVisit.push(originNode);

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
                            const float weight = 1.0f;
                            const float newCost =
                                costs[SCAST<uint>(cur.idx.y)]
                                     [SCAST<uint>(cur.idx.x)] +
                                weight;
                            if (newCost < costs[ny][nx])
                            {
                                heuristicCost = Vector2::Distance(
                                    Vector2(nx, ny),
                                    Vector2(destinyCell.x, destinyCell.y));

                                float priority = newCost + heuristicCost;
                                nodesToVisit.push(
                                    AStarNode(Vector2i(nx, ny), priority));

                                costs[ny][nx] = newCost;
                                previousCells[ny][nx] = cur.idx;
                            }
                        }
                    }
                }
            }
        }
    }

    if (solutionFound)
    {
        Vector2i previousCell = destinyCell;
        Vector2i currentCell = destinyCell;
        while (currentCell != originCell)
        {
            currentCell = previousCell;
            ASSERT(currentCell.x != -1 && currentCell.y != -1);

            pathPositions.PushBack(GetCellCenter(SCAST<uint>(currentCell.x),
                                                 SCAST<uint>(currentCell.y)));
            previousCell = previousCells[SCAST<uint>(currentCell.y)]
                                        [SCAST<uint>(currentCell.x)];
        }
        pathPositions.Reverse();
        pathPositions.PushBack(destiny);
    }

    return pathPositions;
}

void NavigationMesh::RecomputeCollisions()
{
    m_collisions =
        Array<Array<bool>>(GetNumCells(), Array<bool>(GetNumCells(), false));

    if (!GetGameObject())
    {
        return;
    }

    if (PxSceneContainer *pxSceneCont =
            Physics::GetInstance()->GetPxSceneContainerFromScene(
                GetGameObject()->GetScene()))
    {
        Array<Collider *> colliders = pxSceneCont->GetColliders();
        for (Collider *collider : colliders)
        {
            collider->UpdatePxShape();
        }

        for (uint i = 0; i < GetNumCells(); ++i)
        {
            for (uint j = 0; j < GetNumCells(); ++j)
            {
                bool colliding = false;
                {
                    for (Collider *collider : colliders)
                    {
                        if (!collider->IsEnabledRecursively() ||
                            collider->GetIsTrigger() ||
                            !collider->GetUseInNavMesh())
                        {
                            continue;
                        }

                        physx::PxBoxGeometry cellBoxGeometry;
                        cellBoxGeometry.halfExtents =
                            Physics::GetPxVec3FromVector3(
                                GetCellSize().x1y() * Vector3(1, 0.001f, 1) *
                                0.5f);

                        physx::PxTransform cellPlaneTransform;
                        cellPlaneTransform.p =
                            Physics::GetPxVec3FromVector3(GetCellCenter(j, i));
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

void NavigationMesh::SetNumCells(uint divisions)
{
    if (divisions != GetNumCells())
    {
        m_numCells = divisions;
        RecomputeCollisions();
    }
}

const Array<Array<bool>> &NavigationMesh::GetCollisions() const
{
    return m_collisions;
}

bool NavigationMesh::IsPointColliding(const Vector3 &point) const
{
    Vector2i closestCell = GetClosestCellTo(point);
    return IsCellColliding(closestCell.x, closestCell.y);
}

bool NavigationMesh::IsCellColliding(uint xi, uint yi) const
{
    return m_collisions[yi][xi];
}

bool NavigationMesh::IsCellInsideGrid(uint xi, uint yi) const
{
    return (xi < GetNumCells()) && (yi < GetNumCells());
}

AARect NavigationMesh::GetGridAARect() const
{
    Vector2 gridSizeHalf = GetGridSize() * 0.5f;
    AARect gridRect(
        -gridSizeHalf.x, -gridSizeHalf.y, gridSizeHalf.x, gridSizeHalf.y);
    gridRect += GetGridCenter().xz();
    return gridRect;
}

Vector3 NavigationMesh::GetClosestPointInsideGrid(const Vector3 &point) const
{
    AARect gridRect = GetGridAARect();

    Vector2 pos2 = point.xz();
    if (!gridRect.Contains(pos2))
    {
        Vector2 closestPoint = gridRect.GetClosestPointInAARect(pos2);
        pos2 = closestPoint +
               (gridRect.GetCenter() - closestPoint).NormalizedSafe() * 0.01f;
    }
    ASSERT(gridRect.Contains(pos2));

    return Vector3(pos2.x, 0, pos2.y);
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
    return GetGridSize() / SCAST<float>(GetNumCells());
}

Vector2 NavigationMesh::GetGridSize() const
{
    return GetGameObject()->GetTransform()->GetScale().xz();
}

uint NavigationMesh::GetNumCells() const
{
    return m_numCells;
}

void NavigationMesh::Reflect()
{
    Component::Reflect();

    ReflectVarMember<NavigationMesh, uint>(
        "NumCells",
        &NavigationMesh::SetNumCells,
        &NavigationMesh::GetNumCells,
        this,
        BANG_REFLECT_HINT_MIN_VALUE(2) + BANG_REFLECT_HINT_STEP_VALUE(1.0f));

    BANG_REFLECT_BUTTON(NavigationMesh, "Recompute collisions", [this]() {
        RecomputeCollisions();
    });
}

Vector2i NavigationMesh::GetClosestCellTo(const Vector3 &position) const
{
    AARect gridRect = GetGridAARect();
    Vector3 closestPoint = GetClosestPointInsideGrid(position);
    Vector2i cellIdx =
        Vector2i((closestPoint.xz() - gridRect.GetMin()) / GetCellSize());
    ASSERT(IsCellInsideGrid(SCAST<uint>(cellIdx.x), SCAST<uint>(cellIdx.y)));

    return cellIdx;
}
