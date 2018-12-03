#pragma once

#include "Bang/Axis.h"
#include "Bang/Math.h"
#include "Bang/Vector3.h"

namespace Bang
{
template <class T>
Vector3G<T>::Vector3G()
{
    for (int i = 0; i < 3; ++i)
    {
        At(i) = SCAST<T>(0);
    }
}

template <class T>
Vector3G<T>::Vector3G(const T &a)
{
    for (int i = 0; i < 3; ++i)
    {
        At(i) = SCAST<T>(a);
    }
}

template <class T>
template <class OtherT>
Vector3G<T>::Vector3G(const Vector3G<OtherT> &v)
{
    for (int i = 0; i < 3; ++i)
    {
        At(i) = SCAST<T>(v[i]);
    }
}

template <class T>
template <class OtherT1, class OtherT2, class OtherT3>
Vector3G<T>::Vector3G(const OtherT1 &_x, const OtherT2 &_y, const OtherT3 &_z)
    : x(SCAST<T>(_x)), y(SCAST<T>(_y)), z(SCAST<T>(_z))
{
}

template <class T>
template <class OtherT1, class OtherT2>
Vector3G<T>::Vector3G(const Vector2G<OtherT1> &v, const OtherT2 &_z)
    : x(SCAST<T>(v.x)), y(SCAST<T>(v.y)), z(SCAST<T>(_z))
{
}

template <class T>
template <class OtherT1, class OtherT2>
Vector3G<T>::Vector3G(const OtherT1 &_x, const Vector2G<OtherT2> &v)
    : x(SCAST<T>(_x)), y(SCAST<T>(v.y)), z(SCAST<T>(v.z))
{
}

template <class T>
T Vector3G<T>::Length() const
{
    return Math::Sqrt(SqLength());
}

template <class T>
T Vector3G<T>::SqLength() const
{
    T res = SCAST<T>(0);
    for (int i = 0; i < 3; ++i)
    {
        res += At(i) * At(i);
    }
    return res;
}

template <class T>
void Vector3G<T>::Normalize()
{
    *this /= Length();
}

template <class T>
Vector3G<T> Vector3G<T>::NormalizedSafe() const
{
    if (*this == Vector3G<T>::Zero())
    {
        return Vector3G<T>::Zero();
    }
    return (*this).Normalized();
}

template <class T>
Vector3G<T> Vector3G<T>::Normalized() const
{
    Vector3G<T> v(*this);
    v.Normalize();
    return v;
}

template <class T>
Vector3G<T> Vector3G<T>::ToDegrees() const
{
    Vector3G<T> res(*this);
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::RadToDeg(res[i]);
    }
    return res;
}

template <class T>
Vector3G<T> Vector3G<T>::ToRadians() const
{
    Vector3G<T> res(*this);
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::DegToRad(res[i]);
    }
    return res;
}

template <class T>
T Vector3G<T>::Distance(const Vector3G<T> &p) const
{
    return Vector3G<T>::Distance(*this, p);
}

template <class T>
T Vector3G<T>::SqDistance(const Vector3G<T> &p) const
{
    return Vector3G<T>::SqDistance(*this, p);
}

template <class T>
T &Vector3G<T>::At(std::size_t i)
{
    return (*this)[i];
}
template <class T>
const T &Vector3G<T>::At(std::size_t i) const
{
    return (*this)[i];
}

template <class T>
template <class Real>
Vector3G<T> Vector3G<T>::Lerp(const Vector3G<T> &v1,
                              const Vector3G<T> &v2,
                              Real t)
{
    return v1 + (v2 - v1) * t;
}

template <class T>
Vector3G<T> Vector3G<T>::Abs() const
{
    Vector3G res(*this);
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::Abs(res[i]);
    }
    return res;
}

template <class T>
T *Vector3G<T>::Data()
{
    return &At(0);
}
template <class T>
const T *Vector3G<T>::Data() const
{
    return &At(0);
}

template <class T>
Vector3G<T> Vector3G<T>::Abs(const Vector3G<T> &v)
{
    return v.Abs();
}

template <class T>
T Vector3G<T>::Dot(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    T res = SCAST<T>(0);
    for (int i = 0; i < 3; ++i)
    {
        res += v1[i] * v2[i];
    }
    return res;
}

template <class T>
T Vector3G<T>::Distance(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    return (v1 - v2).Length();
}

template <class T>
T Vector3G<T>::SqDistance(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    return (v1 - v2).SqLength();
}

template <class T>
Vector3G<T> Vector3G<T>::Max(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    Vector3G res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::Max(v1[i], v2[i]);
    }
    return res;
}

template <class T>
Vector3G<T> Vector3G<T>::Min(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    Vector3G res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::Min(v1[i], v2[i]);
    }
    return res;
}

template <class T>
Vector3G<T> Vector3G<T>::Floor(const Vector3G<T> &v1)
{
    Vector3G res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::Floor(v1[i]);
    }
    return res;
}

template <class T>
Vector3G<T> Vector3G<T>::Ceil(const Vector3G<T> &v1)
{
    Vector3G res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::Ceil(v1[i]);
    }
    return res;
}

template <class T>
Vector3G<T> Vector3G<T>::Round(const Vector3G<T> &v1)
{
    Vector3G res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::Round(v1[i]);
    }
    return res;
}

template <class T>
Vector3G<T> Vector3G<T>::Clamp(const Vector3G<T> &v,
                               const Vector3G<T> &min,
                               const Vector3G<T> &max)
{
    Vector3G res = v;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::Clamp(res[i], min[i], max[i]);
    }
    return res;
}

template <class T>
Vector3G<T> Vector3G<T>::Clamp2(const Vector3G<T> &v,
                                const Vector3G<T> &bound1,
                                const Vector3G<T> &bound2)
{
    Vector3G res = v;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = Math::Clamp(res[i],
                             Math::Min(bound1[i], bound2[i]),
                             Math::Max(bound1[i], bound2[i]));
    }
    return res;
}

template <class T>
Axis Vector3G<T>::GetAxis() const
{
    return x == 1 ? Axis::HORIZONTAL : Axis::VERTICAL;
}
template <class T>
const T &Vector3G<T>::GetAxis(Axis axis) const
{
    return (axis == Axis::HORIZONTAL) ? x : y;
}
template <class T>
Vector3G<T> Vector3G<T>::FromAxis(Axis axis)
{
    return (axis == Axis::HORIZONTAL) ? Vector3G<T>::Right : Vector3G<T>::Up;
}

template <class T>
T &Vector3G<T>::operator[](std::size_t i)
{
    return (RCAST<T *>(this))[i];
}

template <class T>
const T &Vector3G<T>::operator[](std::size_t i) const
{
    return (RCAST<const T *>(this))[i];
}

template <class T>
Vector3G<T> Vector3G<T>::ProjectedOnPlane(const Vector3G<T> &planeNormal,
                                          const Vector3G<T> &planePoint) const
{
    Vector3G<T> n = planeNormal.Normalized();
    return (*this) - n * Vector3G<T>::Dot(n, *this - planePoint);
}

template <class T>
Vector3G<T> Vector3G<T>::ProjectedOnVector(const Vector3G<T> &vector) const
{
    return Vector3G<T>::Dot(*this, vector) * vector.Normalized();
}
template <class T>
Vector2G<T> Vector3G<T>::ProjectedOnAxis(Axis3D axis) const
{
    switch (axis)
    {
        case Axis3D::X: return Vector2G<T>(y, z);
        case Axis3D::Y: return Vector2G<T>(x, z);
        case Axis3D::Z: return Vector2G<T>(x, y);
    }
    return Vector2G<T>::Zero();
}

template <class T>
template <class OtherT1, class OtherT2>
Vector3G<T> Vector3G<T>::Cross(const Vector3G<OtherT1> &v1,
                               const Vector3G<OtherT2> &v2)
{
    return Vector3G<T>(v1.y * v2.z - v1.z * v2.y,
                       v1.z * v2.x - v1.x * v2.z,
                       v1.x * v2.y - v1.y * v2.x);
}

template <class T>
template <class OtherT1, class OtherT2>
Vector3G<T> Vector3G<T>::Reflect(const Vector3G<OtherT1> &incident,
                                 const Vector3G<OtherT2> &normal)
{
    Vector3G<T> n = normal.Normalized();
    return incident - 2 * (Vector3G<T>::Dot(incident, n)) * n;
}

template <class T>
Vector2G<T> Vector3G<T>::xy() const
{
    return Vector2G<T>(x, y);
}

template <class T>
Vector3G<T> Vector3G<T>::xy0() const
{
    return Vector3G<T>(x, y, 0);
}

template <class T>
Vector3G<T> Vector3G<T>::xy1() const
{
    return Vector3G<T>(x, y, 1);
}

template <class T>
Vector3G<T> Vector3G<T>::x0z() const
{
    return Vector3G<T>(x, 0, z);
}

template <class T>
Vector3G<T> Vector3G<T>::x1z() const
{
    return Vector3G<T>(x, 1, z);
}

template <class T>
Vector2G<T> Vector3G<T>::xz() const
{
    return Vector2G<T>(x, z);
}

/* Operators */

template <class T>
bool operator==(const Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator<(const Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        if (lhs[i] >= rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator<=(const Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        if (lhs[i] > rhs[i])
        {
            return false;
        }
    }
    return true;
}

template <class T>
bool operator>(const Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    return (rhs < lhs);
}

template <class T>
bool operator>=(const Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    return (rhs <= lhs);
}

template <class T>
bool operator!=(const Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    return !(lhs == rhs);
}

template <class T>
Vector3G<T> operator+(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = v1[i] + v2[i];
    }
    return res;
}

template <class T>
Vector3G<T> operator*(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = v1[i] * v2[i];
    }
    return res;
}

template <class T>
Vector3G<T> operator*(const T &a, const Vector3G<T> &v)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = a * v[i];
    }
    return res;
}

template <class T>
Vector3G<T> operator*(const Vector3G<T> &v, const T &a)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = v[i] * a;
    }
    return res;
}

template <class T>
Vector3G<T> operator/(const T &a, const Vector3G<T> &v)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = a / v[i];
    }
    return res;
}

template <class T>
Vector3G<T> operator/(const Vector3G<T> &v, const T &a)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = v[i] / a;
    }
    return res;
}

template <class T>
Vector3G<T> operator/(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = v1[i] / v2[i];
    }
    return res;
}

template <class T>
Vector3G<T> &operator+=(Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] += rhs[i];
    }
    return lhs;
}

template <class T>
Vector3G<T> &operator-=(Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] -= rhs[i];
    }
    return lhs;
}

template <class T>
Vector3G<T> &operator*=(Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] *= rhs[i];
    }
    return lhs;
}

template <class T>
Vector3G<T> &operator/=(Vector3G<T> &lhs, const Vector3G<T> &rhs)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] /= rhs[i];
    }
    return lhs;
}

template <class T>
Vector3G<T> operator+(const T &a, const Vector3G<T> &v)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = a + v[i];
    }
    return res;
}

template <class T>
Vector3G<T> operator+(const Vector3G<T> &v, const T &a)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = v[i] + a;
    }
    return res;
}

template <class T>
Vector3G<T> operator-(const Vector3G<T> &v1, const Vector3G<T> &v2)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = v1[i] - v2[i];
    }
    return res;
}

template <class T>
Vector3G<T> operator-(const T &a, const Vector3G<T> &v)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = a - v[i];
    }
    return res;
}

template <class T>
Vector3G<T> operator-(const Vector3G<T> &v, const T &a)
{
    Vector3G<T> res;
    for (int i = 0; i < 3; ++i)
    {
        res[i] = v[i] - a;
    }
    return res;
}

template <class T>
Vector3G<T> &operator+=(Vector3G<T> &lhs, const T &a)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] += a;
    }
    return lhs;
}

template <class T>
Vector3G<T> &operator-=(Vector3G<T> &lhs, const T &a)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] -= a;
    }
    return lhs;
}

template <class T>
Vector3G<T> &operator*=(Vector3G<T> &lhs, const T &a)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] *= a;
    }
    return lhs;
}

template <class T>
Vector3G<T> &operator/=(Vector3G<T> &lhs, const T &a)
{
    for (int i = 0; i < 3; ++i)
    {
        lhs[i] /= a;
    }
    return lhs;
}

template <class T>
Vector3G<T> operator-(const Vector3G<T> &v)
{
    return v * SCAST<T>(-1);
}

template <class T>
const Vector3G<T> &Vector3G<T>::Up()
{
    static const Vector3G<T> v =
        Vector3G<T>(SCAST<T>(0), SCAST<T>(1), SCAST<T>(0));
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::Down()
{
    static const Vector3G<T> v =
        Vector3G<T>(SCAST<T>(0), SCAST<T>(-1), SCAST<T>(0));
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::Right()
{
    static const Vector3G<T> v =
        Vector3G<T>(SCAST<T>(1), SCAST<T>(0), SCAST<T>(0));
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::Left()
{
    static const Vector3G<T> v =
        Vector3G<T>(SCAST<T>(-1), SCAST<T>(0), SCAST<T>(0));
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::Zero()
{
    static const Vector3G<T> v = Vector3G<T>(SCAST<T>(0));
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::One()
{
    static const Vector3G<T> v = Vector3G<T>(SCAST<T>(1));
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::Forward()
{
    static const Vector3G<T> v =
        Vector3G<T>(SCAST<T>(0), SCAST<T>(0), SCAST<T>(-1));
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::Back()
{
    static const Vector3G<T> v =
        Vector3G<T>(SCAST<T>(0), SCAST<T>(0), SCAST<T>(1));
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::Infinity()
{
    static const Vector3G<T> v = Vector3G<T>(Math::Infinity<T>());
    return v;
}
template <class T>
const Vector3G<T> &Vector3G<T>::NInfinity()
{
    static const Vector3G<T> v = Vector3G<T>(Math::NegativeInfinity<T>());
    return v;
}

}  // namespace Bang
