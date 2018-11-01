#include "Bang/Plane.h"

#include "Bang/Vector3.h"

using namespace Bang;

Plane::Plane(const Vector3 &point, const Vector3 &normal)
{
    SetPoint(point);
    SetNormal(normal);
}

void Plane::SetPoint(const Vector3 &point)
{
    m_point = point;
}

void Plane::SetNormal(const Vector3 &normal)
{
    m_normal = normal.NormalizedSafe();
}

const Vector3 &Plane::GetPoint() const
{
    return m_point;
}

const Vector3 &Plane::GetNormal() const
{
    return m_normal;
}

Vector3 Plane::GetMirroredPoint(const Vector3 &point) const
{
    Vector3 mirroredPoint = point - (GetNormal() * GetDistanceTo(point)) * 2.0f;
    return mirroredPoint;
}

Vector3 Plane::GetProjectedPoint(const Vector3 &point) const
{
    Vector3 projectedPoint = point - (GetNormal() * GetDistanceTo(point));
    return projectedPoint;
}

Vector3 Plane::GetProjectedVector(const Vector3 &vector) const
{
    return Plane::GetProjectedPoint(vector);
}

Vector3 Plane::GetMirroredVector(const Vector3 &vector) const
{
    return GetMirroredPoint(GetPoint() + vector) - GetPoint();
}

float Plane::GetDistanceTo(const Vector3 &point) const
{
    float dist = Vector3::Dot(point - GetPoint(), GetNormal());
    return dist;
}
