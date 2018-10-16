#ifndef BOX_H
#define BOX_H

#include <array>

#include "Bang/Bang.h"
#include "Bang/BangDefines.h"
#include "Bang/Quad.h"
#include "Bang/Quaternion.h"

NAMESPACE_BANG_BEGIN

FORWARD class Quad;

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

NAMESPACE_BANG_END

#endif // BOX_H

