#include "Bang/Transform.h"

#include <sstream>
#include <ostream>

#include "Bang/MetaNode.h"
#include "Bang/GameObject.h"
#include "Bang/IEventsTransform.h"

USING_NAMESPACE_BANG

Transform::Transform()
{
    CONSTRUCT_CLASS_ID(Transform);
}

Transform::~Transform()
{
}

void Transform::SetLocalPosition(const Vector3 &p)
{
    if (GetLocalPosition() != p)
    {
        m_localPosition = p;
        InvalidateTransform();
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
        m_localRotation = q.Normalized();
        m_localEulerAnglesDegreesHint = GetLocalRotation().GetEulerAnglesDegrees();
        InvalidateTransform();
    }
}
void Transform::SetLocalEuler(const Vector3 &degreesEuler)
{
    Vector3 eulersRads = degreesEuler.ToRadians();
    SetLocalRotation( Quaternion::FromEulerAnglesRads(eulersRads) );
    m_localEulerAnglesDegreesHint = degreesEuler;
}
void Transform::SetLocalEuler(float x, float y, float z)
{
    SetLocalEuler(Vector3(x,y,z));
}

void Transform::SetRotation(const Quaternion &q)
{
    if (GameObject *parent = GetGameObject()->GetParent())
    {
        ASSERT(parent->GetTransform());
        SetLocalRotation( Quaternion(
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
    SetEuler(Vector3(x,y,z));
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
    Vector3 parentScale = p ? p->GetTransform()->GetScale() : Vector3::One;
    parentScale = Vector3::Max( Vector3(0.0001f), parentScale );
    SetLocalScale(1.0f / parentScale * v);
}

void Transform::SetLocalScale(float s)
{
    SetLocalScale(Vector3(s));
}

void Transform::SetLocalScale(const Vector3 &s)
{
    if (GetLocalScale() != s)
    {
        m_localScale = s;
        InvalidateTransform();
    }
}

Vector3 Transform::TransformPoint(const Vector3 &point) const
{
    return (GetLocalToWorldMatrix() * Vector4(point, 1)).xyz();
}
Vector3 Transform::InverseTransformPoint(const Vector3 &point) const
{
    return (GetLocalToWorldMatrixInv() * Vector4(point, 1)).xyz();
}
Vector3 Transform::TransformDirection(const Vector3 &dir) const
{
    return GetRotation() * dir;
}
Vector3 Transform::InverseTransformDirection(const Vector3 &dir) const
{
    return GetRotation().Inversed() * dir;
}
Vector3 Transform::TransformVector(const Vector3 &dir) const
{
    return (GetLocalToWorldMatrix() * Vector4(dir, 0)).xyz();
}
Vector3 Transform::InverseTransformVector(const Vector3 &dir) const
{
    return (GetLocalToWorldMatrixInv() * Vector4(dir, 0)).xyz();
}

Vector3 Transform::FromLocalToWorldPoint(const Vector3 &point) const
{
    return TransformPoint(point);
}
Vector3 Transform::FromLocalToWorldVector(const Vector3 &dir) const
{
    return TransformVector(dir);
}
Vector3 Transform::FromLocalToWorldDirection(const Vector3 &dir) const
{
    return TransformDirection(dir);
}

Vector3 Transform::FromWorldToLocalPoint(const Vector3 &point) const
{
    return InverseTransformPoint(point);
}
Vector3 Transform::FromWorldToLocalVector(const Vector3 &dir) const
{
    return InverseTransformVector(dir);
}

Vector3 Transform::FromWorldToLocalDirection(const Vector3 &dir) const
{
    return InverseTransformDirection(dir);
}

bool Transform::CanBeRepeatedInGameObject() const { return false; }

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
    Matrix4 T  = Matrix4::TranslateMatrix(GetLocalPosition());
    Matrix4 R  = Matrix4::RotateMatrix(GetLocalRotation());
    Matrix4 S  = Matrix4::ScaleMatrix(GetLocalScale());

    m_localToParentMatrix = (T * R * S);
    m_localToParentMatrixInv = m_localToParentMatrix.Inversed();
}

void Transform::CalculateLocalToWorldMatrix() const
{
    m_localToWorldMatrix = GetLocalToParentMatrix();
    GameObject *parent = GetGameObject()->GetParent();
    if (parent && parent->GetTransform())
    {
        const Matrix4 &mp = parent->GetTransform()->Transform::GetLocalToWorldMatrix();
        m_localToWorldMatrix = mp * m_localToWorldMatrix;
    }
    m_localToWorldMatrixInv = m_localToWorldMatrix.Inversed();
}

const Matrix4 &Transform::GetLocalToParentMatrix() const
{
    RecalculateParentMatricesIfNeeded();
    return m_localToParentMatrix;
}

const Matrix4 &Transform::GetLocalToParentMatrixInv() const
{
    RecalculateParentMatricesIfNeeded();
    return m_localToParentMatrixInv;
}

const Matrix4& Transform::GetLocalToWorldMatrix() const
{
    RecalculateWorldMatricesIfNeeded();
    return m_localToWorldMatrix;
}

const Matrix4 &Transform::GetLocalToWorldMatrixInv() const
{
    RecalculateWorldMatricesIfNeeded();
    return m_localToWorldMatrixInv;
}

void Transform::LookAt(const Vector3 &target, const Vector3 &_up)
{
    if(target == GetLocalPosition()) { return; }
    Vector3 up = _up.Normalized();
    SetRotation(Quaternion::LookDirection(target - GetPosition(), up) );
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
    LookAt(GetPosition() + dir * 99.0f, up);
}

void Transform::FillFromMatrix(const Matrix4 &transformMatrix)
{
    Vector3 localPos    = transformMatrix.GetTranslation();
    Quaternion localRot = transformMatrix.GetRotation();
    Vector3 localScale  = transformMatrix.GetScale();
    SetLocalPosition( localPos   );
    SetLocalRotation( localRot   );
    SetLocalScale   ( localScale );
}

const Vector3& Transform::GetLocalPosition() const
{
    return m_localPosition;
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

const Quaternion& Transform::GetLocalRotation() const
{
    return m_localRotation;
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

Vector3 Transform::GetLocalEuler() const
{
    return m_localEulerAnglesDegreesHint;
}

Vector3 Transform::GetEuler() const
{
    return FromLocalToWorldDirection( GetLocalEuler() );
}

const Vector3& Transform::GetLocalScale() const
{
    return m_localScale;
}

Vector3 Transform::GetScale() const
{
    GameObject *parent = GetGameObject()->GetParent();
    Transform *parentTr = (parent ? parent->GetTransform() : nullptr);
    Vector3 parentScale = parentTr ? parentTr->GetScale() : Vector3::One;
    return parentScale * GetLocalScale();
}

Vector3 Transform::GetForward() const
{
    return FromLocalToWorldDirection(Vector3::Forward).Normalized();
}

Vector3 Transform::GetBack() const
{
    return -GetForward();
}

Vector3 Transform::GetRight() const
{
    return  FromLocalToWorldDirection(Vector3::Right).Normalized();
}

Vector3 Transform::GetLeft() const
{
    return -GetRight();
}

Vector3 Transform::GetUp() const
{
    return FromLocalToWorldDirection(Vector3::Up).Normalized();
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
void Transform::OnParentChanged(GameObject*, GameObject*)
{
    OnParentTransformChanged();
}
void Transform::OnTransformChanged()
{
    InvalidateTransform();

    GameObject *go = GetGameObject();
    if (!go)
    {
        return;
    }

    EventEmitter<IEventsTransform>::
       PropagateToListeners(&EventListener<IEventsTransform>::OnTransformChanged);

    EventListener<IEventsTransform>::SetReceiveEvents(false);

    go->PropagateToArray(&EventListener<IEventsTransform>::OnTransformChanged,
                  go->GetComponents<EventListener<IEventsTransform>>());

    EventListener<IEventsTransform>::SetReceiveEvents(true);

    PropagateParentTransformChangedEventToChildren();
    PropagateChildrenTransformChangedEventToParent();
}

void Transform::PropagateParentTransformChangedEventToChildren() const
{
    GameObject *go = GetGameObject();
    EventEmitter<IEventsTransform>::
           PropagateToListeners(
                &EventListener<IEventsTransform>::OnParentTransformChanged);
    go->PropagateToArray(
        &EventListener<IEventsTransform>::OnParentTransformChanged,
              go->GetComponentsInChildren<EventListener<IEventsTransform>>());
}

void Transform::PropagateChildrenTransformChangedEventToParent() const
{
    GameObject *go = GetGameObject();
    EventEmitter<IEventsTransform>::
           PropagateToListeners(
                &EventListener<IEventsTransform>::OnChildrenTransformChanged);
    go->PropagateToArray(
        &EventListener<IEventsTransform>::OnChildrenTransformChanged,
              go->GetComponentsInParent<EventListener<IEventsTransform>>());
}

void Transform::OnParentTransformChanged()
{
    InvalidateTransform();
    PropagateParentTransformChangedEventToChildren();
}
void Transform::OnChildrenTransformChanged() {}


void Transform::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Transform *t = Cast<Transform*>(clone);
    t->SetLocalPosition(GetLocalPosition());
    t->SetLocalRotation(GetLocalRotation());
    t->SetLocalScale(GetLocalScale());
}

void Transform::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("Position"))
    {
        SetLocalPosition(metaNode.Get<Vector3>("Position"));
    }

    if (metaNode.Contains("Rotation"))
    {
        SetLocalRotation(metaNode.Get<Quaternion>("Rotation"));
    }

    if (metaNode.Contains("Scale"))
    {
        SetLocalScale(metaNode.Get<Vector3>("Scale"));
    }
}

void Transform::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);
    metaNode->Set("Position", GetLocalPosition());
    metaNode->Set("Rotation", GetLocalRotation());
    metaNode->Set("Scale",    GetLocalScale());
}

void Transform::InvalidateTransform()
{
    IInvalidatableTransformLocal::Invalidate();
    IInvalidatableTransformWorld::Invalidate();
}

