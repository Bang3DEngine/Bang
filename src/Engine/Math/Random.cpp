#include "Bang/Random.h"

#include "Bang/Color.h"
#include "Bang/Math.h"
#include "Bang/Quaternion.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

using namespace Bang;

void Random::SetSeed(long seed)
{
    srand(seed);
}

void Random::SetSeed(const String &seedStr)
{
    long seed = 0;
    for (char c : seedStr)
    {
        seed += int(c);
    }
    Random::SetSeed(seed);
}

bool Random::GetBool()
{
    return (GetValue01() > 0.5f);
}

Vector2 Random::GetInsideUnitCircle()
{
    return Random::GetRandomVector2().NormalizedSafe();
}

Vector3 Random::GetInsideUnitSphere()
{
    return Random::GetRandomVector3().NormalizedSafe();
}

Vector2 Random::GetRandomVector2()
{
    return Vector2(Random::GetRange(-1.0f, 1.0f),
                   Random::GetRange(-1.0f, 1.0f));
}
Vector3 Random::GetRandomVector3()
{
    return Vector3(Random::GetRange(-1.0f, 1.0f),
                   Random::GetRange(-1.0f, 1.0f),
                   Random::GetRange(-1.0f, 1.0f));
}
Vector4 Random::GetRandomVector4()
{
    return Vector4(Random::GetRange(-1.0f, 1.0f),
                   Random::GetRange(-1.0f, 1.0f),
                   Random::GetRange(-1.0f, 1.0f),
                   Random::GetRange(-1.0f, 1.0f));
}

Quaternion Random::GetRotation()
{
    const float angle = Random::GetRange(0.0f, 2.0f * SCAST<float>(Math::Pi));
    return Quaternion::AngleAxis(angle, Random::GetInsideUnitSphere());
}

Color Random::GetColor()
{
    return Color(Random::GetValue01(),
                 Random::GetValue01(),
                 Random::GetValue01(),
                 Random::GetValue01());
}

Color Random::GetColorOpaque()
{
    return Color(
        Random::GetValue01(), Random::GetValue01(), Random::GetValue01(), 1.0f);
}

Random::Random()
{
}
