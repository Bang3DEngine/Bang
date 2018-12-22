#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "Bang/Bang.h"
#include "Bang/Matrix4.h"
#include "Bang/Quaternion.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Transformation
{
public:
    Transformation();
    Transformation(const Vector3 &position,
                   const Quaternion &rotation,
                   const Vector3 &scale);
    explicit Transformation(const Matrix4 &transformationMatrix);

    static const Transformation &Identity();

    Transformation Inversed() const;
    const Matrix4 &GetMatrix() const;
    const Matrix4 &GetMatrixInverse() const;
    void FillFromMatrix(const Matrix4 &transformMatrix);

    void SetPosition(const Vector3 &position);
    void SetRotation(const Quaternion &rotation);
    void SetScale(const Vector3 &scale);
    void LookAt(const Vector3 &target, const Vector3 &up = Vector3::Up());
    void LookAt(Transform *targetTransform, const Vector3 &up = Vector3::Up());
    void LookAt(GameObject *target, const Vector3 &up = Vector3::Up());
    void LookInDirection(const Vector3 &dir, const Vector3 &up = Vector3::Up());

    void Translate(const Vector3 &translation);
    void Rotate(const Quaternion &rotation);
    void Scale(const Vector3 &scale);

    static Transformation Composed(const Transformation &lhs,
                                   const Transformation &rhs);
    Vector3 TransformedPoint(const Vector3 &point);
    Vector3 TransformedVector(const Vector3 &vector);

    Vector3 FromLocalToWorldPoint(const Vector3 &point) const;
    Vector3 FromLocalToWorldVector(const Vector3 &vector) const;
    Vector3 FromLocalToWorldDirection(const Vector3 &dir) const;
    Vector3 FromWorldToLocalPoint(const Vector3 &point) const;
    Vector3 FromWorldToLocalVector(const Vector3 &vector) const;
    Vector3 FromWorldToLocalDirection(const Vector3 &dir) const;

    const Vector3 &GetPosition() const;
    const Quaternion &GetRotation() const;
    const Vector3 &GetScale() const;
    Vector3 GetForward() const;
    Vector3 GetBack() const;
    Vector3 GetRight() const;
    Vector3 GetLeft() const;
    Vector3 GetUp() const;
    Vector3 GetDown() const;

private:
    Vector3 m_position = Vector3::Zero();
    Quaternion m_rotation = Quaternion::Identity();
    Vector3 m_scale = Vector3::One();

    mutable Matrix4 m_localToWorldMatrix;
    mutable Matrix4 m_worldToLocalMatrix;
    mutable bool m_validLocalToWorldMatrix = false;
    mutable bool m_validWorldToLocalMatrix = false;

    void CalculateLocalToWorldMatrixIfNeeded() const;
    void CalculateWorldToLocalMatrixIfNeeded() const;
};

inline bool operator==(const Transformation &lhs, const Transformation &rhs)
{
    return (lhs.GetPosition() == rhs.GetPosition()) &&
           (lhs.GetRotation() == rhs.GetRotation()) &&
           (lhs.GetScale() == rhs.GetScale());
}

inline bool operator!=(const Transformation &lhs, const Transformation &rhs)
{
    return !(lhs == rhs);
}

inline Transformation operator*(const Transformation &lhs,
                                const Transformation &rhs)
{
    return Transformation::Composed(lhs, rhs);
}

template <class T>
inline Vector4G<T> operator*(const Transformation &tr, const Vector4G<T> &v)
{
    return tr.GetMatrix() * v;
}

inline void operator*=(Transformation &lhs, const Transformation &rhs)
{
    lhs = (lhs * rhs);
}
}

#endif  // TRANSFORMATION_H
