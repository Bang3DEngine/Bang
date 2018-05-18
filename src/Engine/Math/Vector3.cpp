#include "Bang/Vector3.h"

template<class T>
const Vector3G<T> Vector3G<T>::Up = Vector3G<T>(SCAST<T>(0),
                                                SCAST<T>(1),
                                                SCAST<T>(0));
template<class T>
const Vector3G<T> Vector3G<T>::Down = Vector3G<T>(SCAST<T>(0),
                                                SCAST<T>(-1),
                                                SCAST<T>(0));
template<class T>
const Vector3G<T> Vector3G<T>::Right = Vector3G<T>(SCAST<T>(1),
                                                   SCAST<T>(0),
                                                   SCAST<T>(0));
template<class T>
const Vector3G<T> Vector3G<T>::Left = Vector3G<T>(SCAST<T>(-1),
                                                  SCAST<T>(0),
                                                  SCAST<T>(0));
template<class T>
const Vector3G<T> Vector3G<T>::Zero = Vector3G<T>(SCAST<T>(0));
template<class T>
const Vector3G<T> Vector3G<T>::One = Vector3G<T>(SCAST<T>(1));
template<class T>
const Vector3G<T> Vector3G<T>::Forward = Vector3G<T>(SCAST<T>(0),
                                                     SCAST<T>(0),
                                                     SCAST<T>(-1));
template<class T>
const Vector3G<T> Vector3G<T>::Back = Vector3G<T>(SCAST<T>(0),
                                                  SCAST<T>(0),
                                                  SCAST<T>(1));
template<class T>
const Vector3G<T> Vector3G<T>::Infinity = Vector3G<T>(Math::Max<T>());
template<class T>
const Vector3G<T> Vector3G<T>::NInfinity = Vector3G<T>(Math::Min<T>());
