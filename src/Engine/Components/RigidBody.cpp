#include "Bang/RigidBody.h"

#include <istream>

#include "Bang/ClassDB.h"
#include "Bang/Debug.h"
#include "Bang/GameObject.h"
#include "Bang/MetaNode.h"
#include "Bang/MetaNode.tcc"
#include "Bang/Physics.h"
#include "Bang/StreamOperators.h"
#include "PxActor.h"
#include "PxRigidBody.h"
#include "extensions/PxRigidBodyExt.h"

namespace Bang
{
class ICloneable;
}

using namespace Bang;
using namespace physx;

RigidBody::RigidBody()
{
    SET_INSTANCE_CLASS_ID(RigidBody)
    SetPhysicsComponentType(PhysicsComponent::Type::RIGIDBODY);

    // Create pxActor
    if (Physics *ph = Physics::GetInstance())
    {
        SetPxRigidActor(ph->CreateNewPxRigidActor(false));
        SetIsKinematic(false);
    }
}

RigidBody::~RigidBody()
{
}

void RigidBody::SetMass(float mass)
{
    if (mass != GetMass())
    {
        m_mass = mass;
        UpdatePxRigidActorValues();
    }
}

void RigidBody::SetDrag(float drag)
{
    if (drag != GetDrag())
    {
        m_drag = drag;
        UpdatePxRigidActorValues();
    }
}

void RigidBody::SetAngularDrag(float angularDrag)
{
    if (angularDrag != GetAngularDrag())
    {
        m_angularDrag = angularDrag;
        UpdatePxRigidActorValues();
    }
}

void RigidBody::SetUseGravity(bool useGravity)
{
    if (useGravity != GetUseGravity())
    {
        m_useGravity = useGravity;
        UpdatePxRigidActorValues();
    }
}

void RigidBody::SetIsKinematic(bool isKinematic)
{
    if (isKinematic != GetIsKinematic())
    {
        m_isKinematic = isKinematic;
        UpdatePxRigidActorValues();
    }
}

void RigidBody::SetLinearVelocity(const Vector3 &linearVelocity)
{
    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->setLinearVelocity(
            Physics::GetPxVec3FromVector3(linearVelocity));
    }
}

void RigidBody::SetAngularVelocity(const Vector3 &angularVelocity)
{
    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->setAngularVelocity(
            Physics::GetPxVec3FromVector3(angularVelocity));
    }
}

void RigidBody::SetMaxAngularVelocity(float maxAngularVelocity)
{
    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->setMaxAngularVelocity(maxAngularVelocity);
    }
}

void RigidBody::SetConstraints(const RigidBodyConstraints &constraints)
{
    if (constraints != GetConstraints())
    {
        m_constraints = constraints;
        UpdatePxRigidActorValues();
    }
}

void RigidBody::AddForce(const Vector3 &force, ForceMode forceMode)
{
    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->addForce(
            Physics::GetPxVec3FromVector3(force),
            SCAST<physx::PxForceMode::Enum>(forceMode));
    }
}

void RigidBody::AddTorque(const Vector3 &torque, ForceMode forceMode)
{
    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->addTorque(
            Physics::GetPxVec3FromVector3(torque),
            SCAST<physx::PxForceMode::Enum>(forceMode));
    }
}

void RigidBody::AddForceAtPos(const Vector3 &force,
                              const Vector3 &pos,
                              ForceMode forceMode)
{
    if (GetPxRigidDynamic())
    {
        physx::PxRigidBodyExt::addForceAtPos(
            *GetPxRigidDynamic(),
            Physics::GetPxVec3FromVector3(force),
            Physics::GetPxVec3FromVector3(pos),
            SCAST<physx::PxForceMode::Enum>(forceMode));
    }
}

void RigidBody::AddForceAtLocalPos(const Vector3 &force,
                                   const Vector3 &pos,
                                   ForceMode forceMode)
{
    if (GetPxRigidDynamic())
    {
        physx::PxRigidBodyExt::addForceAtLocalPos(
            *GetPxRigidDynamic(),
            Physics::GetPxVec3FromVector3(force),
            Physics::GetPxVec3FromVector3(pos),
            SCAST<physx::PxForceMode::Enum>(forceMode));
    }
}

void RigidBody::AddLocalForceAtPos(const Vector3 &force,
                                   const Vector3 &pos,
                                   ForceMode forceMode)
{
    if (GetPxRigidDynamic())
    {
        physx::PxRigidBodyExt::addLocalForceAtPos(
            *GetPxRigidDynamic(),
            Physics::GetPxVec3FromVector3(force),
            Physics::GetPxVec3FromVector3(pos),
            SCAST<physx::PxForceMode::Enum>(forceMode));
    }
}

void RigidBody::AddLocalForceAtLocalPos(const Vector3 &force,
                                        const Vector3 &pos,
                                        ForceMode forceMode)
{
    if (GetPxRigidDynamic())
    {
        physx::PxRigidBodyExt::addLocalForceAtLocalPos(
            *GetPxRigidDynamic(),
            Physics::GetPxVec3FromVector3(force),
            Physics::GetPxVec3FromVector3(pos),
            SCAST<physx::PxForceMode::Enum>(forceMode));
    }
}

void RigidBody::ClearForce(ForceMode forceMode)
{
    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->clearForce(
            SCAST<physx::PxForceMode::Enum>(forceMode));
    }
}

void RigidBody::ClearTorque(ForceMode forceMode)
{
    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->clearTorque(
            SCAST<physx::PxForceMode::Enum>(forceMode));
    }
}

float RigidBody::GetMass() const
{
    return m_mass;
}

float RigidBody::GetDrag() const
{
    return m_drag;
}

float RigidBody::GetAngularDrag() const
{
    return m_angularDrag;
}

bool RigidBody::GetUseGravity() const
{
    return m_useGravity;
}

bool RigidBody::GetIsKinematic() const
{
    return m_isKinematic;
}

Vector3 RigidBody::GetLinearVelocity() const
{
    return GetPxRigidDynamic() ? Physics::GetVector3FromPxVec3(
                                     GetPxRigidDynamic()->getLinearVelocity())
                               : Vector3::Zero();
}

Vector3 RigidBody::GetAngularVelocity() const
{
    return GetPxRigidDynamic() ? Physics::GetVector3FromPxVec3(
                                     GetPxRigidDynamic()->getAngularVelocity())
                               : Vector3::Zero();
}

float RigidBody::GetMaxAngularVelocity() const
{
    return GetPxRigidDynamic() ? GetPxRigidDynamic()->getMaxAngularVelocity()
                               : 0.0f;
}

const RigidBodyConstraints &RigidBody::GetConstraints() const
{
    return m_constraints;
}

void RigidBody::Reflect()
{
    Component::Reflect();

    BANG_REFLECT_VAR_MEMBER_HINTED(
        RigidBody, "Mass", SetMass, GetMass, BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    BANG_REFLECT_VAR_MEMBER_HINTED(
        RigidBody, "Drag", SetDrag, GetDrag, BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    BANG_REFLECT_VAR_MEMBER_HINTED(RigidBody,
                                   "Angular Drag",
                                   SetAngularDrag,
                                   GetAngularDrag,
                                   BANG_REFLECT_HINT_MIN_VALUE(0.0f));
    BANG_REFLECT_VAR_MEMBER(
        RigidBody, "Use gravity", SetUseGravity, GetUseGravity);
    BANG_REFLECT_VAR_MEMBER(
        RigidBody, "Is kinematic", SetIsKinematic, GetIsKinematic);

    // BANG_REFLECT_VAR_MEMBER_ENUM_FLAGS(
    //     RigidBody, "Constraints", SetConstraints, GetConstraints);

    ReflectVarEnum<FlagsPrimitiveType>(
        "Constraints",
        [this](const FlagsPrimitiveType &x) { SetConstraints(x); },
        [this]() { return SCAST<FlagsPrimitiveType>(GetConstraints()); },
        BANG_REFLECT_HINT_ENUM_FLAGS(true));

    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Constraints", "Position Lock X", RigidBodyConstraint::LOCK_POSITION_X);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Constraints", "Position Lock Y", RigidBodyConstraint::LOCK_POSITION_Y);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Constraints", "Position Lock Z", RigidBodyConstraint::LOCK_POSITION_Z);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Constraints", "Rotation Lock X", RigidBodyConstraint::LOCK_ROTATION_X);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Constraints", "Rotation Lock Y", RigidBodyConstraint::LOCK_ROTATION_Y);
    BANG_REFLECT_HINT_ENUM_FIELD_VALUE(
        "Constraints", "Rotation Lock Z", RigidBodyConstraint::LOCK_ROTATION_Z);
}

void RigidBody::UpdatePxRigidActorValues()
{
    if (GetPxRigidActor())
    {
        GetPxRigidActor()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY,
                                        !GetUseGravity());
    }

    if (GetPxRigidDynamic())
    {
        GetPxRigidDynamic()->setRigidBodyFlag(
            physx::PxRigidBodyFlag::eKINEMATIC, GetIsKinematic());
        GetPxRigidDynamic()->setMass(GetMass());
        GetPxRigidDynamic()->setRigidDynamicLockFlags(
            SCAST<physx::PxRigidDynamicLockFlags>(GetConstraints().GetValue()));
        GetPxRigidDynamic()->setLinearDamping(GetDrag());
        GetPxRigidDynamic()->setAngularDamping(GetAngularDrag());
    }
}

void RigidBody::SetPxEnabled(bool pxEnabled)
{
    GetPxRigidActor()->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION,
                                    !pxEnabled);
}

void RigidBody::OnPxRigidActorChanged(PxRigidActor *prevPxRigidActor,
                                      PxRigidActor *newPxRigidActor)
{
    BANG_UNUSED_2(prevPxRigidActor, newPxRigidActor);
    UpdatePxRigidActorValues();
}
