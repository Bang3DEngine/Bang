#include "Bang/Collider.h"

#include "Bang/Assets.h"
#include "Bang/Assets.tcc"
#include "Bang/Extensions.h"
#include "Bang/GUID.h"
#include "Bang/GameObject.h"
#include "Bang/Matrix4.tcc"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/PhysicsMaterial.h"
#include "Bang/PxSceneContainer.h"
#include "Bang/Quaternion.h"
#include "Bang/Transform.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "extensions/PxRigidBodyExt.h"
#include "foundation/PxQuat.h"
#include "foundation/PxTransform.h"
#include "foundation/PxVec3.h"

namespace Bang
{
class ICloneable;
class Object;
}

namespace physx
{
class PxActor;
class PxMaterial;
}

using namespace Bang;

Collider::Collider()
{
    SET_INSTANCE_CLASS_ID(Collider)
}

Collider::~Collider()
{
    if (GetPxShape())
    {
        GetPxShape()->release();
    }
}

void Collider::OnUpdate()
{
    PhysicsComponent::OnUpdate();
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

void Collider::SetUseForQueries(bool useForQueries)
{
    m_useForQueries = useForQueries;
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

void Collider::SetUseInNavMesh(bool useInNavMesh)
{
    m_useInNavMesh = useInNavMesh;
}

bool Collider::GetIsTrigger() const
{
    return m_isTrigger;
}

bool Collider::GetUseInNavMesh() const
{
    return m_useInNavMesh;
}

bool Collider::GetUseForQueries() const
{
    return m_useForQueries;
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
            p_physicsMaterial =
                Assets::Clone<PhysicsMaterial>(GetSharedPhysicsMaterial());
        }
    }
    return p_physicsMaterial.Get();
}

bool Collider::CanComputeInertia() const
{
    return true;
}

bool Collider::CanBeSimulationShape() const
{
    return true;
}

bool Collider::CanBeTriggerShape() const
{
    return true;
}

void Collider::OnPxRigidActorChanged(physx::PxRigidActor *prevPxRigidActor,
                                     physx::PxRigidActor *newPxRigidActor)
{
    if (prevPxRigidActor)
    {
        if (GetPxShape())
        {
            prevPxRigidActor->detachShape(*GetPxShape());
            p_pxShape = nullptr;
        }
    }

    if (newPxRigidActor)
    {
        UpdatePxShape();
    }
}

physx::PxShape *Collider::GetPxShape() const
{
    return p_pxShape;
}

void Collider::SetPxEnabled(bool pxEnabled)
{
    GetPxShape()->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE,
                          pxEnabled && GetUseForQueries());

    GetPxShape()->setFlag(
        physx::PxShapeFlag::eSIMULATION_SHAPE,
        pxEnabled && CanBeSimulationShape() && !GetIsTrigger());

    GetPxShape()->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE,
                          pxEnabled && CanBeTriggerShape() && GetIsTrigger());
}

Quaternion Collider::GetInternalRotation() const
{
    return Quaternion::Identity();
}

void Collider::UpdatePxShape()
{
    if (!GetPxShape())
    {
        if (GetPxRigidActor())
        {
            p_pxShape = CreatePxShape();
        }
    }

    if (GetPxShape())
    {
        physx::PxTransform pxLocalTransform =
            GetWorldPxTransformWithRespectToPxActor();
        GetPxShape()->setLocalPose(pxLocalTransform);

        SetPxEnabled(IsEnabledRecursively());

        if (GetActivePhysicsMaterial())
        {
            physx::PxMaterial *material =
                GetActivePhysicsMaterial()->GetPxMaterial();
            GetPxShape()->setMaterials(&material, 1);
        }

        if (CanComputeInertia() && GetPxRigidDynamic())
        {
            physx::PxRigidBodyExt::updateMassAndInertia(*GetPxRigidDynamic(),
                                                        1.0f);
        }
    }
}

void Collider::Reflect()
{
    Component::Reflect();

    BANG_REFLECT_VAR_MEMBER(Collider, "Is Trigger", SetIsTrigger, GetIsTrigger);
    BANG_REFLECT_VAR_MEMBER(
        Collider, "Use for queries", SetUseForQueries, GetUseForQueries);
    BANG_REFLECT_VAR_MEMBER(
        Collider, "Use in NavMesh", SetUseInNavMesh, GetUseInNavMesh);
    BANG_REFLECT_VAR_MEMBER(Collider, "Center", SetCenter, GetCenter);

    BANG_REFLECT_VAR_ASSET("Physics Material",
                           SetPhysicsMaterial,
                           GetSharedPhysicsMaterial,
                           PhysicsMaterial,
                           BANG_REFLECT_HINT_EXTENSIONS(
                               Extensions::GetPhysicsMaterialExtension()));
}

physx::PxTransform Collider::GetWorldPxTransform() const
{
    Matrix4 worldTransform = GetWorldShapeTransform();
    return Physics::GetPxTransformFromMatrix(worldTransform);
}

physx::PxTransform Collider::GetWorldPxTransformWithRespectToPxActor() const
{
    Matrix4 worldTransformWithRespectToActor =
        GetWorldShapeTransformWithRespectToPxActor();
    return Physics::GetPxTransformFromMatrix(worldTransformWithRespectToActor);
}

Matrix4 Collider::GetWorldShapeTransform() const
{
    Matrix4 worldShapeTransform =
        GetGameObject()->GetTransform()->GetLocalToWorldMatrix() *
        GetLocalShapeTransform();
    return worldShapeTransform;
}

Matrix4 Collider::GetWorldShapeTransformWithRespectToPxActor() const
{
    Matrix4 shapeTransformWithRespectToPxActor = GetWorldShapeTransform();
    if (physx::PxRigidActor *pxRA = GetPxRigidActor())
    {
        Physics *ph = Physics::GetInstance();
        if (PxSceneContainer *pxSceneCont =
                ph->GetPxSceneContainerFromScene(GetGameObject()->GetScene()))
        {
            if (GameObject *pxActorGo =
                    pxSceneCont->GetGameObjectFromPxActor(pxRA))
            {
                Matrix4 actorToWorldInv =
                    pxActorGo->GetTransform()->GetWorldToLocalMatrix();
                shapeTransformWithRespectToPxActor =
                    actorToWorldInv * shapeTransformWithRespectToPxActor;
            }
        }
    }
    return shapeTransformWithRespectToPxActor;
}

Matrix4 Collider::GetLocalShapeTransform() const
{
    Matrix4 localShapeTransform = Matrix4::TranslateMatrix(GetCenter()) *
                                  Matrix4::RotateMatrix(GetInternalRotation());
    return localShapeTransform;
}
