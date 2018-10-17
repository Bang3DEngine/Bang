#ifndef COLLISION_H
#define COLLISION_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/CollisionContact.h"

namespace Bang
{
class Collider;
class GameObject;

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
}

#endif  // COLLISION_H
