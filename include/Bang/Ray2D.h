#ifndef RAY2D_H
#define RAY2D_H

#include "Bang/BangDefines.h"
#include "Bang/Vector2.h"

namespace Bang
{
class Ray2D
{
public:
    Ray2D();
    Ray2D(const Vector2 &origin, const Vector2 &direction);
    ~Ray2D();

    void SetOrigin(const Vector2 &origin);
    void SetDirection(const Vector2 &direction);

    // Gets a point along the ray with distance t to the ray origin
    Vector2 GetPoint(float t) const;

    const Vector2 &GetOrigin() const;
    const Vector2 &GetDirection() const;

private:
    Vector2 m_origin = Vector2::Zero();
    Vector2 m_direction = Vector2(1, 0);
};
}

#endif  // RAY2D_H
