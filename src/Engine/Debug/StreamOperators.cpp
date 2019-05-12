#include "Bang/StreamOperators.h"

#include "BangMath/AABox.h"
#include "BangMath/Color.h"
#include "Bang/ComplexRandom.h"
#include "Bang/GUID.h"
#include "Bang/IToString.h"
#include "Bang/Object.h"
#include "Bang/ObjectPtr.h"
#include "Bang/Path.h"
#include "BangMath/Quad.h"
#include "BangMath/Quaternion.h"
#include "Bang/String.h"
#include "Bang/Time.h"
#include "BangMath/Triangle.h"
#include "Bang/Variant.h"
#include "BangMath/Vector2.h"
#include "BangMath/Vector3.h"
#include "BangMath/Vector4.h"

namespace Bang
{
std::ostream &operator<<(std::ostream &log, const Time &t)
{
    log << t.GetNanos();
    return log;
}

std::ostream &operator<<(std::ostream &log, const Path &p)
{
    log << p.GetAbsolute();
    return log;
}

std::ostream &operator<<(std::ostream &log, const IToString *s)
{
    if (!s)
    {
        log << "Null";
    }
    else
    {
        log << s->ToString();
    }
    return log;
}

std::ostream &operator<<(std::ostream &log, const IToString &v)
{
    log << (&v);
    return log;
}

std::ostream &operator<<(std::ostream &log, const GUID &guid)
{
    log << guid.GetTimeGUID() << " " << guid.GetRandGUID() << " "
        << guid.GetEmbeddedAssetGUID();
    return log;
}

std::ostream &operator<<(std::ostream &log, const ComplexRandom &cr)
{
    log << cr.GetConstantValue() << " ";
    log << cr.GetMinRangeValue() << " ";
    log << cr.GetMaxRangeValue() << " ";
    log << SCAST<uint>(cr.GetType()) << " ";

    return log;
}

std::ostream &operator<<(std::ostream &log, const Variant &variant)
{
    switch (variant.GetType())
    {
        case Variant::Type::FLOAT: log << variant.GetFloat(); break;
        case Variant::Type::DOUBLE: log << variant.GetDouble(); break;
        case Variant::Type::PATH: log << variant.GetPath(); break;
        case Variant::Type::BOOL: log << variant.GetBool(); break;
        case Variant::Type::INT: log << variant.GetInt(); break;
        case Variant::Type::UINT: log << variant.GetUint(); break;
        case Variant::Type::STRING: log << variant.GetString(); break;
        case Variant::Type::GUID: log << variant.GetGUID(); break;
        case Variant::Type::COLOR: log << variant.GetColor(); break;
        case Variant::Type::VECTOR2: log << variant.GetVector2(); break;
        case Variant::Type::VECTOR3: log << variant.GetVector3(); break;
        case Variant::Type::VECTOR4: log << variant.GetVector4(); break;
        case Variant::Type::VECTOR2i: log << variant.GetVector2i(); break;
        case Variant::Type::VECTOR3i: log << variant.GetVector3i(); break;
        case Variant::Type::VECTOR4i: log << variant.GetVector4i(); break;
        case Variant::Type::QUATERNION: log << variant.GetQuaternion(); break;
        case Variant::Type::OBJECT_PTR: log << variant.GetObjectPtr(); break;
        case Variant::Type::NONE: break;

        default: ASSERT(false); break;
    }
    return log;
}

//

std::istream &operator>>(std::istream &is, Time &t)
{
    ConsumeLetters_(is);

    uint64_t timeNanos;
    is >> timeNanos;
    t.SetNanos(timeNanos);
    return is;
}

std::istream &operator>>(std::istream &is, Path &p)
{
    String str;
    is >> str;
    p.SetPath(str);
    return is;
}

std::istream &operator>>(std::istream &is, GUID &guid)
{
    guid.operator>>(is);
    return is;
}

std::istream &operator>>(std::istream &is, ComplexRandom &cr)
{
    uint type = 0;
    float constantValue = 0.0f;
    float minRangeValue = 0.0f;
    float maxRangeValue = 0.0f;
    is >> constantValue;
    is >> minRangeValue;
    is >> maxRangeValue;
    is >> type;

    cr.SetConstantValue(constantValue);
    cr.SetRangeValues(minRangeValue, maxRangeValue);

    ComplexRandomType crType = SCAST<ComplexRandomType>(type);
    cr.SetType(crType);

    return is;
}

std::istream &operator>>(std::istream &is, Variant &variant)
{
    switch (variant.GetType())
    {
        case Variant::Type::FLOAT:
        {
            float v;
            is >> v;
            variant.SetFloat(v);
        }
        break;
        case Variant::Type::DOUBLE:
        {
            double v;
            is >> v;
            variant.SetDouble(v);
        }
        break;
        case Variant::Type::INT:
        {
            int v;
            is >> v;
            variant.SetInt(v);
        }
        break;
        case Variant::Type::UINT:
        {
            uint v;
            is >> v;
            variant.SetUint(v);
        }
        break;
        case Variant::Type::BOOL:
        {
            bool v;
            is >> v;
            variant.SetBool(v);
        }
        break;
        case Variant::Type::PATH:
        {
            String v;
            is >> v;
            variant.SetPath(Path(v));
        }
        break;
        case Variant::Type::STRING:
        {
            String v;
            is >> v;
            variant.SetString(v);
        }
        break;
        case Variant::Type::GUID:
        {
            GUID v;
            is >> v;
            variant.SetGUID(v);
        }
        break;
        case Variant::Type::COLOR:
        {
            Color v;
            is >> v;
            variant.SetColor(v);
        }
        break;

        case Variant::Type::VECTOR2:
        {
            Vector2 v;
            is >> v;
            variant.SetVector2(v);
        }
        break;
        case Variant::Type::VECTOR3:
        {
            Vector3 v;
            is >> v;
            variant.SetVector3(v);
        }
        break;
        case Variant::Type::VECTOR4:
        {
            Vector4 v;
            is >> v;
            variant.SetVector4(v);
        }
        break;

        case Variant::Type::VECTOR2i:
        {
            Vector2i v;
            is >> v;
            variant.SetVector2i(v);
        }
        break;
        case Variant::Type::VECTOR3i:
        {
            Vector3i v;
            is >> v;
            variant.SetVector3i(v);
        }
        break;
        case Variant::Type::VECTOR4i:
        {
            Vector4i v;
            is >> v;
            variant.SetVector4i(v);
        }
        break;

        case Variant::Type::QUATERNION:
        {
            Quaternion v;
            is >> v;
            variant.SetQuaternion(v);
        }
        break;

        case Variant::Type::OBJECT_PTR:
        {
            ObjectPtr v;
            is >> v;
            variant.SetObjectPtr(v);
        }
        break;

        case Variant::Type::NONE: break;

        default: ASSERT(false); break;
    }
    return is;
}
}
