#ifndef RAYCASTINFO_H
#define RAYCASTINFO_H

#include "Bang/Bang.h"
#include "Bang/Math.h"
#include "Bang/Vector3.h"

NAMESPACE_BANG_BEGIN

struct RayCastInfo
{
    Vector3 origin = Vector3::Zero;
    Vector3 direction = Vector3::Forward;
    float maxDistance = Math::Infinity<float>();
};

NAMESPACE_BANG_END

#endif // RAYCASTINFO_H
