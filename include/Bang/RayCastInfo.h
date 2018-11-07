#ifndef RAYCASTINFO_H
#define RAYCASTINFO_H

#include "Bang/Bang.h"
#include "Bang/Math.h"
#include "Bang/Vector3.h"

namespace Bang
{
struct RayCastInfo
{
    Vector3 origin = Vector3::Zero();
    Vector3 direction = Vector3::Forward();
    float maxDistance = Math::Infinity<float>();
};
}

#endif  // RAYCASTINFO_H
