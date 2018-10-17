#include "Bang/CollisionContact.h"

using namespace Bang;

CollisionContact::CollisionContact()
{
}

CollisionContact::~CollisionContact()
{
}

const Vector3 &CollisionContact::GetPoint() const
{
    return m_point;
}

const Vector3 &CollisionContact::GetNormal() const
{
    return m_normal;
}

const Vector3 &CollisionContact::GetImpulse() const
{
    return m_impulse;
}

float CollisionContact::GetSeparation() const
{
    return m_separation;
}

Collider *CollisionContact::GetOtherCollider() const
{
    return p_otherCollider;
}
