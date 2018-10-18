#ifndef VARIANT_H
#define VARIANT_H

#include "Bang/Bang.h"
#include "Bang/String.h"
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
        INT,
        BOOL,
        STRING,
        COLOR,
        VECTOR2,
        VECTOR3,
        VECTOR4,
        QUATERNION
    };

    Variant() = default;
    ~Variant() = default;

    void SetType(Type type);

    void SetInt(int v);
    void SetFloat(float v);
    void SetDouble(double v);
    void SetBool(bool v);
    void SetString(const String &v);
    void SetColor(const Color &v);
    void SetVector2(const Vector2 &v);
    void SetVector3(const Vector3 &v);
    void SetVector4(const Vector4 &v);
    void SetQuaternion(const Quaternion &v);

    Type GetType() const;
    int GetInt() const;
    float GetFloat() const;
    double GetDouble() const;
    bool GetBool() const;
    String GetString() const;
    Color GetColor() const;
    Vector2 GetVector2() const;
    Vector3 GetVector3() const;
    Vector4 GetVector4() const;
    Quaternion GetQuaternion() const;

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
    Vector4 m_vector4 = Vector4::Zero;
};
}

#endif  // VARIANT_H
