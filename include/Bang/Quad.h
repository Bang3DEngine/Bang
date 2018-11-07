#ifndef QUAD_H
#define QUAD_H

#include <array>
#include <cstddef>

#include "Bang/BangDefines.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Plane;
class Polygon;
class Triangle;

class Quad
{
public:
    Quad() = default;

    // Points must be in CCW order!
    Quad(const Vector3 &p0,
         const Vector3 &p1,
         const Vector3 &p2,
         const Vector3 &p3);

    ~Quad() = default;

    void SetPoint(int i, const Vector3 &p);

    Vector3 GetNormal() const;
    Plane GetPlane() const;
    const Vector3 &GetPoint(int i) const;
    const std::array<Vector3, 4> &GetPoints() const;
    Polygon ToPolygon() const;

    // Returns the two triangles of this quad
    void GetTriangles(Triangle *t0, Triangle *t1) const;

    Vector3 &operator[](std::size_t i);
    const Vector3 &operator[](std::size_t i) const;

private:
    std::array<Vector3, 4> m_points = {
        {Vector3::Zero(), Vector3::Zero(), Vector3::Zero(), Vector3::Zero()}};
};

Quad operator*(const Matrix4 &m, const Quad &q);
}

#endif  // QUAD_H
