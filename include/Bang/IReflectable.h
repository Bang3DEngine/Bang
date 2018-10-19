#ifndef IREFLECTABLE_H
#define IREFLECTABLE_H

#include "Bang/BangDefines.h"
#include "Bang/ReflectStruct.h"

namespace Bang
{
#define BANG_REFLECT_VARIABLE(...)
#define BANG_REFLECT_CLASS(...)
#define BANG_REFLECT_STRUCT(...)
#define BANG_REFLECT_DEFINITIONS(...)

#define BANG_REFLECT_VAR_MEMBER_0(Class, Name, Suffix) \
    ReflectVarMember(Name, &Class::Set##Suffix, &Rope::Get##Suffix, this);
#define BANG_REFLECT_VAR_MEMBER_1(Class, Name, Setter, Getter) \
    ReflectVarMember(Name, &Class::Setter, &Rope::Getter, this);

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
                    std::function<T()> getter);

    template <class TClass, class T>
    void ReflectVarMember(const String &varName,
                          void (TClass::*setter)(T),
                          T (TClass::*getter)() const,
                          TClass *instance);

    virtual void Reflect();
    ReflectStruct *GetReflectStructPtr() const;

private:
    mutable bool m_alreadyReflected = false;
    mutable ReflectStruct m_reflectStruct;
};

template <class T>
void IReflectable::ReflectVar(const String &varName,
                              std::function<void(T)> setter,
                              std::function<T()> getter)
{
    ReflectVariable reflVar;
    reflVar.SetName(varName);
    reflVar.SetSetterT<T>(setter);
    reflVar.SetGetterT<T>(getter);
    reflVar.GetVariant().Set<T>(getter ? getter() : T());
    GetReflectStructPtr()->AddVariable(reflVar);
}

template <class TClass, class T>
void IReflectable::ReflectVarMember(const String &varName,
                                    void (TClass::*setter)(T),
                                    T (TClass::*getter)() const,
                                    TClass *instance)
{
    ReflectVariable reflVar;
    reflVar.SetName(varName);
    reflVar.SetSetterT<T>([instance, setter](T v) { (instance->*setter)(v); });
    reflVar.SetGetterT<T>(
        [instance, getter]() { return (instance->*getter)(); });
    reflVar.GetVariant().Set<T>(getter ? (instance->*getter)() : T());
    GetReflectStructPtr()->AddVariable(reflVar);
}
}

#endif  // IREFLECTABLE_H
