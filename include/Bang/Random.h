#ifndef RANDOM_H
#define RANDOM_H

#include <cstdlib>

#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/String.h"

namespace Bang
{
class Random
{
public:
    static void SetSeed(long seed);
    static void SetSeed(const String &seed);

    template <class T = float>
    static T GetValue01();

    template <class T = uint64_t>
    static T GetValue();

    template <class T>
    static T GetRange(T minIncluded, T maxExcluded);

    static bool GetBool();
    static Vector2 GetInsideUnitCircle();
    static Vector3 GetInsideUnitSphere();
    static Vector2 GetRandomVector2();
    static Vector3 GetRandomVector3();
    static Vector4 GetRandomVector4();
    static Quaternion GetRotation();
    static Color GetColor();
    static Color GetColorOpaque();

private:
    Random();
};

template <class T>
T Random::GetValue01()
{
    return SCAST<float>(rand()) / RAND_MAX;
}

template <class T>
T Random::GetValue()
{
    return SCAST<T>(rand());
}

template <class T>
T Random::GetRange(T minIncluded, T maxExcluded)
{
    return SCAST<T>(Random::GetValue01() * (maxExcluded - minIncluded)) +
           minIncluded;
}
}  // namespace Bang

#endif  // RANDOM_H
