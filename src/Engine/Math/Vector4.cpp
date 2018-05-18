#include "Bang/Vector4.h"

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
const Vector4G<T> Vector4G<T>::Infinity = Vector4G<T>(Math::Max<T>());
template<class T>
const Vector4G<T> Vector4G<T>::NInfinity = Vector4G<T>(Math::Min<T>());
