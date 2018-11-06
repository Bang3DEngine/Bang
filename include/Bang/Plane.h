#ifndef PLANE_H
#define PLANE_H

#include "Bang/BangDefines.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Plane
{
public:
    Plane() = default;
    Plane(const Vector3 &point, const Vector3 &normal);
    ~Plane() = default;

    void SetPoint(const Vector3 &point);
    void SetNormal(const Vector3 &normal);

    const Vector3 &GetPoint() const;
    const Vector3 &GetNormal() const;
    float GetDistanceTo(const Vector3 &point) const;
    Vector3 GetMirroredPoint(const Vector3 &point) const;
    Vector3 GetMirroredVector(const Vector3 &vector) const;
    Vector3 GetProjectedPoint(const Vector3 &point) const;
    Vector3 GetProjectedVector(const Vector3 &vector) const;

private:
    Vector3 m_point = Vector3::Zero();
    Vector3 m_normal = Vector3::Up();
};
}

#endif  // PLANE_H
