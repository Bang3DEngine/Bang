#include "Bang/Triangle.h"

#include "Bang/Assert.h"
#include "Bang/Math.h"
#include "Bang/Matrix4.tcc"
#include "Bang/Plane.h"
#include "Bang/Polygon.h"
#include "Bang/Triangle2D.h"
#include "Bang/Vector3.h"

using namespace Bang;

Triangle::Triangle(const Vector3 &point0,
                   const Vector3 &point1,
                   const Vector3 &point2)
{
    SetPoint(0, point0);
    SetPoint(1, point1);
    SetPoint(2, point2);
}

void Triangle::SetPoint(int i, const Vector3 &point)
{
    (*this)[i] = point;
}

float Triangle::GetArea() const
{
    const Vector3 &p0 = GetPoint(0);
    const Vector3 &p1 = GetPoint(1);
    const Vector3 &p2 = GetPoint(2);
    const Vector3 p01 = (p1 - p0);
    const Vector3 p02 = (p2 - p0);
    const Vector3 cross = Vector3::Cross(p01, p02);
    return cross.Length() / 2.0f;
}

Plane Triangle::GetPlane() const
{
    return Plane(GetPoint(0), GetNormal());
}

Vector3 Triangle::GetNormal() const
{
    return Vector3::Cross(GetPoint(1) - GetPoint(0), GetPoint(2) - GetPoint(0))
        .NormalizedSafe();
}

Vector3 Triangle::GetBarycentricCoordinates(const Vector3 &point) const
{
    Vector3 v0 = GetPoint(1) - GetPoint(0);
    Vector3 v1 = GetPoint(2) - GetPoint(0);

    Vector3 v2 = point - GetPoint(0);
    float d00 = Vector3::Dot(v0, v0);
    float d01 = Vector3::Dot(v0, v1);
    float d11 = Vector3::Dot(v1, v1);
    float d20 = Vector3::Dot(v2, v0);
    float d21 = Vector3::Dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    Vector3 baryCoords;
    baryCoords.y = (d11 * d20 - d01 * d21) / denom;
    baryCoords.z = (d00 * d21 - d01 * d20) / denom;
    baryCoords.x = 1.0f - baryCoords.y - baryCoords.z;
    return baryCoords;
}

Vector3 Triangle::GetPoint(const Vector3 &barycentricCoordinates) const
{
    Vector3 point = GetPoint(0) * barycentricCoordinates[0] +
                    GetPoint(1) * barycentricCoordinates[1] +
                    GetPoint(2) * barycentricCoordinates[2];
    return point;
}

const Vector3 &Triangle::GetPoint(int i) const
{
    return (*this)[i];
}

Polygon Triangle::ToPolygon() const
{
    Polygon poly;
    poly.AddPoint(GetPoint(0));
    poly.AddPoint(GetPoint(1));
    poly.AddPoint(GetPoint(2));
    return poly;
}

const std::array<Vector3, 3> &Triangle::GetPoints() const
{
    return m_points;
}

Vector3 &Triangle::operator[](std::size_t i)
{
    ASSERT(i >= 0 && i <= 2);
    return m_points[i];
}

const Vector3 &Triangle::operator[](std::size_t i) const
{
    ASSERT(i >= 0 && i <= 2);
    return m_points[i];
}

namespace Bang
{
Triangle operator*(const Matrix4 &m, const Triangle &t)
{
    return Triangle(m.TransformedPoint(t[0]),
                    m.TransformedPoint(t[1]),
                    m.TransformedPoint(t[2]));
}
}  // namespace Bang
