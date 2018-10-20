#ifndef IREFLECTABLE_H
#define IREFLECTABLE_H

#include "Bang/BangDefines.h"
#include "Bang/GUID.h"
#include "Bang/ReflectMacros.h"
#include "Bang/ReflectStruct.h"
#include "Bang/ReflectVariable.h"
#include "Bang/ReflectVariableHints.h"

namespace Bang
{
class ReflectStruct;
class Resource;
class GUID;

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
    template <class TClass, class T, class TToCastTo = T>
    void ReflectVarMember(const String &varName,
                          void (TClass::*setter)(const T &),
                          const T &(TClass::*getter)() const,
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

    static Resource *LoadResourceFromGUID(const GUID &guid);
};

template <class T>
void IReflectable::ReflectVar(const String &varName,
                              std::function<void(T)> setter,
                              std::function<T()> getter,
                              const String &hintsString)
{
    ASSERT(setter);
    ASSERT(getter);

    ReflectVariable reflVar;
    reflVar.SetName(varName);
    reflVar.SetSetterT<T>(setter);
    reflVar.SetGetterT<T>(getter);
    reflVar.GetVariant().Set<T>(getter());

    ReflectVariableHints hints(hintsString);
    reflVar.SetHints(hints);

    GetReflectStructPtr()->AddVariable(reflVar);
}

template <class TClass, class T, class TToCastTo>
void IReflectable::ReflectVarMember(const String &varName,
                                    void (TClass::*setter)(const T &),
                                    const T &(TClass::*getter)() const,
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
