#include "Bang/Collision.h"

#include "Bang/Array.tcc"
#include "Bang/Collider.h"

namespace Bang
{
class GameObject;
}

using namespace Bang;

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
