#ifndef TRIANGLE2D_H
#define TRIANGLE2D_H

#include <array>
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Triangle2D
{
public:
    Triangle2D() = default;
    Triangle2D(const Vector2 &point0,
               const Vector2 &point1,
               const Vector2 &point2);
    ~Triangle2D() = default;

    void SetPoint(int i, const Vector2 &point);

    float GetArea() const;
    bool Contains(const Vector2 &point) const;
    Vector3 GetBarycentricCoordinates(const Vector2 &point) const;
    Vector2 GetPoint(const Vector3 &barycentricCoordinates) const;
    const Vector2 &GetPoint(int i) const;
    const std::array<Vector2, 3> &GetPoints() const;

    Vector2 &operator[](std::size_t i);
    const Vector2 &operator[](std::size_t i) const;

private:
    std::array<Vector2, 3> m_points;
};
}

#endif  // TRIANGLE2D_H
