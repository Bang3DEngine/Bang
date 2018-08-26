#ifndef COLLISIONCONTACT_H
#define COLLISIONCONTACT_H

#include "Bang/Vector3.h"

NAMESPACE_BANG_BEGIN

FORWARD class Collider;

class CollisionContact
{
public:
	CollisionContact();
    ~CollisionContact();

    const Vector3& GetPoint() const;
    const Vector3& GetNormal() const;
    const Vector3& GetImpulse() const;
    float GetSeparation() const;
    Collider *GetOtherCollider() const;

private:
    Vector3 m_point = Vector3::Zero;
    Vector3 m_normal = Vector3::Zero;
    Vector3 m_impulse = Vector3::Zero;
    float m_separation = 0.0f;
    Collider *p_otherCollider = nullptr;

    friend class PxSceneContainer;
};

NAMESPACE_BANG_END

#endif // COLLISIONCONTACT_H

