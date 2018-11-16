#ifndef BANGDEFINES_H
#define BANGDEFINES_H

#include <cstddef>
#include <cstdint>

#ifndef DEBUG

#ifndef NDEBUG
#define DEBUG
#endif

#ifdef _DEBUG
#define DEBUG
#endif

#endif

#define BANG_UNUSED(v1) ((void)v1)
#define BANG_UNUSED_2(v1, v2) \
    BANG_UNUSED(v1);          \
    ((void)v2)
#define BANG_UNUSED_3(v1, v2, v3) \
    BANG_UNUSED_2(v1, v2);        \
    ((void)v3)
#define BANG_UNUSED_4(v1, v2, v3, v4) \
    BANG_UNUSED_3(v1, v2, v3);        \
    ((void)v4)
#define BANG_UNUSED_5(v1, v2, v3, v4, v5) \
    BANG_UNUSED_4(v1, v2, v3, v4);        \
    ((void)v5)

// Casts ===============================================
#define SCAST static_cast
#define DCAST dynamic_cast
#define RCAST reinterpret_cast
#define GLCAST(x) static_cast<GLenum>(x)
// =====================================================

// Frequent class forwardings ==========================

namespace std
{
template <class, size_t>
class array;
}

namespace Bang
{
class Mesh;
class Path;
class Color;
class AABox;
class String;
class Sphere;
class MetaNode;
class Material;
class Transform;
class GameObject;
class ICloneable;

template <class, class, class>
class Map;
template <class, class>
class Set;
template <class>
class List;
template <class>
class Array;
template <class, class, class, class>
class USet;
template <class, class, class, class, class>
class UMap;

class File;
template <class>
class RectG;
template <class>
class AARectG;
template <class>
class Vector2G;
template <class>
class Vector3G;
template <class>
class Vector4G;
template <class>
class Matrix3G;
template <class>
class Matrix4G;
template <class>
class QuaternionG;
//====================================================

// Types ================================================
using uint = unsigned int;
using Byte = unsigned char;
using GLId = uint;
using BoolByte = Byte;
using EpochTime = long long;

using Vector2i = Vector2G<int>;
using Vector2f = Vector2G<float>;
using Vector2d = Vector2G<double>;
using Vector2 = Vector2f;

using Vector3i = Vector3G<int>;
using Vector3f = Vector3G<float>;
using Vector3d = Vector3G<double>;
using Vector3 = Vector3f;

using Vector4i = Vector4G<int>;
using Vector4f = Vector4G<float>;
using Vector4d = Vector4G<double>;
using Vector4 = Vector4f;

using Quaternionf = QuaternionG<float>;
using Quaterniond = QuaternionG<double>;
using Quaternion = Quaternionf;

using Matrix4f = Matrix4G<float>;
using Matrix4d = Matrix4G<double>;
using Matrix4 = Matrix4f;

using Matrix3f = Matrix3G<float>;
using Matrix3d = Matrix3G<double>;
using Matrix3 = Matrix3f;

using AARectf = AARectG<float>;
using AARectd = AARectG<double>;
using AARecti = AARectG<int>;
using AARect = AARectf;

using Rectf = RectG<float>;
using Rectd = RectG<double>;
using Recti = RectG<int>;
using Rect = Rectf;
using RectPoints = std::array<Vector2, 4>;

}  // namespace Bang

using SDL_GLContext = void *;

// ======================================================

// Other ================================================
template <class EnumT>
constexpr EnumT Undef()
{
    return SCAST<EnumT>(-1);
}

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};
// ======================================================

#endif  // BANGDEFINES_H
