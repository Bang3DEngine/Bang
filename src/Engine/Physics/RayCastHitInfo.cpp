#include <stdint.h>

#include "Bang/BangDefines.h"
#include "Bang/Collider.h"
#include "Bang/RayCastHitInfo.h"

namespace Bang
{
class GameObject;
template <class>
class Array;
}

using namespace Bang;

const Array<RayCastHit> &RayCastHitInfo::GetHits() const
{
    return m_hits;
}

float RayCastHit::GetDistance() const
{
    return m_distance;
}

const Vector2 &RayCastHit::GetUv() const
{
    return m_uv;
}

uint32_t RayCastHit::GetFaceIndex() const
{
    return m_faceIndex;
}

const Vector3 &RayCastHit::GetNormal() const
{
    return m_normal;
}

const Vector3 &RayCastHit::GetPosition() const
{
    return m_position;
}

Collider *RayCastHit::GetCollider() const
{
    return p_collider;
}

GameObject *RayCastHit::GetGameObject() const
{
    return GetCollider()->GetGameObject();
}
