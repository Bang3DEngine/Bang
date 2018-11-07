#include "Bang/Box.h"

#include "Bang/Matrix4.h"
#include "Bang/Matrix4.tcc"
#include "Bang/Quad.h"
#include "Bang/Quaternion.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

using namespace Bang;

Box::Box()
{
}

Box::~Box()
{
}

void Box::SetCenter(const Vector3 &center)
{
    m_center = center;
}

void Box::SetLocalExtents(const Vector3 &localExtents)
{
    m_localExtents = localExtents;
}

void Box::SetOrientation(const Quaternion &orientation)
{
    m_orientation = orientation;
}

bool Box::Contains(const Vector3 &point) const
{
    Matrix4 transform = Matrix4::RotateMatrix(-GetOrientation()) *
                        Matrix4::TranslateMatrix(-GetCenter());
    Vector3 localPoint = (transform * Vector4(point, 1)).xyz();

    const Vector3 &lExt = GetLocalExtents();
    return (localPoint.x >= -lExt.x && localPoint.x <= lExt.x) &&
           (localPoint.y >= -lExt.y && localPoint.y <= lExt.y) &&
           (localPoint.z >= -lExt.z && localPoint.z <= lExt.z);
}

Vector3 Box::GetExtentX() const
{
    return GetOrientation() * (GetLocalExtents().x * Vector3::Right());
}

Vector3 Box::GetExtentY() const
{
    return GetOrientation() * (GetLocalExtents().y * Vector3::Up());
}

Vector3 Box::GetExtentZ() const
{
    return GetOrientation() * (GetLocalExtents().z * Vector3::Forward());
}

const Vector3 &Box::GetCenter() const
{
    return m_center;
}

const Vector3 &Box::GetLocalExtents() const
{
    return m_localExtents;
}

std::array<Quad, 6> Box::GetQuads() const
{
    const Vector3 &c = GetCenter();
    Vector3 ex = GetExtentX();
    Vector3 ey = GetExtentY();
    Vector3 ez = GetExtentZ();
    std::array<Quad, 6> quads;
    Quad leftQuad = Quad(
        c - ex + ey + ez, c - ex + ey - ez, c - ex - ey - ez, c - ex - ey + ez);
    Quad rightQuad = Quad(
        c + ex + ey + ez, c + ex + ey - ez, c + ex - ey - ez, c + ex - ey + ez);
    Quad topQuad = Quad(
        c + ex + ey + ez, c + ex + ey - ez, c - ex + ey - ez, c - ex + ey + ez);
    Quad botQuad = Quad(
        c + ex - ey + ez, c + ex - ey - ez, c - ex - ey - ez, c - ex - ey + ez);
    Quad frontQuad = Quad(
        c + ex + ey - ez, c + ex - ey - ez, c - ex - ey - ez, c - ex + ey - ez);
    Quad backQuad = Quad(
        c + ex + ey + ez, c + ex - ey + ez, c - ex - ey + ez, c - ex + ey + ez);

    quads[0] = leftQuad;
    quads[1] = rightQuad;
    quads[2] = topQuad;
    quads[3] = botQuad;
    quads[4] = frontQuad;
    quads[5] = backQuad;

    return quads;
}

const Quaternion &Box::GetOrientation() const
{
    return m_orientation;
}
