#ifndef REFLECTVARIABLE_H
#define REFLECTVARIABLE_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/ReflectVariableHints.h"
#include "Bang/String.h"
#include "Bang/Variant.h"

namespace Bang
{
class ReflectVariable
{
public:
    ReflectVariable() = default;
    ~ReflectVariable() = default;

    static void FromString(String::Iterator varBegin,
                           String::Iterator varEnd,
                           ReflectVariable *outReflectedVar,
                           bool *success);

    void SetName(const String &name);
    void SetCodeName(const String &varCodeName);
    void SetTypeString(const String &typeString);
    void SetInitValue(const Variant &initValueVariant);
    void SetInitValueString(const String &initValueStr);
    void SetHints(const ReflectVariableHints &hints);

    using SetterFunc = std::function<void(const Variant &variant)>;
    void SetSetter(std::function<void(const Variant &variant)> setter);

    template <class T>
    void SetSetterT(std::function<void(T)> setter)
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
            SetGetter([getter]() {
                return Variant::From<typename std::remove_const<
                    typename std::remove_reference<T>::type>::type>(getter());
            });
        }
    }

    Variant &GetVariant();
    Variant GetCurrentValue() const;
    ReflectVariableHints *GetHintsPtr();
    const String &GetName() const;
    const Variant &GetVariant() const;
    const String &GetCodeName() const;
    SetterFunc GetSetter() const;
    GetterFunc GetGetter() const;
    const String &GetTypeString() const;
    const Variant &GetInitValue() const;
    const String &GetInitValueString() const;
    const ReflectVariableHints &GetHints() const;

    bool EqualsWithoutValue(const ReflectVariable &rhs) const;
    bool operator==(const ReflectVariable &rhs) const;
    bool operator!=(const ReflectVariable &rhs) const;

private:
    Variant m_variant;
    String m_name = "";
    String m_typeString = "";
    String m_codeName = "";
    String m_initValueString = "";
    ReflectVariableHints m_hints;

    SetterFunc m_setter = nullptr;
    GetterFunc m_getter = nullptr;
};
}  // namespace Bang

#endif  // REFLECTVARIABLE_H
