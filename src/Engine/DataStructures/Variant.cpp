#include "Bang/Variant.h"

#include "Bang/Color.h"
#include "Bang/GUID.h"
#include "Bang/Object.h"
#include "Bang/ObjectPtr.h"
#include "Bang/Path.h"
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
    SetType(Type::INT);
}

void Variant::SetUint(uint v)
{
    SetInt(SCAST<int>(v));
    SetType(Type::UINT);
}

void Variant::SetFloat(float v)
{
    SetDouble(SCAST<double>(v));
    SetType(Type::FLOAT);
}

void Variant::SetDouble(double v)
{
    m_double = v;
    SetType(Type::DOUBLE);
}

void Variant::SetBool(bool v)
{
    SetInt(SCAST<int>(v));
    SetType(Type::BOOL);
}

void Variant::SetPath(const Path &path)
{
    SetString(path.GetAbsolute());
    SetType(Type::PATH);
}

void Variant::SetGUID(const GUID &guid)
{
    m_guid = guid;
    SetType(Type::GUID);
}

void Variant::SetObjectPtr(const ObjectPtr &objectPtr)
{
    m_objectPtr = objectPtr;
    SetType(Type::OBJECT_PTR);
}

void Variant::SetString(const String &v)
{
    m_string = v;
    SetType(Type::STRING);
}

void Variant::SetColor(const Color &v)
{
    SetVector4(Vector4(v.r, v.g, v.b, v.a));
    SetType(Type::COLOR);
}

void Variant::SetVector2(const Vector2 &v)
{
    SetVector4(Vector4(v.x, v.y, 0, 0));
    SetType(Type::VECTOR2);
}

void Variant::SetVector3(const Vector3 &v)
{
    SetVector4(Vector4(v.x, v.y, v.z, 0));
    SetType(Type::VECTOR3);
}

void Variant::SetVector4(const Vector4 &v)
{
    m_vector4 = v;
    SetType(Type::VECTOR4);
}

void Variant::SetVector2i(const Vector2i &v)
{
    SetVector4i(Vector4i(v.x, v.y, 0, 0));
    SetType(Type::VECTOR2i);
}

void Variant::SetVector3i(const Vector3i &v)
{
    SetVector4i(Vector4i(v.x, v.y, v.z, 0));
    SetType(Type::VECTOR3i);
}

void Variant::SetVector4i(const Vector4i &v)
{
    m_vector4i = v;
    SetType(Type::VECTOR4i);
}

void Variant::SetQuaternion(const Quaternion &v)
{
    SetVector4(Vector4(v.x, v.y, v.z, v.w));
    SetType(Type::QUATERNION);
}

Variant::Type Variant::GetType() const
{
    return m_type;
}

int Variant::GetInt() const
{
    return m_int;
}

uint Variant::GetUint() const
{
    return SCAST<uint>(GetInt());
}

float Variant::GetFloat() const
{
    return SCAST<float>(GetDouble());
}

Path Variant::GetPath() const
{
    return Path(GetString());
}

double Variant::GetDouble() const
{
    return m_double;
}

bool Variant::GetBool() const
{
    return SCAST<bool>(GetInt());
}

const GUID &Variant::GetGUID() const
{
    return m_guid;
}

ObjectPtr Variant::GetObjectPtr() const
{
    return m_objectPtr;
}

String Variant::GetString() const
{
    return m_string;
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

Vector2i Variant::GetVector2i() const
{
    return GetVector4i().xy();
}

Vector3i Variant::GetVector3i() const
{
    return GetVector4i().xyz();
}

Vector4i Variant::GetVector4i() const
{
    return m_vector4i;
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

Variant Variant::FromUint(uint v)
{
    Variant variant;
    variant.SetUint(v);
    return variant;
}

Variant Variant::FromBool(bool v)
{
    Variant variant;
    variant.SetBool(v);
    return variant;
}

Variant Variant::FromPath(const Path &v)
{
    Variant variant;
    variant.SetPath(v);
    return variant;
}

Variant Variant::FromGUID(const GUID &v)
{
    Variant variant;
    variant.SetGUID(v);
    return variant;
}

Variant Variant::FromObjectPtr(const ObjectPtr &ptr)
{
    Variant variant;
    variant.SetObjectPtr(ptr);
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

Variant Variant::FromVector2i(const Vector2i &v)
{
    Variant variant;
    variant.SetVector2i(v);
    return variant;
}

Variant Variant::FromVector3i(const Vector3i &v)
{
    Variant variant;
    variant.SetVector3i(v);
    return variant;
}

Variant Variant::FromVector4i(const Vector4i &v)
{
    Variant variant;
    variant.SetVector4i(v);
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
        case Variant::Type::UINT: return (GetUint() == rhs.GetUint());
        case Variant::Type::BOOL: return (GetBool() == rhs.GetBool());
        case Variant::Type::PATH: return (GetPath() == rhs.GetPath());
        case Variant::Type::STRING: return (GetString() == rhs.GetString());
        case Variant::Type::GUID: return (GetGUID() == rhs.GetGUID());
        case Variant::Type::COLOR: return (GetColor() == rhs.GetColor());
        case Variant::Type::VECTOR2: return (GetVector2() == rhs.GetVector2());
        case Variant::Type::VECTOR3: return (GetVector3() == rhs.GetVector3());
        case Variant::Type::VECTOR4: return (GetVector4() == rhs.GetVector4());
        case Variant::Type::VECTOR2i:
            return (GetVector2i() == rhs.GetVector2i());
        case Variant::Type::VECTOR3i:
            return (GetVector3i() == rhs.GetVector3i());
        case Variant::Type::VECTOR4i:
            return (GetVector4i() == rhs.GetVector4i());
        case Variant::Type::QUATERNION:
            return (GetQuaternion() == rhs.GetQuaternion());
        case Variant::Type::OBJECT_PTR:
            return (GetObjectPtr() == rhs.GetObjectPtr());
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
        case Variant::Type::UINT: return "uint";
        case Variant::Type::BOOL: return "bool";
        case Variant::Type::COLOR: return "Color";
        case Variant::Type::STRING: return "String";
        case Variant::Type::PATH: return "Path";
        case Variant::Type::GUID: return "GUID";
        case Variant::Type::VECTOR2: return "Vector2";
        case Variant::Type::VECTOR3: return "Vector3";
        case Variant::Type::VECTOR4: return "Vector4";
        case Variant::Type::VECTOR2i: return "Vector2i";
        case Variant::Type::VECTOR3i: return "Vector3i";
        case Variant::Type::VECTOR4i: return "Vector4i";
        case Variant::Type::QUATERNION: return "Quaternion";
        case Variant::Type::OBJECT_PTR: return "ObjectPtr";
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
    else if (typeStr == "uint")
    {
        return Variant::Type::UINT;
    }
    else if (typeStr == "bool")
    {
        return Variant::Type::BOOL;
    }
    else if (typeStr == "Color")
    {
        return Variant::Type::COLOR;
    }
    else if (typeStr == "GUID")
    {
        return Variant::Type::GUID;
    }
    else if (typeStr == "Path")
    {
        return Variant::Type::PATH;
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
    else if (typeStr == "Vector2i")
    {
        return Variant::Type::VECTOR2i;
    }
    else if (typeStr == "Vector3i")
    {
        return Variant::Type::VECTOR3i;
    }
    else if (typeStr == "Vector4i")
    {
        return Variant::Type::VECTOR4i;
    }
    else if (typeStr == "Quaternion")
    {
        return Variant::Type::QUATERNION;
    }
    else if (typeStr.BeginsWith("ObjectPtr") || typeStr.EndsWith("*"))
    {
        return Variant::Type::OBJECT_PTR;
    }
    return Variant::Type::NONE;
}

bool Variant::ExistsType(const String &typeStr)
{
    return (Variant::GetTypeFromString(typeStr) != Variant::Type::NONE);
}
