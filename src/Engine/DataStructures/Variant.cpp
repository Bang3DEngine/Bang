#include "Bang/Variant.h"

#include "Bang/Color.h"
#include "Bang/Quaternion.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/Vector3.h"
#include "Bang/Vector4.h"

using namespace Bang;

void Variant::SetType(Variant::Type type)
{
    m_type = type;
}

void Variant::SetInt(int v)
{
    m_int = v;
    m_type = Type::INT;
}

void Variant::SetFloat(float v)
{
    SetDouble(SCAST<double>(v));
    m_type = Type::FLOAT;
}

void Variant::SetDouble(double v)
{
    m_double = v;
    m_type = Type::DOUBLE;
}

void Variant::SetBool(bool v)
{
    SetInt(SCAST<int>(v));
    m_type = Type::BOOL;
}

void Variant::SetString(const String &v)
{
    m_string = v;
    m_type = Type::STRING;
}

void Variant::SetColor(const Color &v)
{
    SetVector4(Vector4(v.r, v.g, v.b, v.a));
    m_type = Type::COLOR;
}

void Variant::SetVector2(const Vector2 &v)
{
    SetVector4(Vector4(v.x, v.y, 0, 0));
    m_type = Type::VECTOR2;
}

void Variant::SetVector3(const Vector3 &v)
{
    SetVector4(Vector4(v.x, v.y, v.z, 0));
    m_type = Type::VECTOR3;
}

void Variant::SetVector4(const Vector4 &v)
{
    m_vector4 = v;
    m_type = Type::VECTOR4;
}

void Variant::SetQuaternion(const Quaternion &v)
{
    SetVector4(Vector4(v.x, v.y, v.z, v.w));
    m_type = Type::QUATERNION;
}

Variant::Type Variant::GetType() const
{
    return m_type;
}

int Variant::GetInt() const
{
    return m_int;
}

float Variant::GetFloat() const
{
    return SCAST<float>(GetDouble());
}

double Variant::GetDouble() const
{
    return m_double;
}

bool Variant::GetBool() const
{
    return SCAST<bool>(GetInt());
}

String Variant::GetString() const
{
    return GetString();
}

Color Variant::GetColor() const
{
    return Color(GetVector4());
}

Vector2 Variant::GetVector2() const
{
    return GetVector4().xy();
}

Vector3 Variant::GetVector3() const
{
    return GetVector4().xyz();
}

Vector4 Variant::GetVector4() const
{
    return m_vector4;
}

Quaternion Variant::GetQuaternion() const
{
    Vector4 v4 = GetVector4();
    return Quaternion(v4.x, v4.y, v4.z, v4.w);
}

Variant Variant::FromFloat(float v)
{
    Variant variant;
    variant.SetFloat(v);
    return variant;
}

Variant Variant::FromDouble(double v)
{
    Variant variant;
    variant.SetDouble(v);
    return variant;
}

Variant Variant::FromInt(int v)
{
    Variant variant;
    variant.SetInt(v);
    return variant;
}

Variant Variant::FromBool(bool v)
{
    Variant variant;
    variant.SetBool(v);
    return variant;
}

Variant Variant::FromString(const String &v)
{
    Variant variant;
    variant.SetString(v);
    return variant;
}

Variant Variant::FromColor(const Color &v)
{
    Variant variant;
    variant.SetColor(v);
    return variant;
}

Variant Variant::FromVector2(const Vector2 &v)
{
    Variant variant;
    variant.SetVector2(v);
    return variant;
}

Variant Variant::FromVector3(const Vector3 &v)
{
    Variant variant;
    variant.SetVector3(v);
    return variant;
}

Variant Variant::FromVector4(const Vector4 &v)
{
    Variant variant;
    variant.SetVector4(v);
    return variant;
}

Variant Variant::FromQuaternion(const Quaternion &v)
{
    Variant variant;
    variant.SetQuaternion(v);
    return variant;
}

bool Variant::operator==(const Variant &rhs) const
{
    if (GetType() != rhs.GetType())
    {
        return false;
    }

    switch (GetType())
    {
        case Variant::Type::FLOAT: return (GetFloat() == rhs.GetFloat());
        case Variant::Type::DOUBLE: return (GetDouble() == rhs.GetDouble());
        case Variant::Type::INT: return (GetInt() == rhs.GetInt());
        case Variant::Type::BOOL: return (GetBool() == rhs.GetBool());
        case Variant::Type::STRING: return (GetString() == rhs.GetString());
        case Variant::Type::COLOR: return (GetColor() == rhs.GetColor());
        case Variant::Type::VECTOR2: return (GetVector2() == rhs.GetVector2());
        case Variant::Type::VECTOR3: return (GetVector3() == rhs.GetVector3());
        case Variant::Type::VECTOR4: return (GetVector4() == rhs.GetVector4());
        case Variant::Type::QUATERNION:
            return (GetQuaternion() == rhs.GetQuaternion());
        case Variant::Type::NONE: return true;

        default: break;
    }

    ASSERT(false);
    return true;
}

bool Variant::operator!=(const Variant &rhs) const
{
    return !(*this == rhs);
}

String Variant::GetTypeToString(Variant::Type type)
{
    switch (type)
    {
        case Variant::Type::FLOAT: return "float";
        case Variant::Type::DOUBLE: return "double";
        case Variant::Type::INT: return "int";
        case Variant::Type::BOOL: return "bool";
        case Variant::Type::COLOR: return "Color";
        case Variant::Type::STRING: return "String";
        case Variant::Type::VECTOR2: return "Vector2";
        case Variant::Type::VECTOR3: return "Vector3";
        case Variant::Type::VECTOR4: return "Vector4";
        case Variant::Type::QUATERNION: return "Quaternion";
        case Variant::Type::NONE: return "None";

        default: break;
    }

    ASSERT(false);
    return "";
}

Variant::Type Variant::GetTypeFromString(const String &typeStr)
{
    if (typeStr == "float")
    {
        return Variant::Type::FLOAT;
    }
    else if (typeStr == "double")
    {
        return Variant::Type::DOUBLE;
    }
    else if (typeStr == "int")
    {
        return Variant::Type::INT;
    }
    else if (typeStr == "bool")
    {
        return Variant::Type::BOOL;
    }
    else if (typeStr == "Color")
    {
        return Variant::Type::COLOR;
    }
    else if (typeStr == "String")
    {
        return Variant::Type::STRING;
    }
    else if (typeStr == "Vector2")
    {
        return Variant::Type::VECTOR2;
    }
    else if (typeStr == "Vector3")
    {
        return Variant::Type::VECTOR3;
    }
    else if (typeStr == "Vector4")
    {
        return Variant::Type::VECTOR4;
    }
    else if (typeStr == "Quaternion")
    {
        return Variant::Type::QUATERNION;
    }
    return Variant::Type::NONE;
}

bool Variant::ExistsType(const String &typeStr)
{
    return (Variant::GetTypeFromString(typeStr) != Variant::Type::NONE);
}
