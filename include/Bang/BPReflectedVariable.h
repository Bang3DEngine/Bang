#ifndef BPPROPERTY_H
#define BPPROPERTY_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/IToString.h"
#include "Bang/String.h"

namespace Bang
{
class BPReflectedVariable : public IToString
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

    BPReflectedVariable();

    static void FromString(String::Iterator propBegin,
                           String::Iterator propEnd,
                           BPReflectedVariable *outReflectedVar,
                           bool *success);

    String GetInitializationCode(const String &propInitVarName) const;

    void SetName(const String &name);
    void SetType(BPReflectedVariable::Type varType);
    void SetCodeName(const String &varCodeName);
    void SetInitValue(const String &initValue);

    const String &GetName() const;
    BPReflectedVariable::Type GetType() const;
    const String &GetCodeName() const;
    const String &GetInitValue() const;

    static String GetTypeToString(BPReflectedVariable::Type type);
    static BPReflectedVariable::Type GetTypeFromString(const String &typeStr);
    static bool ExistsType(const String &typeStr);

private:
    String m_name = "";
    BPReflectedVariable::Type m_variableType = BPReflectedVariable::Type::FLOAT;
    String m_codeName = "";
    String m_initValue = "";

    String ToString() const override;
};
}

#endif  // BPPROPERTY_H
