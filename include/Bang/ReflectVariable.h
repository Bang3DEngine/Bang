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
    void SetInitValue(const Variant &initValueVariant);
    void SetInitValueString(const String &initValueStr);

    using SetterFunc = std::function<void(const Variant &variant)>;
    void SetSetter(std::function<void(const Variant &variant)> setter);

    template <class T>
    void SetSetterT(std::function<void(const T &v)> setter)
    {
        if (setter)
        {
            SetSetter(
                [setter](const Variant &variant) { setter(variant.Get<T>()); });
        }
    }

    using GetterFunc = std::function<Variant()>;
    void SetGetter(std::function<Variant()> getter);

    template <class T>
    void SetGetterT(std::function<T()> getter)
    {
        if (getter)
        {
            SetGetter([getter]() { return Variant::From<T>(getter()); });
        }
    }

    Variant &GetVariant();
    Variant GetCurrentValue() const;
    const String &GetName() const;
    const Variant &GetVariant() const;
    const String &GetCodeName() const;
    const SetterFunc &GetSetter() const;
    const GetterFunc &GetGetter() const;
    const Variant &GetInitValue() const;
    const String &GetInitValueString() const;

    bool operator==(const ReflectVariable &rhs) const;
    bool operator!=(const ReflectVariable &rhs) const;

private:
    Variant m_variant;
    String m_name = "";
    String m_codeName = "";
    String m_initValueString = "";

    SetterFunc m_setter = nullptr;
    GetterFunc m_getter = nullptr;
};
}

#endif  // REFLECTVARIABLE_H
