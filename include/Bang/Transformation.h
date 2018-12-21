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
    virtual ~Transformation();

    const Matrix4 &GetLocalToWorldMatrix() const;
    const Matrix4 &GetWorldToLocalMatrix() const;
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
}

#endif  // TRANSFORMATION_H
