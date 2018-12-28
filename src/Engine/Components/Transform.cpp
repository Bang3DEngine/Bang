#include "Bang/Transform.h"

#include "Bang/Assert.h"
#include "Bang/ClassDB.h"
#include "Bang/EventListener.tcc"
#include "Bang/GameObject.h"
#include "Bang/GameObject.tcc"
#include "Bang/ICloneable.h"
#include "Bang/IEventsChildren.h"
#include "Bang/IEventsTransform.h"
#include "Bang/Matrix4.h"
#include "Bang/Matrix4.tcc"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Quaternion.h"
#include "Bang/StreamOperators.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

using namespace Bang;

Transform::Transform()
{
    SET_INSTANCE_CLASS_ID(Transform);
}

Transform::~Transform()
{
}

void Transform::SetLocalPosition(const Vector3 &p)
{
    if (GetLocalPosition() != p)
    {
        m_localTransformation.SetPosition(p);
        OnTransformChanged();
    }
}
void Transform::SetPosition(const Vector3 &p)
{
    if (GameObject *parent = GetGameObject()->GetParent())
    {
        ASSERT(parent->GetTransform());
        SetLocalPosition(parent->GetTransform()->FromWorldToLocalPoint(p));
    }
    else
    {
        SetLocalPosition(p);
    }
}
void Transform::TranslateLocal(const Vector3 &translation)
{
    SetLocalPosition(GetLocalPosition() + translation);
}
void Transform::Translate(const Vector3 &translation)
{
    SetPosition(GetPosition() + translation);
}

void Transform::SetLocalRotation(const Quaternion &q)
{
    if (GetLocalRotation() != q)
    {
        m_localTransformation.SetRotation(q.Normalized());
        m_localEulerAnglesDegreesHint =
            GetLocalRotation().GetEulerAnglesDegrees();
        OnTransformChanged();
    }
}
void Transform::SetLocalEuler(const Vector3 &degreesEuler)
{
    Vector3 eulersRads = degreesEuler.ToRadians();
    SetLocalRotation(Quaternion::FromEulerAnglesRads(eulersRads));
    m_localEulerAnglesDegreesHint = degreesEuler;
}
void Transform::SetLocalEuler(float x, float y, float z)
{
    SetLocalEuler(Vector3(x, y, z));
}

void Transform::SetRotation(const Quaternion &q)
{
    if (GameObject *parent = GetGameObject()->GetParent())
    {
        ASSERT(parent->GetTransform());
        SetLocalRotation(Quaternion(
            parent->GetTransform()->GetRotation().Inversed() * q.Normalized()));
    }
    else
    {
        SetLocalRotation(q.Normalized());
    }
}
void Transform::SetEuler(const Vector3 &degreesEuler)
{
    if (GameObject *parent = GetGameObject()->GetParent())
    {
        ASSERT(parent->GetTransform());
        SetLocalEuler(-parent->GetTransform()->GetEuler() + degreesEuler);
    }
    else
    {
        SetLocalEuler(degreesEuler);
    }
}
void Transform::SetEuler(float x, float y, float z)
{
    SetEuler(Vector3(x, y, z));
}

void Transform::RotateLocal(const Quaternion &r)
{
    SetLocalRotation(Quaternion(GetLocalRotation() * r.Normalized()));
}
void Transform::RotateLocalEuler(const Vector3 &degreesEuler)
{
    SetLocalEuler(GetLocalEuler() + degreesEuler);
}
void Transform::Rotate(const Quaternion &r)
{
    SetRotation(Quaternion(r.Normalized() * GetRotation()));
}
void Transform::RotateEuler(const Vector3 &degreesEuler)
{
    SetEuler(GetEuler() + degreesEuler);
}

void Transform::SetScale(float s)
{
    SetScale(Vector3(s));
}

void Transform::SetScale(const Vector3 &v)
{
    GameObject *p = GetGameObject()->GetParent();
    Vector3 parentScale = p ? p->GetTransform()->GetScale() : Vector3::One();
    parentScale = Vector3::Max(Vector3(0.0001f), parentScale);
    SetLocalScale(1.0f / parentScale * v);
}

void Transform::SetLocalScale(float s)
{
    SetLocalScale(Vector3(s));
}

void Transform::SetLocalScale(const Vector3 &localScale)
{
    if (GetLocalScale() != localScale)
    {
        m_localTransformation.SetScale(localScale);
        OnTransformChanged();
    }
}

Vector3 Transform::FromLocalToWorldPoint(const Vector3 &point) const
{
    return GetLocalToWorldMatrix().TransformedPoint(point);
}
Vector3 Transform::FromLocalToWorldVector(const Vector3 &dir) const
{
    return GetLocalToWorldMatrix().TransformedVector(dir);
}
Vector3 Transform::FromLocalToWorldDirection(const Vector3 &dir) const
{
    return GetRotation() * dir;
}

Vector3 Transform::FromWorldToLocalPoint(const Vector3 &point) const
{
    return GetWorldToLocalMatrix().TransformedPoint(point);
}

Vector3 Transform::FromWorldToLocalVector(const Vector3 &dir) const
{
    return GetWorldToLocalMatrix().TransformedVector(dir);
}

Vector3 Transform::FromWorldToLocalDirection(const Vector3 &dir) const
{
    return GetRotation().Inversed() * dir;
}

bool Transform::CanBeRepeatedInGameObject() const
{
    return false;
}

void Transform::RecalculateParentMatricesIfNeeded() const
{
    if (IInvalidatableTransformLocal::IsInvalid())
    {
        CalculateLocalToParentMatrix();
        IInvalidatableTransformLocal::Validate();
    }
}

void Transform::RecalculateWorldMatricesIfNeeded() const
{
    if (IInvalidatableTransformWorld::IsInvalid())
    {
        CalculateLocalToWorldMatrix();
        IInvalidatableTransformWorld::Validate();
    }
}

void Transform::CalculateLocalToParentMatrix() const
{
    m_localToParentMatrix = GetLocalTransformation().GetMatrix();
    m_parentToLocalMatrix = GetLocalTransformation().GetMatrixInverse();
}

void Transform::CalculateLocalToWorldMatrix() const
{
    m_localToWorldMatrix = GetLocalToParentMatrix();
    GameObject *parent = GetGameObject()->GetParent();
    if (parent && parent->GetTransform())
    {
        const Matrix4 &mp =
            parent->GetTransform()->Transform::GetLocalToWorldMatrix();
        m_localToWorldMatrix = mp * m_localToWorldMatrix;
    }
    m_worldToLocalMatrix = m_localToWorldMatrix.Inversed();
}

const Matrix4 &Transform::GetLocalToParentMatrix() const
{
    RecalculateParentMatricesIfNeeded();
    return m_localToParentMatrix;
}

const Matrix4 &Transform::GetParentToLocalMatrix() const
{
    RecalculateParentMatricesIfNeeded();
    return m_parentToLocalMatrix;
}

const Matrix4 &Transform::GetLocalToWorldMatrix() const
{
    RecalculateWorldMatricesIfNeeded();
    return m_localToWorldMatrix;
}

const Matrix4 &Transform::GetWorldToLocalMatrix() const
{
    RecalculateWorldMatricesIfNeeded();
    return m_worldToLocalMatrix;
}

void Transform::LookAt(const Vector3 &target, const Vector3 &up)
{
    m_localTransformation.LookAt(target, up);
}

void Transform::LookAt(Transform *targetTransform, const Vector3 &up)
{
    LookAt(targetTransform->GetPosition(), up);
}

void Transform::LookAt(GameObject *target, const Vector3 &up)
{
    LookAt(target->GetTransform(), up);
}

void Transform::LookInDirection(const Vector3 &dir, const Vector3 &up)
{
    m_localTransformation.LookAt(dir, up);
}

void Transform::FillFromMatrix(const Matrix4 &transformMatrix)
{
    Transformation matrixTransformation;
    matrixTransformation.FillFromMatrix(transformMatrix);
    FillFromTransformation(matrixTransformation);
}

void Transform::FillFromTransformation(const Transformation &transformation)
{
    m_localTransformation.SetPosition(transformation.GetPosition());
    m_localTransformation.SetRotation(transformation.GetRotation());
    m_localTransformation.SetScale(transformation.GetScale());
    OnTransformChanged();
}

const Vector3 &Transform::GetLocalPosition() const
{
    return GetLocalTransformation().GetPosition();
}

Vector3 Transform::GetPosition() const
{
    if (GameObject *parent = GetGameObject()->GetParent())
    {
        if (Transform *tr = parent->GetTransform())
        {
            return tr->FromLocalToWorldPoint(GetLocalPosition());
        }
    }
    return GetLocalPosition();
}

const Quaternion &Transform::GetLocalRotation() const
{
    return GetLocalTransformation().GetRotation();
}

Quaternion Transform::GetRotation() const
{
    if (GameObject *parent = GetGameObject()->GetParent())
    {
        if (Transform *tr = parent->GetTransform())
        {
            return tr->GetRotation() * GetLocalRotation();
        }
    }
    return GetLocalRotation();
}

const Vector3 &Transform::GetLocalEuler() const
{
    return m_localEulerAnglesDegreesHint;
}

Vector3 Transform::GetEuler() const
{
    return FromLocalToWorldDirection(GetLocalEuler());
}

const Vector3 &Transform::GetLocalScale() const
{
    return GetLocalTransformation().GetScale();
}

Vector3 Transform::GetScale() const
{
    GameObject *parent = GetGameObject()->GetParent();
    Transform *parentTr = (parent ? parent->GetTransform() : nullptr);
    Vector3 parentScale = parentTr ? parentTr->GetScale() : Vector3::One();
    return parentScale * GetLocalScale();
}

const Transformation &Transform::GetLocalTransformation() const
{
    return m_localTransformation;
}

Vector3 Transform::GetForward() const
{
    Vector3 forward = FromLocalToWorldDirection(Vector3::Forward());
    return forward;
}

Vector3 Transform::GetBack() const
{
    return -GetForward();
}

Vector3 Transform::GetRight() const
{
    Vector3 right = FromLocalToWorldDirection(Vector3::Right());
    return right;
}

Vector3 Transform::GetLeft() const
{
    return -GetRight();
}

Vector3 Transform::GetUp() const
{
    Vector3 up = FromLocalToWorldDirection(Vector3::Up());
    return up;
}

Vector3 Transform::GetDown() const
{
    return -GetUp();
}

void Transform::OnInvalidatedWorld()
{
    OnTransformInvalidated();
}
void Transform::OnInvalidatedLocal()
{
    OnTransformInvalidated();
}
void Transform::OnTransformInvalidated()
{
    OnTransformChanged();
}
void Transform::OnParentChanged(GameObject *, GameObject *)
{
    OnParentTransformChanged();
}
void Transform::OnTransformChanged()
{
    InvalidateTransform();
    m_alreadyNotifiedChildrenThatTransformHasChanged = false;

    if (GameObject *go = GetGameObject())
    {
        EventListener<IEventsTransform>::SetReceiveEvents(false);

        EventEmitter<IEventsTransform>::PropagateToListeners(
            &EventListener<IEventsTransform>::OnTransformChanged);

        EventEmitter<IEventsTransform>::PropagateToArray(
            go->GetComponents<EventListener<IEventsTransform>>(),
            &EventListener<IEventsTransform>::OnTransformChanged);

        EventListener<IEventsTransform>::SetReceiveEvents(true);

        PropagateParentTransformChangedEventToChildren();
    }
}

void Transform::PropagateParentTransformChangedEventToChildren() const
{
    if (!m_alreadyNotifiedChildrenThatTransformHasChanged)
    {
        m_alreadyNotifiedChildrenThatTransformHasChanged = true;

        GameObject *go = GetGameObject();
        EventEmitter<IEventsTransform>::PropagateToListeners(
            &EventListener<IEventsTransform>::OnParentTransformChanged);
        EventEmitter<IEventsTransform>::PropagateToArray(
            go->GetComponentsInChildren<EventListener<IEventsTransform>>(),
            &EventListener<IEventsTransform>::OnParentTransformChanged);
    }
}

void Transform::OnParentTransformChanged()
{
    InvalidateTransform();
    PropagateParentTransformChangedEventToChildren();
}

void Transform::OnChildrenTransformChanged()
{
}

void Transform::Reflect()
{
    Component::Reflect();

    BANG_REFLECT_VAR_MEMBER(
        Transform, "Position", SetLocalPosition, GetLocalPosition);
    BANG_REFLECT_VAR_MEMBER(
        Transform, "Rotation", SetLocalEuler, GetLocalEuler);
    BANG_REFLECT_VAR_MEMBER_HINTED(Transform,
                                   "RotationQuaternion",
                                   SetLocalRotation,
                                   GetLocalRotation,
                                   BANG_REFLECT_HINT_SHOWN(false));
    BANG_REFLECT_VAR_MEMBER(Transform, "Scale", SetLocalScale, GetLocalScale);
}

void Transform::InvalidateTransform()
{
    IInvalidatableTransformLocal::Invalidate();
    IInvalidatableTransformWorld::Invalidate();
}
