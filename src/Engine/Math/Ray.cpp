#include "Bang/Ray.h"

#include "Bang/Matrix4.h"
#include "Bang/Vector4.h"

using namespace Bang;

Ray::Ray()
{
}

Ray::Ray(const Vector3 &origin, const Vector3 &direction)
{
    SetOrigin(origin);
    SetDirection(direction);
}

Ray::~Ray()
{
}

void Ray::SetOrigin(const Vector3 &origin)
{
    m_origin = origin;
}
void Ray::SetDirection(const Vector3 &direction)
{
    m_direction = direction.NormalizedSafe();
}

Vector3 Ray::GetPoint(float t) const
{
    return GetOrigin() + GetDirection() * t;
}

const Vector3 &Ray::GetOrigin() const
{
    return m_origin;
}

const Vector3 &Ray::GetDirection() const
{
    return m_direction;
}

namespace Bang
{
Ray operator*(const Matrix4 &m, const Ray &ray)
{
    Vector3 newDir = m.TransformedVector(ray.GetDirection());
    Vector3 newOrigin = m.TransformedPoint(ray.GetOrigin());
    return Ray(newOrigin, newDir);
}
}
