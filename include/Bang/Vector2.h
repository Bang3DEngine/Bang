#ifndef VECTOR2_H
#define VECTOR2_H

#include "Bang/Math.h"
#include "Bang/Axis.h"
#include "Bang/TypeTraits.h"

NAMESPACE_BANG_BEGIN

template<class T>
class Vector2G
{
public:
    const static Vector2G Up;
    const static Vector2G Down;
    const static Vector2G Right;
    const static Vector2G Left;
    const static Vector2G Zero;
    const static Vector2G One;
    const static Vector2G Infinity;
    const static Vector2G NInfinity;

    T x,y;
   
    Vector2G();
    explicit Vector2G(const T& a);
    template <class OtherT1, class OtherT2>
    explicit Vector2G(const OtherT1 &_x, const OtherT2 &_y);

    template<class OtherT>
    explicit Vector2G(const Vector2G<OtherT>& v);

    T Length() const;  
    T SqLength() const;
    void Normalize();  

    Vector2G NormalizedSafe() const;
    Vector2G Normalized() const;    

    Vector2G ToDegrees() const;
    Vector2G ToRadians() const;

    T Distance(const Vector2G<T> &p) const;
    T SqDistance(const Vector2G<T> &p) const;

    Vector2G<T> Perpendicular() const;
    Vector2G<T> Abs() const;

    T &At(std::size_t i);
    const T &At(std::size_t i) const;

    T* Data();
    const T* Data() const;

    static T Dot(const Vector2G<T> &v1, const Vector2G<T> &v2);

    static T Distance(const Vector2G<T> &v1, const Vector2G<T> &v2);
    static T SqDistance(const Vector2G<T> &v1, const Vector2G<T> &v2);

    static Vector2G<T> Abs(const Vector2G<T> &v);
    static Vector2G<T> Max(const Vector2G<T> &v1, const Vector2G<T> &v2);
    static Vector2G<T> Min(const Vector2G<T> &v1, const Vector2G<T> &v2);

    static Vector2G<T> Floor(const Vector2G<T> &v1);     
    static Vector2G<T> Ceil(const Vector2G<T> &v1);      
    static Vector2G<T> Round(const Vector2G<T> &v1);     
    static Vector2G<T> Clamp(const Vector2G<T> &v,
                             const Vector2G<T> &min,
                             const Vector2G<T> &max);    
    static Vector2G<T> Clamp2(const Vector2G<T> &v,
                              const Vector2G<T> &bound1,
                              const Vector2G<T> &bound2);
   
    template<class Real>
    static Vector2G<T> Lerp(const Vector2G<T> &v1,
                            const Vector2G<T> &v2,
                            Real t);
   
    Axis GetAxis() const;
    const T& GetAxis(Axis axis) const;
    static Vector2G<T> FromAxis(Axis axis);
   
    T& operator[](std::size_t i);
    const T& operator[](std::size_t i) const;
   
   static T Cross(const Vector2G<T> &v1, const Vector2G<T> &v2);
};


template<class T>
bool operator==(const Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
bool operator!=(const Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
bool operator<(const Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
bool operator<=(const Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
bool operator>(const Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
bool operator>=(const Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
Vector2G<T> operator+(const Vector2G<T> & v1, const Vector2G<T> &v2);

template<class T>
Vector2G<T> operator*(const Vector2G<T> &v1, const Vector2G<T> &v2);

template<class T>
Vector2G<T> operator*(const T& a, const Vector2G<T> &v);

template<class T>
Vector2G<T> operator*(const Vector2G<T> &v, const T& a);

template<class T>
Vector2G<T> operator/(const T& a, const Vector2G<T> &v);

template<class T>
Vector2G<T> operator/(const Vector2G<T> &v, const T& a);

template<class T>
Vector2G<T> operator/(const Vector2G<T> &v1, const Vector2G<T> &v2);

template<class T>
Vector2G<T> &operator+=(Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
Vector2G<T> &operator-=(Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
Vector2G<T> &operator*=(Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
Vector2G<T> &operator/=(Vector2G<T> &lhs, const Vector2G<T> &rhs);

template<class T>
Vector2G<T> operator+(const T& a, const Vector2G<T> &v);

template<class T>
Vector2G<T> operator+(const Vector2G<T> &v, const T& a);

template<class T>
Vector2G<T> operator-(const Vector2G<T> &v1, const Vector2G<T> &v2);

template<class T>
Vector2G<T> operator-(const T& a, const Vector2G<T> &v);

template<class T>
Vector2G<T> operator-(const Vector2G<T> &v, const T& a);

template<class T>
Vector2G<T>& operator+=(Vector2G<T> &lhs, const T& a);

template<class T>
Vector2G<T>& operator-=(Vector2G<T> &lhs, const T& a);

template<class T>
Vector2G<T>& operator*=(Vector2G<T> &lhs, const T& a);

template<class T>
Vector2G<T>& operator/=(Vector2G<T> &lhs, const T& a);

template<class T>
Vector2G<T> operator-(const Vector2G<T> &v);

NAMESPACE_BANG_END

#include "Bang/Vector2.tcc"

#endif // VECTOR2_H
