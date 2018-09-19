#include "Bang/Collider.h"

#include "PxPhysicsAPI.h"

#include "Bang/Physics.h"
#include "Bang/MetaNode.h"
#include "Bang/Resources.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/MaterialFactory.h"
#include "Bang/PxSceneContainer.h"

USING_NAMESPACE_BANG

Collider::Collider()
{
    CONSTRUCT_CLASS_ID(Collider)
    SetPhysicsMaterial( MaterialFactory::GetDefaultPhysicsMaterial().Get() );
}

Collider::~Collider()
{
}

void Collider::OnUpdate()
{
    Component::OnUpdate();
    UpdatePxShape();
}

void Collider::SetIsTrigger(bool isTrigger)
{
    if (isTrigger != GetIsTrigger())
    {
        m_isTrigger = isTrigger;
        UpdatePxShape();
    }
}

void Collider::SetCenter(const Vector3 &center)
{
    if (center != GetCenter())
    {
        m_center = center;
        UpdatePxShape();
    }
}

void Collider::SetPhysicsMaterial(PhysicsMaterial *physicsMaterial)
{
    if (physicsMaterial != GetSharedPhysicsMaterial())
    {
        if (p_physicsMaterial.Get())
        {
            p_physicsMaterial.Set(nullptr);
        }

        p_sharedPhysicsMaterial.Set(physicsMaterial);
        UpdatePxShape();
    }
}

bool Collider::GetIsTrigger() const
{
    return m_isTrigger;
}

const Vector3 &Collider::GetCenter() const
{
    return m_center;
}

PhysicsMaterial *Collider::GetSharedPhysicsMaterial() const
{
    return p_sharedPhysicsMaterial.Get();
}

PhysicsMaterial *Collider::GetActivePhysicsMaterial() const
{
    if (p_physicsMaterial.Get())
    {
        return GetPhysicsMaterial();
    }
    return GetSharedPhysicsMaterial();
}

PhysicsMaterial *Collider::GetPhysicsMaterial() const
{
    if (!p_physicsMaterial)
    {
        if (GetSharedPhysicsMaterial())
        {
            p_physicsMaterial = Resources::Clone<PhysicsMaterial>(
                                        GetSharedPhysicsMaterial());
        }
    }
    return p_physicsMaterial.Get();
}

void Collider::OnEnabled(Object *)
{
    if (GetPxShape())
    {
        GetPxShape()->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !GetIsTrigger());
        GetPxShape()->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, GetIsTrigger());
        GetPxShape()->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, !GetIsTrigger());
    }
}

void Collider::OnDisabled(Object *)
{
    if (GetPxShape())
    {
        GetPxShape()->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
        GetPxShape()->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
        GetPxShape()->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
    }
}

void Collider::SetPxRigidBody(physx::PxRigidBody *pxRB)
{
    p_pxRigidBody = pxRB;
}

void Collider::SetPxShape(physx::PxShape *pxShape)
{
    p_pxShape = pxShape;
}

physx::PxRigidBody *Collider::GetPxRigidBody() const
{
    return p_pxRigidBody;
}

physx::PxShape *Collider::GetPxShape() const
{
    return p_pxShape;
}

Quaternion Collider::GetInternalRotation() const
{
    return Quaternion::Identity;
}

Matrix4 Collider::GetShapeTransformWithRespectToPxActor() const
{
    Matrix4 shapeTransformWithRespectToPxActor =
            GetGameObject()->GetTransform()->GetLocalToWorldMatrix();
    if (physx::PxActor *pxActor = GetPxRigidBody())
    {
        Physics *ph = Physics::GetInstance();
        if (PxSceneContainer *pxSceneCont = ph->GetPxSceneContainerFromScene(
                                                GetGameObject()->GetScene() ))
        {
            if (GameObject *pxActorGo = pxSceneCont->GetGameObjectFromPxActor(pxActor))
            {
                shapeTransformWithRespectToPxActor =
                        pxActorGo->GetTransform()->GetLocalToWorldMatrixInv() *
                        shapeTransformWithRespectToPxActor;
            }
        }
    }
    return shapeTransformWithRespectToPxActor;
}

void Collider::UpdatePxShape()
{
    if (GetPxShape())
    {
        Vector3 shapeLocalPosFromPxActor = GetCenter();
        Quaternion shapeLocalRotFromPxActor = GetInternalRotation();

        Matrix4 shapeTransformWithRespectToPxActor =
                                    GetShapeTransformWithRespectToPxActor();
        shapeLocalPosFromPxActor += shapeTransformWithRespectToPxActor.GetTranslation();
        shapeLocalRotFromPxActor = shapeTransformWithRespectToPxActor.GetRotation() *
                                   shapeLocalRotFromPxActor;

        physx::PxTransform pxLocalTransform = GetPxShape()->getLocalPose();
        pxLocalTransform.p = Physics::GetPxVec3FromVector3( shapeLocalPosFromPxActor );
        pxLocalTransform.q = Physics::GetPxQuatFromQuaternion(
                                                    shapeLocalRotFromPxActor );
        GetPxShape()->setLocalPose( pxLocalTransform );

        GetPxShape()->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !GetIsTrigger());
        GetPxShape()->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, GetIsTrigger());

        if (GetActivePhysicsMaterial())
        {
            physx::PxMaterial *material = GetActivePhysicsMaterial()->
                                          GetPxMaterial();
            GetPxShape()->setMaterials(&material, 1);
        }

        physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidBody(), 1.0f);
    }
}

void Collider::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);

    Collider *colliderClone = SCAST<Collider*>(clone);
    colliderClone->SetCenter( GetCenter() );
    colliderClone->SetIsTrigger( GetIsTrigger() );
    colliderClone->SetPhysicsMaterial( GetSharedPhysicsMaterial() );
}

void Collider::ImportMeta(const MetaNode &metaNode)
{
    Component::ImportMeta(metaNode);

    if (metaNode.Contains("IsTrigger"))
    {
        SetIsTrigger( metaNode.Get<bool>("IsTrigger") );
    }

    if (metaNode.Contains("Center"))
    {
        SetCenter( metaNode.Get<Vector3>("Center") );
    }

    if (metaNode.Contains("PhysicsMaterial"))
    {
        RH<PhysicsMaterial> phMat = Resources::Load<PhysicsMaterial>(
                                      metaNode.Get<GUID>("PhysicsMaterial"));
        SetPhysicsMaterial(phMat.Get());
    }
}

void Collider::ExportMeta(MetaNode *metaNode) const
{
    Component::ExportMeta(metaNode);

    metaNode->Set("Center", GetCenter());
    metaNode->Set("IsTrigger", GetIsTrigger());
    metaNode->Set("PhysicsMaterial",
                 GetSharedPhysicsMaterial() ?
                     GetSharedPhysicsMaterial()->GetGUID() : GUID::Empty());
}

