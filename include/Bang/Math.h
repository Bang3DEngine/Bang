#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <limits>

#include "Bang/BangDefines.h"

namespace Bang
{
class Math
{
public:
    static constexpr double Pi = 3.1415926535897932384626;

    Math() = delete;

    template <class T>
    static constexpr T Min()
    {
        return std::numeric_limits<T>::min();
    }

    template <class T>
    static constexpr T Max()
    {
        return std::numeric_limits<T>::max();
    }

    template <class T>
    static constexpr T Infinity()
    {
        return std::numeric_limits<T>::infinity();
    }

    template <class T>
    static constexpr T NaN()
    {
        return 0;
    }

    template <class T>
    static constexpr T NegativeInfinity()
    {
        return -std::numeric_limits<T>::infinity();
    }

    template <class T>
    static constexpr T IsInfinity(T value)
    {
        return std::isinf(value);
    }

    template <class T>
    static constexpr T IsNaN(T value)
    {
        return std::isnan(value);
    }

    template <class T>
    static constexpr T Min(const T &a, const T &b)
    {
        return (a <= b) ? a : b;
    }

    template <class T>
    static constexpr T Max(const T &a, const T &b)
    {
        return (a >= b) ? a : b;
    }

    template <class T, class T2, class T3>
    static constexpr T Clamp(const T &value, const T2 &min, const T3 &max)
    {
        return Math::Min(Math::Max(SCAST<T>(value), SCAST<T>(min)),
                         SCAST<T>(max));
    }

    template <class T>
    static constexpr T Abs(const T &value)
    {
        return value < 0 ? -value : value;
    }

    template <class T = int, class T2>
    static constexpr T Round(const T2 &value)
    {
        return SCAST<T>(std::round(value));
    }

    template <class T>
    static constexpr T Ceil(const T &value)
    {
        return std::ceil(value);
    }

    template <class T>
    static constexpr T Floor(const T &value)
    {
        return std::floor(value);
    }

    template <class T>
    static constexpr T Fract(const T &value)
    {
        return value - Math::Floor(value);
    }

    template <class T>
    static constexpr T Pow(const T &base, const T &exponent)
    {
        return SCAST<T>(std::pow(base, exponent));
    }

    template <class T>
    static constexpr T Log(const T &value)
    {
        return SCAST<T>(std::log(value));
    }

    template <class T>
    static constexpr T Log10(const T &value)
    {
        return std::log10(value);
    }

    template <class T>
    static constexpr T Exp(const T &x)
    {
        return SCAST<T>(std::exp(x));
    }

    template <class T>
    static constexpr T Sqrt(const T &x)
    {
        return SCAST<T>(std::sqrt(x));
    }

    template <class T>
    static constexpr T Sin(const T &rad)
    {
        return SCAST<T>(std::sin(rad));
    }

    template <class T>
    static constexpr T Cos(const T &rad)
    {
        return SCAST<T>(std::cos(rad));
    }

    template <class T>
    static constexpr T Tan(const T &rad)
    {
        return SCAST<T>(std::tan(rad));
    }

    template <class T>
    static constexpr T Sinh(const T &rad)
    {
        return std::sinh(rad);
    }

    template <class T>
    static constexpr T Cosh(const T &rad)
    {
        return std::cosh(rad);
    }

    template <class T>
    static constexpr T Tanh(const T &rad)
    {
        return std::tanh(rad);
    }

    template <class T>
    static constexpr T ASin(const T &value)
    {
        return std::asin(value);
    }

    template <class T>
    static constexpr T ACos(const T &value)
    {
        return std::acos(value);
    }

    template <class T>
    static constexpr T ATan(const T &value)
    {
        return std::atan(value);
    }

    template <class T>
    static constexpr T ASinh(const T &value)
    {
        return std::asinh(value);
    }

    template <class T>
    static constexpr T ACosh(const T &value)
    {
        return std::acosh(value);
    }

    template <class T>
    static constexpr T ATanh(const T &value)
    {
        return std::atanh(value);
    }

    template <class T>
    static constexpr T ATan2(const T &valueX, const T &valueY)
    {
        return std::atan2(valueX, valueY);
    }

    template <class T>
    static constexpr T FMod(const T &value, const T &length)
    {
        return std::fmod(value, length);
    }

    template <class T>
    static constexpr T FModAbs(const T &value, const T &length)
    {
        return (value < 0
                    ? Math::FMod(Math::FMod(value, length) + length, length)
                    : Math::FMod(value, length));
    }

    template <class T>
    static constexpr T Sign(const T &value)
    {
        return static_cast<T>(value < 0 ? -1 : 1);
    }

    template <class T>
    static constexpr T Lerp(const T &a, const T &b, const T &t)
    {
        return (b - a) * t + a;
    }

    static constexpr float Map(float value,
                               float srcMin,
                               float srcMax,
                               float destMin,
                               float destMax)
    {
        return ((value - srcMin) / (srcMax - srcMin)) * (destMax - destMin) +
               destMin;
    }

    template <class T>
    static bool constexpr Equals(const T &lhs, const T &rhs, const T &diff)
    {
        return (Math::Abs(lhs - rhs) <= diff);
    }

    template <class T>
    static constexpr bool IsInt(const T &v, float tolerance = 0.0001f)
    {
        return (Math::Abs(v - Math::Round(v)) < tolerance);
    }

    template <class T>
    static constexpr bool IsPowerOfTwo(const T &v)
    {
        return Math::IsInt(Math::Log10(float(v)) / Math::Log10(2.0f));
    }

    static constexpr float DegToRad(float deg)
    {
        return deg * static_cast<float>(Math::s_DegToRad);
    }

    static constexpr float RadToDeg(float rad)
    {
        return rad * static_cast<float>(Math::s_RadToDeg);
    }

    static constexpr double DegToRad(double deg)
    {
        return deg * Math::s_DegToRad;
    }

    static constexpr double RadToDeg(double rad)
    {
        return rad * Math::s_RadToDeg;
    }

private:
    static constexpr double s_RadToDeg = 180.0 / Math::Pi;
    static constexpr double s_DegToRad = Math::Pi / 180.0;
};

template <>
inline double Math::NaN()
{
    return std::nan("");
}

template <>
inline float Math::NaN()
{
    return std::nanf("");
}
}  // namespace Bang

#endif  // MATH_H
