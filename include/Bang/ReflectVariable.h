#ifndef REFLECTVARIABLE_H
#define REFLECTVARIABLE_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/String.h"
#include "Bang/Variant.h"

namespace Bang
{
class ReflectVariable
{
public:
    ReflectVariable() = default;
    ~ReflectVariable() = default;

    static void FromString(String::Iterator propBegin,
                           String::Iterator propEnd,
                           ReflectVariable *outReflectedVar,
                           bool *success);

    String GetInitializationCode(const String &propInitVarName) const;

    void SetName(const String &name);
    void SetCodeName(const String &varCodeName);
    void SetInitValue(const String &initValue);

    Variant &GetVariant();
    const String &GetName() const;
    const Variant &GetVariant() const;
    const String &GetCodeName() const;
    const String &GetInitValue() const;

    bool operator==(const ReflectVariable &rhs) const;
    bool operator!=(const ReflectVariable &rhs) const;

private:
    Variant m_variant;
    String m_name = "";
    String m_codeName = "";
    String m_initValue = "";

    std::function<void(const Variant &variant)> m_setter;
    std::function<const Variant &()> m_getter;
};
}

#endif  // REFLECTVARIABLE_H
