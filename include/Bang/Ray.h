#ifndef RAY_H
#define RAY_H

#include "Bang/BangDefines.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Ray
{
public:
    Ray();
    Ray(const Vector3 &origin, const Vector3 &direction);
    ~Ray();

    void SetOrigin(const Vector3 &origin);
    void SetDirection(const Vector3 &direction);

    // Gets a point along the ray with distance t to the ray origin
    Vector3 GetPoint(float t) const;

    const Vector3 &GetOrigin() const;
    const Vector3 &GetDirection() const;

private:
    Vector3 m_origin = Vector3::Zero();
    Vector3 m_direction = Vector3::Forward();
};

Ray operator*(const Matrix4 &m, const Ray &ray);
}

#endif  // RAY_H
