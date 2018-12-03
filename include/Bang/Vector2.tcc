#pragma once

#include "Bang/Axis.h"
#include "Bang/Math.h"
#include "Bang/Vector2.h"

namespace Bang
{
template <class T>
T Vector2G<T>::Cross(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    return (v1.x * v2.y) - (v1.y * v2.x);
}

template <class T>
Vector2G<T>::Vector2G()
{
    for (int i = 0; i < 2; ++i)
    {
        At(i) = SCAST<T>(0);
    }
}

template <class T>
template <class OtherT1, class OtherT2>
Vector2G<T>::Vector2G(const OtherT1 &_x, const OtherT2 &_y)
    : x(SCAST<T>(_x)), y(SCAST<T>(_y))
{
}

template <class T>
Vector2G<T>::Vector2G(const T &a)
{
    for (int i = 0; i < 2; ++i)
    {
        At(i) = SCAST<T>(a);
    }
}

template <class T>
template <class OtherT>
Vector2G<T>::Vector2G(const Vector2G<OtherT> &v)
{
    for (int i = 0; i < 2; ++i)
    {
        At(i) = SCAST<T>(v[i]);
    }
}

template <class T>
T Vector2G<T>::Length() const
{
    return Math::Sqrt(SqLength());
}

template <class T>
T Vector2G<T>::SqLength() const
{
    T res = SCAST<T>(0);
    for (int i = 0; i < 2; ++i)
    {
        res += At(i) * At(i);
    }
    return res;
}

template <class T>
void Vector2G<T>::Normalize()
{
    *this /= Length();
}

template <class T>
Vector2G<T> Vector2G<T>::NormalizedSafe() const
{
    if (*this == Vector2G<T>::Zero())
    {
        return Vector2G<T>::Zero();
    }
    return (*this).Normalized();
}

template <class T>
Vector2G<T> Vector2G<T>::Normalized() const
{
    Vector2G<T> v(*this);
    v.Normalize();
    return v;
}

template <class T>
Vector2G<T> Vector2G<T>::ToDegrees() const
{
    Vector2G<T> res(*this);
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::RadToDeg(res[i]);
    }
    return res;
}

template <class T>
Vector2G<T> Vector2G<T>::ToRadians() const
{
    Vector2G<T> res(*this);
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::DegToRad(res[i]);
    }
    return res;
}

template <class T>
T Vector2G<T>::Distance(const Vector2G<T> &p) const
{
    return Vector2G<T>::Distance(*this, p);
}

template <class T>
T Vector2G<T>::SqDistance(const Vector2G<T> &p) const
{
    return Vector2G<T>::SqDistance(*this, p);
}

template <class T>
T &Vector2G<T>::At(std::size_t i)
{
    return (*this)[i];
}
template <class T>
const T &Vector2G<T>::At(std::size_t i) const
{
    return (*this)[i];
}

template <class T>
template <class Real>
Vector2G<T> Vector2G<T>::Lerp(const Vector2G<T> &v1,
                              const Vector2G<T> &v2,
                              Real t)
{
    return v1 + (v2 - v1) * t;
}

template <class T>
Vector2G<T> Vector2G<T>::Perpendicular() const
{
    return Vector2G<T>(-y, x);
}

template <class T>
Vector2G<T> Vector2G<T>::Abs() const
{
    Vector2G res(*this);
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::Abs(res[i]);
    }
    return res;
}

template <class T>
Vector3G<T> Vector2G<T>::xy0() const
{
    return Vector3G<T>(x, y, 0);
}

template <class T>
Vector3G<T> Vector2G<T>::xy1() const
{
    return Vector3G<T>(x, y, 1);
}

template <class T>
Vector3G<T> Vector2G<T>::x0y() const
{
    return Vector3G<T>(x, 0, y);
}

template <class T>
Vector3G<T> Vector2G<T>::x1y() const
{
    return Vector3G<T>(x, 1, y);
}

template <class T>
T *Vector2G<T>::Data()
{
    return &At(0);
}
template <class T>
const T *Vector2G<T>::Data() const
{
    return &At(0);
}

template <class T>
Vector2G<T> Vector2G<T>::Abs(const Vector2G<T> &v)
{
    return v.Abs();
}

template <class T>
T Vector2G<T>::Dot(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    T res = SCAST<T>(0);
    for (int i = 0; i < 2; ++i)
    {
        res += v1[i] * v2[i];
    }
    return res;
}

template <class T>
T Vector2G<T>::Distance(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    return (v1 - v2).Length();
}

template <class T>
T Vector2G<T>::SqDistance(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    return (v1 - v2).SqLength();
}

template <class T>
Vector2G<T> Vector2G<T>::Max(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    Vector2G res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::Max(v1[i], v2[i]);
    }
    return res;
}

template <class T>
Vector2G<T> Vector2G<T>::Min(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    Vector2G res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::Min(v1[i], v2[i]);
    }
    return res;
}

template <class T>
Vector2G<T> Vector2G<T>::Floor(const Vector2G<T> &v1)
{
    Vector2G res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::Floor(v1[i]);
    }
    return res;
}

template <class T>
Vector2G<T> Vector2G<T>::Ceil(const Vector2G<T> &v1)
{
    Vector2G res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::Ceil(v1[i]);
    }
    return res;
}

template <class T>
Vector2G<T> Vector2G<T>::Round(const Vector2G<T> &v1)
{
    Vector2G res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = SCAST<T>(Math::Round(v1[i]));
    }
    return res;
}

template <class T>
Vector2G<T> Vector2G<T>::Clamp(const Vector2G<T> &v,
                               const Vector2G<T> &min,
                               const Vector2G<T> &max)
{
    Vector2G res = v;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::Clamp(res[i], min[i], max[i]);
    }
    return res;
}

template <class T>
Vector2G<T> Vector2G<T>::Clamp2(const Vector2G<T> &v,
                                const Vector2G<T> &bound1,
                                const Vector2G<T> &bound2)
{
    Vector2G res = v;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = Math::Clamp(res[i],
                             Math::Min(bound1[i], bound2[i]),
                             Math::Max(bound1[i], bound2[i]));
    }
    return res;
}

template <class T>
Axis Vector2G<T>::GetAxis() const
{
    return x == 1 ? Axis::HORIZONTAL : Axis::VERTICAL;
}
template <class T>
const T &Vector2G<T>::GetAxis(Axis axis) const
{
    return (axis == Axis::HORIZONTAL) ? x : y;
}
template <class T>
Vector2G<T> Vector2G<T>::FromAxis(Axis axis)
{
    return (axis == Axis::HORIZONTAL) ? Vector2G<T>::Right()
                                      : Vector2G<T>::Up();
}

template <class T>
T &Vector2G<T>::operator[](std::size_t i)
{
    return (RCAST<T *>(this))[i];
}

template <class T>
const T &Vector2G<T>::operator[](std::size_t i) const
{
    return (RCAST<const T *>(this))[i];
}

/* Operators */

template <class T>
bool operator==(const Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    for (int i = 0; i < 2; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator<(const Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    for (int i = 0; i < 2; ++i)
    {
        if (lhs[i] >= rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator<=(const Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    for (int i = 0; i < 2; ++i)
    {
        if (lhs[i] > rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator>(const Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    return (rhs < lhs);
}

template <class T>
bool operator>=(const Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    return (rhs <= lhs);
}

template <class T>
bool operator!=(const Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    return !(lhs == rhs);
}

template <class T>
Vector2G<T> operator+(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = v1[i] + v2[i];
    }
    return res;
}

template <class T>
Vector2G<T> operator*(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = v1[i] * v2[i];
    }
    return res;
}

template <class T>
Vector2G<T> operator*(const T &a, const Vector2G<T> &v)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = a * v[i];
    }
    return res;
}

template <class T>
Vector2G<T> operator*(const Vector2G<T> &v, const T &a)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = v[i] * a;
    }
    return res;
}

template <class T>
Vector2G<T> operator/(const T &a, const Vector2G<T> &v)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = a / v[i];
    }
    return res;
}

template <class T>
Vector2G<T> operator/(const Vector2G<T> &v, const T &a)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = v[i] / a;
    }
    return res;
}

template <class T>
Vector2G<T> operator/(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = v1[i] / v2[i];
    }
    return res;
}

template <class T>
Vector2G<T> &operator+=(Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    for (int i = 0; i < 2; ++i)
    {
        lhs[i] += rhs[i];
    }
    return lhs;
}

template <class T>
Vector2G<T> &operator-=(Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    for (int i = 0; i < 2; ++i)
    {
        lhs[i] -= rhs[i];
    }
    return lhs;
}

template <class T>
Vector2G<T> &operator*=(Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    for (int i = 0; i < 2; ++i)
    {
        lhs[i] *= rhs[i];
    }
    return lhs;
}

template <class T>
Vector2G<T> &operator/=(Vector2G<T> &lhs, const Vector2G<T> &rhs)
{
    for (int i = 0; i < 2; ++i)
    {
        lhs[i] /= rhs[i];
    }
    return lhs;
}

template <class T>
Vector2G<T> operator+(const T &a, const Vector2G<T> &v)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = a + v[i];
    }
    return res;
}

template <class T>
Vector2G<T> operator+(const Vector2G<T> &v, const T &a)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = v[i] + a;
    }
    return res;
}

template <class T>
Vector2G<T> operator-(const Vector2G<T> &v1, const Vector2G<T> &v2)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = v1[i] - v2[i];
    }
    return res;
}

template <class T>
Vector2G<T> operator-(const T &a, const Vector2G<T> &v)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = a - v[i];
    }
    return res;
}

template <class T>
Vector2G<T> operator-(const Vector2G<T> &v, const T &a)
{
    Vector2G<T> res;
    for (int i = 0; i < 2; ++i)
    {
        res[i] = v[i] - a;
    }
    return res;
}

template <class T>
Vector2G<T> &operator+=(Vector2G<T> &lhs, const T &a)
{
    for (int i = 0; i < 2; ++i)
    {
        lhs[i] += a;
    }
    return lhs;
}

template <class T>
Vector2G<T> &operator-=(Vector2G<T> &lhs, const T &a)
{
    for (int i = 0; i < 2; ++i)
    {
        lhs[i] -= a;
    }
    return lhs;
}

template <class T>
Vector2G<T> &operator*=(Vector2G<T> &lhs, const T &a)
{
    for (int i = 0; i < 2; ++i)
    {
        lhs[i] *= a;
    }
    return lhs;
}

template <class T>
Vector2G<T> &operator/=(Vector2G<T> &lhs, const T &a)
{
    for (int i = 0; i < 2; ++i)
    {
        lhs[i] /= a;
    }
    return lhs;
}

template <class T>
Vector2G<T> operator-(const Vector2G<T> &v)
{
    return v * SCAST<T>(-1);
}

template <class T>
const Vector2G<T> &Vector2G<T>::Up()
{
    static const Vector2G<T> v = Vector2G<T>(SCAST<T>(0), SCAST<T>(1));
    return v;
}
template <class T>
const Vector2G<T> &Vector2G<T>::Down()
{
    static const Vector2G<T> v = Vector2G<T>(SCAST<T>(0), SCAST<T>(-1));
    return v;
}
template <class T>
const Vector2G<T> &Vector2G<T>::Right()
{
    static const Vector2G<T> v = Vector2G<T>(SCAST<T>(1), SCAST<T>(0));
    return v;
}
template <class T>
const Vector2G<T> &Vector2G<T>::Left()
{
    static const Vector2G<T> v = Vector2G<T>(SCAST<T>(-1), SCAST<T>(0));
    return v;
}
template <class T>
const Vector2G<T> &Vector2G<T>::Zero()
{
    static const Vector2G<T> v = Vector2G<T>(SCAST<T>(0));
    return v;
}
template <class T>
const Vector2G<T> &Vector2G<T>::One()
{
    static const Vector2G<T> v = Vector2G<T>(SCAST<T>(1));
    return v;
}
template <class T>
const Vector2G<T> &Vector2G<T>::Infinity()
{
    static const Vector2G<T> v = Vector2G<T>(Math::Infinity<T>());
    return v;
}
template <class T>
const Vector2G<T> &Vector2G<T>::NInfinity()
{
    static const Vector2G<T> v = Vector2G<T>(Math::NegativeInfinity<T>());
    return v;
}

}  // namespace Bang
