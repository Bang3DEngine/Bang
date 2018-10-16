#include "Bang/Collision.h"

#include "Bang/Array.tcc"
#include "Bang/Collider.h"

FORWARD NAMESPACE_BANG_BEGIN
FORWARD class GameObject;
FORWARD NAMESPACE_BANG_END

USING_NAMESPACE_BANG

Collision::Collision()
{
}

Collision::~Collision()
{
}

Collider *Collision::GetThisCollider() const
{
    return p_thisCollider;
}

GameObject *Collision::GetOtherGameObject() const
{
    GameObject *otherGo = nullptr;
    if (GetContacts().Size() >= 1)
    {
        otherGo = GetContacts().Front().GetOtherCollider()->GetGameObject();
    }
    return otherGo;
}

const Array<CollisionContact> &Collision::GetContacts() const
{
    return m_contacts;
}

