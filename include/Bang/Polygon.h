#ifndef POLYGON_H
#define POLYGON_H

#include <cstddef>

#include "Bang/Array.h"
#include "Bang/Axis.h"
#include "Bang/BangDefines.h"

namespace Bang
{
class Plane;
class Polygon2D;

class Polygon
{
public:
    Polygon() = default;
    ~Polygon() = default;

    void AddPoint(const Vector3 &p);
    void AddPoints(const Array<Vector3> &points);
    void SetPoint(int i, const Vector3 &p);

    Plane GetPlane() const;
    Vector3 GetNormal() const;
    Polygon2D ProjectedOnAxis(Axis3D axis) const;

    const Vector3 &GetPoint(int i) const;
    const Array<Vector3> &GetPoints() const;

    Vector3 &operator[](std::size_t i);
    const Vector3 &operator[](std::size_t i) const;

private:
    Array<Vector3> m_points;
};
}

#endif  // POLYGON_H
