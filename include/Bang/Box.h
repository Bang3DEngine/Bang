#ifndef BOX_H
#define BOX_H

#include <array>

#include "Bang/BangDefines.h"
#include "Bang/Quaternion.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Quad;

class Box
{
public:
    Box();
    ~Box();

    void SetCenter(const Vector3 &center);
    void SetLocalExtents(const Vector3 &localExtents);
    void SetOrientation(const Quaternion &orientation);

    bool Contains(const Vector3 &point) const;
    Vector3 GetExtentX() const;
    Vector3 GetExtentY() const;
    Vector3 GetExtentZ() const;
    const Vector3 &GetCenter() const;
    std::array<Quad, 6> GetQuads() const;
    const Vector3 &GetLocalExtents() const;
    const Quaternion &GetOrientation() const;

private:
    Vector3 m_center;
    Vector3 m_localExtents;
    Quaternion m_orientation;
};
}

#endif  // BOX_H
