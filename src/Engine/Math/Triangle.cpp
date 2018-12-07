#include "Bang/Triangle.h"

#include "Bang/Assert.h"
#include "Bang/Math.h"
#include "Bang/Matrix4.tcc"
#include "Bang/Plane.h"
#include "Bang/Polygon.h"
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
    float a = Vector3::Distance(GetPoint(0), GetPoint(1));
    float b = Vector3::Distance(GetPoint(1), GetPoint(2));
    float c = Vector3::Distance(GetPoint(2), GetPoint(0));
    float s = (a + b + c) / 2;
    float area = Math::Sqrt(s * (s - a) * (s - b) * (s - c));
    return area;
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
    Vector3 projPoint = GetPlane().GetProjectedPoint(point);
    Triangle tri0(projPoint, GetPoint(1), GetPoint(2));
    Triangle tri1(projPoint, GetPoint(0), GetPoint(2));
    Triangle tri2(projPoint, GetPoint(0), GetPoint(1));
    float area = GetArea();
    return Vector3(
        tri0.GetArea() / area, tri1.GetArea() / area, tri2.GetArea() / area);
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
