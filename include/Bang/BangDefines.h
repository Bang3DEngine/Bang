#ifndef BANGFORWARD_H
#define BANGFORWARD_H

#include <cstdint>
#include <stddef.h>

#define NAMESPACE_NAME       Bang
#define USING_NAMESPACE_BANG using namespace NAMESPACE_NAME;
#define NAMESPACE_BANG_BEGIN namespace NAMESPACE_NAME {
#define NAMESPACE_BANG_END   }

NAMESPACE_BANG_BEGIN

#ifndef NDEBUG
#define DEBUG
#endif

// Casts ===============================================
#define SCAST static_cast
#define DCAST dynamic_cast
#define RCAST reinterpret_cast
#define GLCAST(x) static_cast<GLenum>(x)
// =====================================================


// Frequent class forwardings ==========================
#define FORWARD
#define FORWARD_T  template<class>
#define FORWARD_TT template<class,class>

NAMESPACE_BANG_END

FORWARD namespace std { FORWARD template<class, size_t> class array; }

NAMESPACE_BANG_BEGIN

FORWARD class Mesh;
FORWARD class Path;
FORWARD class Color;
FORWARD class AABox;
FORWARD class String;
FORWARD class Sphere;
FORWARD class XMLNode;
FORWARD class Material;
FORWARD class Transform;
FORWARD class GameObject;
FORWARD class ICloneable;

FORWARD_TT class Map;
FORWARD_T  class Set;
FORWARD_T  class List;
FORWARD_T  class Array;

FORWARD   class File;
FORWARD_T class RectG;
FORWARD_T class AARectG;
FORWARD_T class Vector2G;
FORWARD_T class Vector3G;
FORWARD_T class Vector4G;
FORWARD_T class Matrix3G;
FORWARD_T class Matrix4G;
FORWARD_T class QuaternionG;
//====================================================


// Types ================================================
using uint        = unsigned int;
using Byte        = unsigned char;
using GLId        = uint;
using BoolByte    = Byte;
using EpochTime   = long long;

using Vector2i    = Vector2G<int>;
using Vector2f    = Vector2G<float>;
using Vector2d    = Vector2G<double>;
using Vector2     = Vector2f;

using Vector3i    = Vector3G<int>;
using Vector3f    = Vector3G<float>;
using Vector3d    = Vector3G<double>;
using Vector3     = Vector3f;

using Vector4i    = Vector4G<int>;
using Vector4f    = Vector4G<float>;
using Vector4d    = Vector4G<double>;
using Vector4     = Vector4f;

using Quaternionf = QuaternionG<float>;
using Quaterniond = QuaternionG<double>;
using Quaternion  = Quaternionf;

using Matrix4f    = Matrix4G<float>;
using Matrix4d    = Matrix4G<double>;
using Matrix4     = Matrix4f;

using Matrix3f    = Matrix3G<float>;
using Matrix3d    = Matrix3G<double>;
using Matrix3     = Matrix3f;

using AARectf     = AARectG<float>;
using AARectd     = AARectG<double>;
using AARecti     = AARectG<int>;
using AARect      = AARectf;

using Rectf       = RectG<float>;
using Rectd       = RectG<double>;
using Recti       = RectG<int>;
using Rect        = Rectf;
using RectPoints  = std::array<Vector2, 4>;

using SDL_GLContext = void*;

// ======================================================

// Other ================================================
template<class EnumT> constexpr EnumT Undef() { return SCAST<EnumT>(-1); }
// ======================================================

#ifdef BUILD_STATIC
    #define BANG_SDL2_INCLUDE(file)     <file>
    #define BANG_SDL2_TTF_INCLUDE(file) <file>
#else
    #define BANG_SDL2_INCLUDE(file)     <SDL2/file>
    #define BANG_SDL2_TTF_INCLUDE(file) <SDL2/file>
#endif

NAMESPACE_BANG_END

#endif // BANGFORWARD_H
