#ifndef NAVIGATIONMESH_H
#define NAVIGATIONMESH_H

#include "Bang/Array.h"
#include "Bang/Bang.h"
#include "Bang/Component.h"

namespace Bang
{
class NavigationMesh : public Component
{
    COMPONENT(NavigationMesh)

public:
    NavigationMesh();
    virtual ~NavigationMesh() override;

    // Component
    void OnStart() override;

    Array<Vector3> GetPath(const Vector3 &origin, const Vector3 &destiny) const;

    void RecomputeCollisions();
    void SetNumCells(uint numCells);

    const Array<Array<bool>> &GetCollisions() const;
    bool IsPointColliding(const Vector3 &point) const;
    bool IsCellColliding(uint xi, uint yi) const;
    bool IsCellInsideGrid(uint xi, uint yi) const;
    AARect GetGridAARect() const;
    Vector3 GetClosestPointInsideGrid(const Vector3 &point) const;
    Vector3 GetCellCenter(uint xi, uint yi) const;
    Vector3 GetGridCenter() const;
    Vector2 GetCellSize() const;
    Vector2 GetGridSize() const;
    uint GetNumCells() const;

    // IReflectable
    virtual void Reflect() override;

private:
    Array<Array<bool>> m_collisions;
    uint m_numCells = 0;

    Vector2i GetClosestCellTo(const Vector3 &position) const;
};
}

#endif  // NAVIGATIONMESH_H
