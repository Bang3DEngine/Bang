#ifndef REFLECTVARIABLE_H
#define REFLECTVARIABLE_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/IToString.h"
#include "Bang/String.h"

namespace Bang
{
class ReflectVariable : public IToString
{
public:
    enum class Type
    {
        NONE,
        FLOAT,
        DOUBLE,
        INT,
        BOOL,
        COLOR,
        STRING,
        VECTOR2,
        VECTOR3,
        VECTOR4,
        QUATERNION
    };

    ReflectVariable();

    static void FromString(String::Iterator propBegin,
                           String::Iterator propEnd,
                           ReflectVariable *outReflectedVar,
                           bool *success);

    String GetInitializationCode(const String &propInitVarName) const;

    void SetName(const String &name);
    void SetType(ReflectVariable::Type varType);
    void SetCodeName(const String &varCodeName);
    void SetInitValue(const String &initValue);

    const String &GetName() const;
    ReflectVariable::Type GetType() const;
    const String &GetCodeName() const;
    const String &GetInitValue() const;

    static String GetTypeToString(ReflectVariable::Type type);
    static ReflectVariable::Type GetTypeFromString(const String &typeStr);
    static bool ExistsType(const String &typeStr);

    bool operator==(const ReflectVariable &rhs) const;
    bool operator!=(const ReflectVariable &rhs) const;

private:
    String m_name = "";
    ReflectVariable::Type m_variableType = ReflectVariable::Type::FLOAT;
    String m_codeName = "";
    String m_initValue = "";

    String ToString() const override;
};
}

#endif  // REFLECTVARIABLE_H
