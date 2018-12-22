#include "Bang/Transformation.h"

#include "Bang/Matrix4.h"

using namespace Bang;

Transformation::Transformation()
{
}

Transformation::Transformation(const Vector3 &position,
                               const Quaternion &rotation,
                               const Vector3 &scale)
{
    SetPosition(position);
    SetRotation(rotation);
    SetScale(scale);
}

Transformation::Transformation(const Matrix4 &transformationMatrix)
{
    FillFromMatrix(transformationMatrix);
}

const Transformation &Transformation::Identity()
{
    static const Transformation identity;
    return identity;
}

Transformation Transformation::Inversed() const
{
    return Transformation(
        -GetPosition(), GetRotation().Inversed(), 1.0f / GetScale());
}

const Matrix4 &Transformation::GetMatrix() const
{
    CalculateLocalToWorldMatrixIfNeeded();
    return m_localToWorldMatrix;
}

const Matrix4 &Transformation::GetMatrixInverse() const
{
    CalculateWorldToLocalMatrixIfNeeded();
    return m_worldToLocalMatrix;
}

void Transformation::FillFromMatrix(const Matrix4 &transformMatrix)
{
    Vector3 position = transformMatrix.GetTranslation();
    Quaternion rotation = transformMatrix.GetRotation();
    Vector3 scale = transformMatrix.GetScale();
    SetPosition(position);
    SetRotation(rotation);
    SetScale(scale);
}

void Transformation::SetPosition(const Vector3 &position)
{
    if (position != m_position)
    {
        m_position = position;
        m_validLocalToWorldMatrix = false;
        m_validWorldToLocalMatrix = false;
    }
}

void Transformation::SetRotation(const Quaternion &rotation)
{
    if (rotation != m_rotation)
    {
        m_rotation = rotation;
        m_validLocalToWorldMatrix = false;
        m_validWorldToLocalMatrix = false;
    }
}

void Transformation::SetScale(const Vector3 &scale)
{
    if (scale != m_scale)
    {
        m_scale = scale;
        m_validLocalToWorldMatrix = false;
        m_validWorldToLocalMatrix = false;
    }
}

void Transformation::LookAt(const Vector3 &target, const Vector3 &up)
{
    if (target != GetPosition())
    {
        SetRotation(Quaternion::LookDirection(target - GetPosition(), up));
    }
}

void Transformation::LookInDirection(const Vector3 &dir, const Vector3 &up)
{
    LookAt(GetPosition() + dir * 99.0f, up);
}

void Transformation::Translate(const Vector3 &translation)
{
    SetPosition(GetPosition() + translation);
}

void Transformation::Rotate(const Quaternion &rotation)
{
    SetRotation(rotation * GetRotation());
}

void Transformation::Scale(const Vector3 &scale)
{
    SetScale(scale * GetScale());
}

Transformation Transformation::Composed(const Transformation &lhs,
                                        const Transformation &rhs)
{
    const Vector3 &lp = lhs.GetPosition();
    const Quaternion &lr = lhs.GetRotation();
    const Vector3 &ls = lhs.GetScale();
    const Vector3 &rp = rhs.GetPosition();
    const Quaternion &rr = rhs.GetRotation();
    const Vector3 &rs = rhs.GetScale();

    Transformation newTransformation;
    newTransformation.SetPosition(lp + (lr * (ls * rp)));
    newTransformation.SetRotation(lr * rr);
    newTransformation.SetScale(ls * rs);

    return newTransformation;
}

Vector3 Transformation::TransformedPoint(const Vector3 &point)
{
    return ((*this) * Vector4(point, 1)).xyz();
}

Vector3 Transformation::TransformedVector(const Vector3 &vector)
{
    return ((*this) * Vector4(vector, 0)).xyz();
}

Vector3 Transformation::FromLocalToWorldPoint(const Vector3 &point) const
{
    return GetMatrix().TransformedPoint(point);
}

Vector3 Transformation::FromLocalToWorldVector(const Vector3 &vector) const
{
    return GetMatrix().TransformedVector(vector);
}

Vector3 Transformation::FromLocalToWorldDirection(const Vector3 &dir) const
{
    return GetRotation() * dir;
}

Vector3 Transformation::FromWorldToLocalPoint(const Vector3 &point) const
{
    return GetMatrixInverse().TransformedPoint(point);
}

Vector3 Transformation::FromWorldToLocalVector(const Vector3 &vector) const
{
    return GetMatrixInverse().TransformedVector(vector);
}

Vector3 Transformation::FromWorldToLocalDirection(const Vector3 &dir) const
{
    return GetRotation().Inversed() * dir;
}

const Vector3 &Transformation::GetPosition() const
{
    return m_position;
}

const Quaternion &Transformation::GetRotation() const
{
    return m_rotation;
}

const Vector3 &Transformation::GetScale() const
{
    return m_scale;
}

Vector3 Transformation::GetForward() const
{
    return FromLocalToWorldDirection(Vector3::Forward());
}

Vector3 Transformation::GetBack() const
{
    return FromLocalToWorldDirection(Vector3::Back());
}

Vector3 Transformation::GetRight() const
{
    return FromLocalToWorldDirection(Vector3::Right());
}

Vector3 Transformation::GetLeft() const
{
    return FromLocalToWorldDirection(Vector3::Left());
}

Vector3 Transformation::GetUp() const
{
    return FromLocalToWorldDirection(Vector3::Up());
}

Vector3 Transformation::GetDown() const
{
    return FromLocalToWorldDirection(Vector3::Down());
}

void Transformation::CalculateLocalToWorldMatrixIfNeeded() const
{
    if (!m_validLocalToWorldMatrix)
    {
        m_localToWorldMatrix =
            Matrix4::TransformMatrix(GetPosition(), GetRotation(), GetScale());
        m_validLocalToWorldMatrix = true;
    }
}

void Transformation::CalculateWorldToLocalMatrixIfNeeded() const
{
    if (!m_validWorldToLocalMatrix)
    {
        m_worldToLocalMatrix = Matrix4::TransformMatrixInverse(
            GetPosition(), GetRotation(), GetScale());
        m_validWorldToLocalMatrix = true;
    }
}
