#include "Bang/Triangle2D.h"

#include "Bang/Assert.h"
#include "Bang/Geometry.h"

using namespace Bang;

Triangle2D::Triangle2D(const Vector2 &point0,
                       const Vector2 &point1,
                       const Vector2 &point2)
{
    SetPoint(0, point0);
    SetPoint(1, point1);
    SetPoint(2, point2);
}

void Triangle2D::SetPoint(int i, const Vector2 &point)
{
    ASSERT(i >= 0 && i <= 2);
    m_points[i] = point;
}

float Triangle2D::GetArea() const
{
    float a = Vector2::Distance(GetPoint(0), GetPoint(1));
    float b = Vector2::Distance(GetPoint(1), GetPoint(2));
    float c = Vector2::Distance(GetPoint(2), GetPoint(0));
    float s = (a + b + c) / 2;
    float area = Math::Sqrt(s * (s - a) * (s - b) * (s - c));
    return area;
}

bool Triangle2D::Contains(const Vector2 &point) const
{
    const Vector2 &p0 = GetPoint(0);
    const Vector2 &p1 = GetPoint(1);
    const Vector2 &p2 = GetPoint(2);

    const Geometry::Orientation triOri = Geometry::GetOrientation(p0, p1, p2);
    if (triOri == Geometry::Orientation::ZERO)
    {
        return false;
    }

    return (Geometry::GetOrientation(p0, p1, point) == triOri) &&
           (Geometry::GetOrientation(p1, p2, point) == triOri) &&
           (Geometry::GetOrientation(p2, p0, point) == triOri);
}

Vector3 Triangle2D::GetBarycentricCoordinates(const Vector2 &point) const
{
    Triangle2D tri0(point, GetPoint(1), GetPoint(2));
    Triangle2D tri1(point, GetPoint(0), GetPoint(2));
    Triangle2D tri2(point, GetPoint(0), GetPoint(1));
    float area = GetArea();
    return Vector3(
        tri0.GetArea() / area, tri1.GetArea() / area, tri2.GetArea() / area);
}

const Vector2 &Triangle2D::GetPoint(int i) const
{
    ASSERT(i >= 0 && i <= 2);
    return GetPoints()[i];
}

const std::array<Vector2, 3> &Triangle2D::GetPoints() const
{
    return m_points;
}

Vector2 &Triangle2D::operator[](std::size_t i)
{
    return m_points[i];
}

const Vector2 &Triangle2D::operator[](std::size_t i) const
{
    return GetPoints()[i];
}
