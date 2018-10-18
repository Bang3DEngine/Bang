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

    using SetterFunc = std::function<void(const Variant &variant)>;
    void SetSetter(std::function<void(const Variant &variant)> setter);

    template <class T>
    void SetSetterT(std::function<void(const T &v)> setter)
    {
        SetSetter(
            [setter](const Variant &variant) { setter(variant.Get<T>()); });
    }

    template <class TClass, class T>
    void SetSetterT(std::function<void(TClass *thisPtr, const T &v)> setter)
    {
        SetSetter(
            [setter](const Variant &variant) { setter(variant.Get<T>()); });
    }

    using GetterFunc = std::function<Variant()>;
    void SetGetter(std::function<Variant()> getter);

    template <class T>
    void SetGetterT(std::function<T()> getter)
    {
        SetGetter([getter]() { return Variant::From<T>(getter()); });
    }

    Variant &GetVariant();
    const String &GetName() const;
    const Variant &GetVariant() const;
    const String &GetCodeName() const;
    const String &GetInitValue() const;
    const SetterFunc &GetSetter() const;
    const GetterFunc &GetGetter() const;

    bool operator==(const ReflectVariable &rhs) const;
    bool operator!=(const ReflectVariable &rhs) const;

private:
    Variant m_variant;
    String m_name = "";
    String m_codeName = "";
    String m_initValue = "";

    SetterFunc m_setter;
    GetterFunc m_getter;
};
}

#endif  // REFLECTVARIABLE_H
