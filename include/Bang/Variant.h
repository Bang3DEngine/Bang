#ifndef VARIANT_H
#define VARIANT_H

#include "Bang/Bang.h"
#include "Bang/Color.h"
#include "Bang/GUID.h"
#include "Bang/ObjectPtr.h"
#include "Bang/Path.h"
#include "Bang/Quaternion.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

namespace Bang
{
class Variant
{
public:
    enum class Type
    {
        NONE,
        FLOAT,
        DOUBLE,
        PATH,
        INT,
        UINT,
        BOOL,
        STRING,
        GUID,
        OBJECT_PTR,
        COLOR,
        VECTOR2,
        VECTOR3,
        VECTOR4,
        VECTOR2i,
        VECTOR3i,
        VECTOR4i,
        QUATERNION
    };

    Variant() = default;
    ~Variant() = default;

    void SetType(Type type);

    void SetInt(int v);
    void SetUint(uint v);
    void SetFloat(float v);
    void SetDouble(double v);
    void SetBool(bool v);
    void SetPath(const Path &path);
    void SetGUID(const GUID &guid);
    void SetObjectPtr(const ObjectPtr &objectPtr);
    void SetString(const String &v);
    void SetColor(const Color &v);
    void SetVector2(const Vector2 &v);
    void SetVector3(const Vector3 &v);
    void SetVector4(const Vector4 &v);
    void SetVector2i(const Vector2i &v);
    void SetVector3i(const Vector3i &v);
    void SetVector4i(const Vector4i &v);
    void SetQuaternion(const Quaternion &v);

    template <class T>
    void Set(const T &v)
    {
        ASSERT(false);
        BANG_UNUSED(v);
    }

    Type GetType() const;
    int GetInt() const;
    uint GetUint() const;
    float GetFloat() const;
    Path GetPath() const;
    double GetDouble() const;
    bool GetBool() const;
    const GUID &GetGUID() const;
    ObjectPtr GetObjectPtr() const;
    String GetString() const;
    Color GetColor() const;
    Vector2 GetVector2() const;
    Vector3 GetVector3() const;
    Vector4 GetVector4() const;
    Vector2i GetVector2i() const;
    Vector3i GetVector3i() const;
    Vector4i GetVector4i() const;
    Quaternion GetQuaternion() const;

    template <class T>
    T Get() const
    {
        ASSERT(false);
        return T();
    }

    static Variant FromFloat(float v);
    static Variant FromDouble(double v);
    static Variant FromInt(int v);
    static Variant FromUint(uint v);
    static Variant FromBool(bool b);
    static Variant FromPath(const Path &path);
    static Variant FromGUID(const GUID &guid);
    static Variant FromObjectPtr(const ObjectPtr &ptr);
    static Variant FromString(const String &str);
    static Variant FromColor(const Color &color);
    static Variant FromVector2(const Vector2 &v);
    static Variant FromVector3(const Vector3 &v);
    static Variant FromVector4(const Vector4 &v);
    static Variant FromVector2i(const Vector2i &v);
    static Variant FromVector3i(const Vector3i &v);
    static Variant FromVector4i(const Vector4i &v);
    static Variant FromQuaternion(const Quaternion &q);

    template <class T>
    static Variant From(const T &v)
    {
        Variant variant;
        variant.Set<T>(v);
        return variant;
    }

    bool operator==(const Variant &rhs) const;
    bool operator!=(const Variant &rhs) const;

    static String GetTypeToString(Variant::Type type);
    static Variant::Type GetTypeFromString(const String &typeStr);
    static bool ExistsType(const String &typeStr);

private:
    Type m_type = Type::NONE;

    int m_int = 0;
    double m_double = 0.0;
    String m_string = "";
    GUID m_guid = GUID::Empty();
    Vector4 m_vector4 = Vector4::Zero();
    Vector4i m_vector4i = Vector4i::Zero();
    ObjectPtr m_objectPtr;
};

// Set specializations
template <>
inline void Variant::Set<float>(const float &v)
{
    SetFloat(v);
}
template <>
inline void Variant::Set<double>(const double &v)
{
    SetDouble(v);
}
template <>
inline void Variant::Set<bool>(const bool &v)
{
    SetBool(v);
}
template <>
inline void Variant::Set<int>(const int &v)
{
    SetInt(v);
}
template <>
inline void Variant::Set<uint>(const uint &v)
{
    SetUint(v);
}
template <>
inline void Variant::Set<GUID>(const GUID &v)
{
    SetGUID(v);
}
template <>
inline void Variant::Set<Path>(const Path &v)
{
    SetPath(v);
}
template <>
inline void Variant::Set<String>(const String &v)
{
    SetString(v);
}
template <>
inline void Variant::Set<Color>(const Color &v)
{
    SetColor(v);
}
template <>
inline void Variant::Set<Vector2>(const Vector2 &v)
{
    SetVector2(v);
}
template <>
inline void Variant::Set<Vector3>(const Vector3 &v)
{
    SetVector3(v);
}
template <>
inline void Variant::Set<Vector4>(const Vector4 &v)
{
    SetVector4(v);
}
template <>
inline void Variant::Set<Vector2i>(const Vector2i &v)
{
    SetVector2i(v);
}
template <>
inline void Variant::Set<Vector3i>(const Vector3i &v)
{
    SetVector3i(v);
}
template <>
inline void Variant::Set<Vector4i>(const Vector4i &v)
{
    SetVector4i(v);
}
template <>
inline void Variant::Set<Quaternion>(const Quaternion &v)
{
    SetQuaternion(v);
}
template <>
inline void Variant::Set<ObjectPtr>(const ObjectPtr &v)
{
    SetObjectPtr(v);
}

// Get specializations
template <>
inline float Variant::Get() const
{
    return GetFloat();
}
template <>
inline double Variant::Get() const
{
    return GetDouble();
}
template <>
inline Path Variant::Get() const
{
    return GetPath();
}
template <>
inline bool Variant::Get() const
{
    return GetBool();
}
template <>
inline int Variant::Get() const
{
    return GetInt();
}
template <>
inline uint Variant::Get() const
{
    return GetUint();
}
template <>
inline String Variant::Get() const
{
    return GetString();
}
template <>
inline GUID Variant::Get() const
{
    return GetGUID();
}
template <>
inline Color Variant::Get() const
{
    return GetColor();
}
template <>
inline Vector2 Variant::Get() const
{
    return GetVector2();
}
template <>
inline Vector3 Variant::Get() const
{
    return GetVector3();
}
template <>
inline Vector4 Variant::Get() const
{
    return GetVector4();
}
template <>
inline Vector2i Variant::Get() const
{
    return GetVector2i();
}
template <>
inline Vector3i Variant::Get() const
{
    return GetVector3i();
}
template <>
inline Vector4i Variant::Get() const
{
    return GetVector4i();
}
template <>
inline Quaternion Variant::Get() const
{
    return GetQuaternion();
}
template <>
inline ObjectPtr Variant::Get() const
{
    return GetObjectPtr();
}
}  // namespace Bang

#endif  // VARIANT_H
