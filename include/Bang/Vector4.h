#ifndef VECTOR4_H
#define VECTOR4_H

#include "Bang/Vector.h"

NAMESPACE_BANG_BEGIN

FORWARD_T class Vector2G;
FORWARD_T class Vector3G;

#define XYZ_MEMBERS x,y,z,w
#define EXTRA_DECLARATIONS \
template <class OtherT1, class OtherT2, class OtherT3, class OtherT4>\
explicit Vector4G(OtherT1 _x, \
                  OtherT2 _y, \
                  OtherT3 _z, \
                  OtherT4 _w) : \
                   x( SCAST<T>(_x) ),\
                   y( SCAST<T>(_y) ),\
                   z( SCAST<T>(_z) ),\
                   w( SCAST<T>(_w) ) {}\
\
template <class OtherT1, class OtherT2, class OtherT3>\
explicit Vector4G(const Vector2G<OtherT1> &v, const OtherT2 &_z, const OtherT3 &_w)\
                        : x( SCAST<T>(v.x) ),\
                          y( SCAST<T>(v.y) ),\
                          z( SCAST<T>(_z) ),\
                          w( SCAST<T>(_w) ) {}\
template <class OtherT1, class OtherT2, class OtherT3>\
explicit Vector4G(const OtherT1 &_x, const Vector2G<OtherT2> &v, const OtherT3 &_w)\
                        : x( SCAST<T>(_x) ),\
                          y( SCAST<T>(v.y) ),\
                          z( SCAST<T>(v.z) ),\
                          w( SCAST<T>(_w) ) {}\
template <class OtherT1, class OtherT2, class OtherT3>\
explicit Vector4G(const OtherT1 &_x, const OtherT2 &_y, const Vector2G<OtherT3> &v)\
                        : x( SCAST<T>(_x) ),\
                          y( SCAST<T>(_y) ),\
                          z( SCAST<T>(v.z) ),\
                          w( SCAST<T>(v.w) ) {}\
template <class OtherT1, class OtherT2>\
explicit Vector4G(const Vector3G<OtherT1> &v, const OtherT2 &_w)\
                        : x( SCAST<T>(v.x) ),\
                          y( SCAST<T>(v.y) ),\
                          z( SCAST<T>(v.z) ),\
                          w( SCAST<T>(_w) ) {}\
template <class OtherT1, class OtherT2>\
explicit Vector4G(const OtherT1 &_x, const Vector3G<OtherT2> &v)\
                        : x( SCAST<T>(_x) ),\
                          y( SCAST<T>(v.y) ),\
                          z( SCAST<T>(v.z) ),\
                          w( SCAST<T>(v.w) ) {}\
\
\
Vector2G<T> xy()  const { return Vector2G<T>(x,y); } \
Vector3G<T> xyz() const { return Vector3G<T>(x,y,z); } \
\
const static Vector4G Up; \
const static Vector4G Down; \
const static Vector4G Right; \
const static Vector4G Left; \
const static Vector4G Zero; \
const static Vector4G One; \
const static Vector4G Forward; \
const static Vector4G Back; \
const static Vector4G Infinity; \
const static Vector4G NInfinity;

CLASS_VECTOR_T(Vector4G, 4)
CLASS_VECTOR_T_IMPL(Vector4G, 4)

template<class T>
const Vector4G<T> Vector4G<T>::Up = Vector4G<T>(SCAST<T>(0),
                                                SCAST<T>(1),
                                                SCAST<T>(0),
                                                SCAST<T>(0));
template<class T>
const Vector4G<T> Vector4G<T>::Down = Vector4G<T>(SCAST<T>(0),
                                                  SCAST<T>(-1),
                                                  SCAST<T>(0),
                                                  SCAST<T>(0));
template<class T>
const Vector4G<T> Vector4G<T>::Right = Vector4G<T>(SCAST<T>(1),
                                                   SCAST<T>(0),
                                                   SCAST<T>(0),
                                                   SCAST<T>(0));
template<class T>
const Vector4G<T> Vector4G<T>::Left = Vector4G<T>(SCAST<T>(-1),
                                                  SCAST<T>(0),
                                                  SCAST<T>(0),
                                                  SCAST<T>(0));
template<class T>
const Vector4G<T> Vector4G<T>::Zero = Vector4G<T>(SCAST<T>(0));
template<class T>
const Vector4G<T> Vector4G<T>::One = Vector4G<T>(SCAST<T>(1));
template<class T>
const Vector4G<T> Vector4G<T>::Forward = Vector4G<T>(SCAST<T>(0),
                                                     SCAST<T>(0),
                                                     SCAST<T>(-1),
                                                     SCAST<T>(0));
template<class T>
const Vector4G<T> Vector4G<T>::Back = Vector4G<T>(SCAST<T>(0),
                                                  SCAST<T>(0),
                                                  SCAST<T>(1),
                                                  SCAST<T>(0));
template<class T>
const Vector4G<T> Vector4G<T>::Infinity = Vector4G<T>(Math::Infinity<T>());
template<class T>
const Vector4G<T> Vector4G<T>::NInfinity = Vector4G<T>(Math::NegativeInfinity<T>());

#undef XYZ_MEMBERS
#undef EXTRA_DECLARATIONS

NAMESPACE_BANG_END

#endif // VECTOR4_H
