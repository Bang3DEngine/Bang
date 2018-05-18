#include "Bang/Vector2.h"

template<class T>
const Vector2G<T> Vector2G<T>::Up = Vector2G<T>(SCAST<T>(0), SCAST<T>(1));
template<class T>
const Vector2G<T> Vector2G<T>::Down = Vector2G<T>(SCAST<T>(0), SCAST<T>(-1));
template<class T>
const Vector2G<T> Vector2G<T>::Right = Vector2G<T>(SCAST<T>(1), SCAST<T>(0));
template<class T>
const Vector2G<T> Vector2G<T>::Left = Vector2G<T>(SCAST<T>(-1), SCAST<T>(0));
template<class T>
const Vector2G<T> Vector2G<T>::Zero = Vector2G<T>(SCAST<T>(0));
template<class T>
const Vector2G<T> Vector2G<T>::One = Vector2G<T>(SCAST<T>(1));
template<class T>
const Vector2G<T> Vector2G<T>::Infinity = Vector2G<T>(Math::Max<T>());
template<class T>
const Vector2G<T> Vector2G<T>::NInfinity = Vector2G<T>(Math::Min<T>());