#ifndef RAYCASTHITINFO_H
#define RAYCASTHITINFO_H

#include "Bang/Bang.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"

namespace Bang
{
class Collider;
class GameObject;

struct RayCastHit
{
public:
    float GetDistance() const;
    const Vector2 &GetUv() const;
    uint32_t GetFaceIndex() const;
    const Vector3 &GetNormal() const;
    const Vector3 &GetPosition() const;

    Collider *GetCollider() const;
    GameObject *GetGameObject() const;

private:
    float m_distance = 0.0f;
    uint32_t m_faceIndex = 0;
    Vector2 m_uv = Vector2::Zero();
    Vector3 m_normal = Vector3::Zero();
    Vector3 m_position = Vector3::Zero();

    Collider *p_collider = nullptr;

    friend class PxSceneContainer;
};

struct RayCastHitInfo
{
public:
    const Array<RayCastHit> &GetHits() const;

private:
    Array<RayCastHit> m_hits;

    friend class PxSceneContainer;
};
}

#endif  // RAYCASTHITINFO_H
