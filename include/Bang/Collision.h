#ifndef COLLISION_H
#define COLLISION_H

#include "Bang/Array.h"
#include "Bang/Bang.h"
#include "Bang/BangDefines.h"
#include "Bang/CollisionContact.h"

NAMESPACE_BANG_BEGIN

FORWARD class Collider;
FORWARD class GameObject;
FORWARD class RigidBody;

class Collision
{
public:
	Collision();
    ~Collision();

    Collider *GetThisCollider() const;
    GameObject *GetOtherGameObject() const;
    const Array<CollisionContact> &GetContacts() const;

private:
    Collider *p_thisCollider = nullptr;

    Array<CollisionContact> m_contacts;

    friend class PxSceneContainer;
};

NAMESPACE_BANG_END

#endif // COLLISION_H

