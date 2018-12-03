#pragma once

#include "Bang/Axis.h"
#include "Bang/Bang.h"
#include "Bang/Math.h"
#include "Bang/Vector4.h"

namespace Bang
{
template <class T>
Vector4G<T>::Vector4G()
{
    for (int i = 0; i < 4; ++i)
    {
        At(i) = SCAST<T>(0);
    }
}

template <class T>
Vector4G<T>::Vector4G(const T &a)
{
    for (int i = 0; i < 4; ++i)
    {
        At(i) = SCAST<T>(a);
    }
}

template <class T>
template <class OtherT>
Vector4G<T>::Vector4G(const Vector4G<OtherT> &v)
{
    for (int i = 0; i < 4; ++i)
    {
        At(i) = SCAST<T>(v[i]);
    }
}

template <class T>
template <class OtherT1, class OtherT2, class OtherT3, class OtherT4>
Vector4G<T>::Vector4G(OtherT1 _x, OtherT2 _y, OtherT3 _z, OtherT4 _w)
    : x(SCAST<T>(_x)), y(SCAST<T>(_y)), z(SCAST<T>(_z)), w(SCAST<T>(_w))
{
}

template <class T>
template <class OtherT1, class OtherT2, class OtherT3>
Vector4G<T>::Vector4G(const Vector2G<OtherT1> &v,
                      const OtherT2 &_z,
                      const OtherT3 &_w)
    : x(SCAST<T>(v.x)), y(SCAST<T>(v.y)), z(SCAST<T>(_z)), w(SCAST<T>(_w))
{
}

template <class T>
template <class OtherT1, class OtherT2, class OtherT3>
Vector4G<T>::Vector4G(const OtherT1 &_x,
                      const Vector2G<OtherT2> &v,
                      const OtherT3 &_w)
    : x(SCAST<T>(_x)), y(SCAST<T>(v.y)), z(SCAST<T>(v.z)), w(SCAST<T>(_w))
{
}

template <class T>
template <class OtherT1, class OtherT2, class OtherT3>
Vector4G<T>::Vector4G(const OtherT1 &_x,
                      const OtherT2 &_y,
                      const Vector2G<OtherT3> &v)
    : x(SCAST<T>(_x)), y(SCAST<T>(_y)), z(SCAST<T>(v.z)), w(SCAST<T>(v.w))
{
}

template <class T>
template <class OtherT1, class OtherT2>
Vector4G<T>::Vector4G(const Vector3G<OtherT1> &v, const OtherT2 &_w)
    : x(SCAST<T>(v.x)), y(SCAST<T>(v.y)), z(SCAST<T>(v.z)), w(SCAST<T>(_w))
{
}

template <class T>
template <class OtherT1, class OtherT2>
Vector4G<T>::Vector4G(const OtherT1 &_x, const Vector3G<OtherT2> &v)
    : x(SCAST<T>(_x)), y(SCAST<T>(v.y)), z(SCAST<T>(v.z)), w(SCAST<T>(v.w))
{
}

template <class T>
T Vector4G<T>::Length() const
{
    return Math::Sqrt(SqLength());
}

template <class T>
T Vector4G<T>::SqLength() const
{
    T res = SCAST<T>(0);
    for (int i = 0; i < 4; ++i)
    {
        res += At(i) * At(i);
    }
    return res;
}

template <class T>
void Vector4G<T>::Normalize()
{
    *this /= Length();
}

template <class T>
Vector4G<T> Vector4G<T>::NormalizedSafe() const
{
    if (*this == Vector4G<T>::Zero)
    {
        return Vector4G<T>::Zero;
    }
    return (*this).Normalized();
}

template <class T>
Vector4G<T> Vector4G<T>::Normalized() const
{
    Vector4G<T> v(*this);
    v.Normalize();
    return v;
}

template <class T>
Vector4G<T> Vector4G<T>::ToDegrees() const
{
    Vector4G<T> res(*this);
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::RadToDeg(res[i]);
    }
    return res;
}

template <class T>
Vector4G<T> Vector4G<T>::ToRadians() const
{
    Vector4G<T> res(*this);
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::DegToRad(res[i]);
    }
    return res;
}

template <class T>
T Vector4G<T>::Distance(const Vector4G<T> &p) const
{
    return Vector4G<T>::Distance(*this, p);
}

template <class T>
T Vector4G<T>::SqDistance(const Vector4G<T> &p) const
{
    return Vector4G<T>::SqDistance(*this, p);
}

template <class T>
T &Vector4G<T>::At(std::size_t i)
{
    return (*this)[i];
}
template <class T>
const T &Vector4G<T>::At(std::size_t i) const
{
    return (*this)[i];
}

template <class T>
template <class Real>
Vector4G<T> Vector4G<T>::Lerp(const Vector4G<T> &v1,
                              const Vector4G<T> &v2,
                              Real t)
{
    return v1 + (v2 - v1) * t;
}

template <class T>
Vector4G<T> Vector4G<T>::Abs() const
{
    Vector4G res(*this);
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::Abs(res[i]);
    }
    return res;
}

template <class T>
T *Vector4G<T>::Data()
{
    return &At(0);
}
template <class T>
const T *Vector4G<T>::Data() const
{
    return &At(0);
}

template <class T>
Vector4G<T> Vector4G<T>::Abs(const Vector4G<T> &v)
{
    return v.Abs();
}

template <class T>
T Vector4G<T>::Dot(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    T res = SCAST<T>(0);
    for (int i = 0; i < 4; ++i)
    {
        res += v1[i] * v2[i];
    }
    return res;
}

template <class T>
T Vector4G<T>::Distance(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    return (v1 - v2).Length();
}

template <class T>
T Vector4G<T>::SqDistance(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    return (v1 - v2).SqLength();
}

template <class T>
Vector4G<T> Vector4G<T>::Max(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    Vector4G res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::Max(v1[i], v2[i]);
    }
    return res;
}

template <class T>
Vector4G<T> Vector4G<T>::Min(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    Vector4G res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::Min(v1[i], v2[i]);
    }
    return res;
}

template <class T>
Vector4G<T> Vector4G<T>::Floor(const Vector4G<T> &v1)
{
    Vector4G res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::Floor(v1[i]);
    }
    return res;
}

template <class T>
Vector4G<T> Vector4G<T>::Ceil(const Vector4G<T> &v1)
{
    Vector4G res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::Ceil(v1[i]);
    }
    return res;
}

template <class T>
Vector4G<T> Vector4G<T>::Round(const Vector4G<T> &v1)
{
    Vector4G res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = SCAST<T>(Math::Round(v1[i]));
    }
    return res;
}

template <class T>
Vector4G<T> Vector4G<T>::Clamp(const Vector4G<T> &v,
                               const Vector4G<T> &min,
                               const Vector4G<T> &max)
{
    Vector4G res = v;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::Clamp(res[i], min[i], max[i]);
    }
    return res;
}

template <class T>
Vector4G<T> Vector4G<T>::Clamp2(const Vector4G<T> &v,
                                const Vector4G<T> &bound1,
                                const Vector4G<T> &bound2)
{
    Vector4G res = v;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = Math::Clamp(res[i],
                             Math::Min(bound1[i], bound2[i]),
                             Math::Max(bound1[i], bound2[i]));
    }
    return res;
}

template <class T>
Axis Vector4G<T>::GetAxis() const
{
    return x == 1 ? Axis::HORIZONTAL : Axis::VERTICAL;
}
template <class T>
const T &Vector4G<T>::GetAxis(Axis axis) const
{
    return (axis == Axis::HORIZONTAL) ? x : y;
}
template <class T>
Vector4G<T> Vector4G<T>::FromAxis(Axis axis)
{
    return (axis == Axis::HORIZONTAL) ? Vector4G<T>::Right : Vector4G<T>::Up;
}

template <class T>
T &Vector4G<T>::operator[](std::size_t i)
{
    return (RCAST<T *>(this))[i];
}

template <class T>
const T &Vector4G<T>::operator[](std::size_t i) const
{
    return (RCAST<const T *>(this))[i];
}

/* Operators */

template <class T>
bool operator==(const Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator<(const Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        if (lhs[i] >= rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator<=(const Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        if (lhs[i] > rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator>(const Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    return (rhs < lhs);
}

template <class T>
bool operator>=(const Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    return (rhs <= lhs);
}

template <class T>
bool operator!=(const Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    return !(lhs == rhs);
}

template <class T>
Vector4G<T> operator+(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = v1[i] + v2[i];
    }
    return res;
}

template <class T>
Vector4G<T> operator*(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = v1[i] * v2[i];
    }
    return res;
}

template <class T>
Vector4G<T> operator*(const T &a, const Vector4G<T> &v)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = a * v[i];
    }
    return res;
}

template <class T>
Vector4G<T> operator*(const Vector4G<T> &v, const T &a)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = v[i] * a;
    }
    return res;
}

template <class T>
Vector4G<T> operator/(const T &a, const Vector4G<T> &v)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = a / v[i];
    }
    return res;
}

template <class T>
Vector4G<T> operator/(const Vector4G<T> &v, const T &a)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = v[i] / a;
    }
    return res;
}

template <class T>
Vector4G<T> operator/(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = v1[i] / v2[i];
    }
    return res;
}

template <class T>
Vector4G<T> &operator+=(Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] += rhs[i];
    }
    return lhs;
}

template <class T>
Vector4G<T> &operator-=(Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] -= rhs[i];
    }
    return lhs;
}

template <class T>
Vector4G<T> &operator*=(Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] *= rhs[i];
    }
    return lhs;
}

template <class T>
Vector4G<T> &operator/=(Vector4G<T> &lhs, const Vector4G<T> &rhs)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] /= rhs[i];
    }
    return lhs;
}

template <class T>
Vector4G<T> operator+(const T &a, const Vector4G<T> &v)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = a + v[i];
    }
    return res;
}

template <class T>
Vector4G<T> operator+(const Vector4G<T> &v, const T &a)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = v[i] + a;
    }
    return res;
}

template <class T>
Vector4G<T> operator-(const Vector4G<T> &v1, const Vector4G<T> &v2)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = v1[i] - v2[i];
    }
    return res;
}

template <class T>
Vector4G<T> operator-(const T &a, const Vector4G<T> &v)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = a - v[i];
    }
    return res;
}

template <class T>
Vector4G<T> operator-(const Vector4G<T> &v, const T &a)
{
    Vector4G<T> res;
    for (int i = 0; i < 4; ++i)
    {
        res[i] = v[i] - a;
    }
    return res;
}

template <class T>
Vector4G<T> &operator+=(Vector4G<T> &lhs, const T &a)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] += a;
    }
    return lhs;
}

template <class T>
Vector4G<T> &operator-=(Vector4G<T> &lhs, const T &a)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] -= a;
    }
    return lhs;
}

template <class T>
Vector4G<T> &operator*=(Vector4G<T> &lhs, const T &a)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] *= a;
    }
    return lhs;
}

template <class T>
Vector4G<T> &operator/=(Vector4G<T> &lhs, const T &a)
{
    for (int i = 0; i < 4; ++i)
    {
        lhs[i] /= a;
    }
    return lhs;
}

template <class T>
Vector4G<T> operator-(const Vector4G<T> &v)
{
    return v * SCAST<T>(-1);
}

template <class T>
const Vector4G<T> &Vector4G<T>::Up()
{
    static const Vector4G<T> v =
        Vector4G<T>(SCAST<T>(0), SCAST<T>(1), SCAST<T>(0), SCAST<T>(0));
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::Down()
{
    static const Vector4G<T> v =
        Vector4G<T>(SCAST<T>(0), SCAST<T>(-1), SCAST<T>(0), SCAST<T>(0));
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::Right()
{
    static const Vector4G<T> v =
        Vector4G<T>(SCAST<T>(1), SCAST<T>(0), SCAST<T>(0), SCAST<T>(0));
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::Left()
{
    static const Vector4G<T> v =
        Vector4G<T>(SCAST<T>(-1), SCAST<T>(0), SCAST<T>(0), SCAST<T>(0));
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::Zero()
{
    static const Vector4G<T> v = Vector4G<T>(SCAST<T>(0));
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::One()
{
    static const Vector4G<T> v = Vector4G<T>(SCAST<T>(1));
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::Forward()
{
    static const Vector4G<T> v =
        Vector4G<T>(SCAST<T>(0), SCAST<T>(0), SCAST<T>(-1), SCAST<T>(0));
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::Back()
{
    static const Vector4G<T> v =
        Vector4G<T>(SCAST<T>(0), SCAST<T>(0), SCAST<T>(1), SCAST<T>(0));
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::Infinity()
{
    static const Vector4G<T> v = Vector4G<T>(Math::Infinity<T>());
    return v;
}
template <class T>
const Vector4G<T> &Vector4G<T>::NInfinity()
{
    static const Vector4G<T> v = Vector4G<T>(Math::NegativeInfinity<T>());
    return v;
}

}  // namespace Bang
