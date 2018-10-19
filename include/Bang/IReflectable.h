#ifndef IREFLECTABLE_H
#define IREFLECTABLE_H

#include "Bang/BangDefines.h"
#include "Bang/ReflectStruct.h"
#include "Bang/ReflectVariable.h"
#include "Bang/ReflectVariableHints.h"

namespace Bang
{
#define BANG_REFLECT_VARIABLE(...)
#define BANG_REFLECT_CLASS(...)
#define BANG_REFLECT_STRUCT(...)
#define BANG_REFLECT_DEFINITIONS(...)

#define BANG_REFLECT_VAR_MEMBER_HINTED(Class, Name, Suffix, Hints) \
    ReflectVarMember(                                              \
        Name, &Class::Set##Suffix, &Class::Get##Suffix, this, Hints);

#define BANG_REFLECT_VAR_MEMBER(Class, Name, Suffix) \
    BANG_REFLECT_VAR_MEMBER_HINTED(Class, Name, Suffix, "")

#define BANG_REFLECT_VAR_MEMBER_ENUM(Class, Name, Suffix) \
    ReflectVarMemberEnum(Name,                            \
                         &Class::Set##Suffix,             \
                         &Class::Get##Suffix,             \
                         this,                            \
                         BANG_REFLECT_HINT_ENUM());

#define BANG_REFLECT_HINT_ENUM_FIELD(enumName, enumFieldName) \
    GetReflectStructPtr()->AddEnumField(enumName, enumFieldName)

#define BANG_REFLECT_HINT_ENUM_FIELD_VALUE(   \
    enumName, enumFieldName, enumFieldValue)  \
    GetReflectStructPtr()->AddEnumFieldValue( \
        enumName, enumFieldName, enumFieldValue)

class ReflectStruct;

class IReflectable
{
public:
    const ReflectStruct &GetReflectStruct() const;

protected:
    IReflectable() = default;
    virtual ~IReflectable() = default;

    template <class T>
    void ReflectVar(const String &varName,
                    std::function<void(T)> setter,
                    std::function<T()> getter,
                    const String &hintsString = "");

    template <class TClass, class T, class TToCastTo = T>
    void ReflectVarMember(const String &varName,
                          void (TClass::*setter)(T),
                          T (TClass::*getter)() const,
                          TClass *instance,
                          const String &hintsString = "");

    template <class TClass, class T>
    void ReflectVarMemberEnum(const String &varName,
                              void (TClass::*setter)(T),
                              T (TClass::*getter)() const,
                              TClass *instance,
                              const String &hintsString = "");

    virtual void Reflect();
    ReflectStruct *GetReflectStructPtr() const;

private:
    mutable bool m_alreadyReflected = false;
    mutable ReflectStruct m_reflectStruct;
};

template <class T>
void IReflectable::ReflectVar(const String &varName,
                              std::function<void(T)> setter,
                              std::function<T()> getter,
                              const String &hintsString)
{
    ReflectVariable reflVar;
    reflVar.SetName(varName);
    reflVar.SetSetterT<T>(setter);
    reflVar.SetGetterT<T>(getter);
    reflVar.GetVariant().Set<T>(getter ? getter() : T());

    ReflectVariableHints hints(hintsString);
    reflVar.SetHints(hints);

    GetReflectStructPtr()->AddVariable(reflVar);
}

template <class TClass, class T, class TToCastTo>
void IReflectable::ReflectVarMember(const String &varName,
                                    void (TClass::*setter)(T),
                                    T (TClass::*getter)() const,
                                    TClass *instance,
                                    const String &hintsString)
{
    ReflectVar<TToCastTo>(
        varName,
        [instance, setter](TToCastTo v) { (instance->*setter)(SCAST<T>(v)); },
        [instance, getter]() -> TToCastTo {
            return SCAST<TToCastTo>((instance->*getter)());
        },
        hintsString);
}
template <class TClass, class T>
void IReflectable::ReflectVarMemberEnum(const String &varName,
                                        void (TClass::*setter)(T),
                                        T (TClass::*getter)() const,
                                        TClass *instance,
                                        const String &hintsString)
{
    ReflectVarMember<TClass, T, int>(
        varName, setter, getter, instance, hintsString);
}
}

#endif  // IREFLECTABLE_H
