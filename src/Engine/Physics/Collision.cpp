#include "Bang/Collision.h"

#include "Bang/Collider.h"

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

