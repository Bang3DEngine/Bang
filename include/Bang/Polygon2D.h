#ifndef POLYGON2D_H
#define POLYGON2D_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"

namespace Bang
{
class Polygon2D
{
public:
    Polygon2D();
    ~Polygon2D();

    void AddPoint(const Vector2 &p);
    void SetPoint(int i, const Vector2 &p);
    bool Contains(const Vector2 &p);

    const Vector2 &GetPoint(int i) const;
    const Array<Vector2> &GetPoints() const;

private:
    Array<Vector2> m_points;
};
}

#endif  // POLYGON2D_H
