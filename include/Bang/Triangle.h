#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <array>
#include <cstddef>

#include "Bang/BangDefines.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Plane;
class Polygon;

class Triangle
{
public:
    Triangle() = default;
    Triangle(const Vector3 &point0,
             const Vector3 &point1,
             const Vector3 &point2);
    ~Triangle() = default;

    void SetPoint(int i, const Vector3 &point);

    float GetArea() const;
    Plane GetPlane() const;
    Vector3 GetNormal() const;
    Vector3 GetBarycentricCoordinates(const Vector3 &point) const;
    const std::array<Vector3, 3> &GetPoints() const;
    const Vector3 &GetPoint(int i) const;
    Polygon ToPolygon() const;

    Vector3 &operator[](std::size_t i);
    const Vector3 &operator[](std::size_t i) const;

private:
    std::array<Vector3, 3> m_points;
};

Triangle operator*(const Matrix4 &m, const Triangle &t);
}

#endif  // TRIANGLE_H
