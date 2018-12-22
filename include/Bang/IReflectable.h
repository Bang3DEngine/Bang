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
class Asset;
class GUID;

class IReflectable
{
public:
    const ReflectStruct &GetReflectStruct() const;

protected:
    IReflectable() = default;
    virtual ~IReflectable() = default;

    template <class T>
    ReflectVariable *ReflectVar(const String &varName,
                                std::function<void(T)> setter,
                                std::function<T()> getter,
                                const String &hintsString = "");

    template <class TClass, class T, class TToCastTo = T>
    ReflectVariable *ReflectVarMember(const String &varName,
                                      void (TClass::*setter)(T),
                                      T (TClass::*getter)() const,
                                      TClass *instance,
                                      const String &hintsString = "");
    template <class TClass, class T, class TToCastTo = T>
    ReflectVariable *ReflectVarMember(const String &varName,
                                      void (TClass::*setter)(const T &),
                                      const T &(TClass::*getter)() const,
                                      TClass *instance,
                                      const String &hintsString = "");

    template <class T>
    ReflectVariable *ReflectVarEnum(const String &varName,
                                    std::function<void(T)> setter,
                                    std::function<T()> getter,
                                    const String &hintsString = "");
    template <class TClass, class T>
    ReflectVariable *ReflectVarMemberEnum(const String &varName,
                                          void (TClass::*setter)(T),
                                          T (TClass::*getter)() const,
                                          TClass *instance,
                                          const String &hintsString = "");

    virtual void Reflect();
    ReflectStruct *GetReflectStructPtr() const;

private:
    mutable bool m_alreadyReflected = false;
    mutable ReflectStruct m_reflectStruct;

    static Asset *LoadAssetFromGUID(const GUID &guid);
};

template <class T>
ReflectVariable *IReflectable::ReflectVar(const String &varName,
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

    return GetReflectStructPtr()->GetReflectVariablePtr(varName);
}

template <class TClass, class T, class TToCastTo>
ReflectVariable *IReflectable::ReflectVarMember(
    const String &varName,
    void (TClass::*setter)(const T &),
    const T &(TClass::*getter)() const,
    TClass *instance,
    const String &hintsString)
{
    return ReflectVar<TToCastTo>(
        varName,
        [instance, setter](TToCastTo v) { (instance->*setter)(SCAST<T>(v)); },
        [instance, getter]() -> TToCastTo {
            return SCAST<TToCastTo>((instance->*getter)());
        },
        hintsString);
}
template <class TClass, class T, class TToCastTo>
ReflectVariable *IReflectable::ReflectVarMember(const String &varName,
                                                void (TClass::*setter)(T),
                                                T (TClass::*getter)() const,
                                                TClass *instance,
                                                const String &hintsString)
{
    return ReflectVar<TToCastTo>(
        varName,
        [instance, setter](TToCastTo v) { (instance->*setter)(SCAST<T>(v)); },
        [instance, getter]() -> TToCastTo {
            return SCAST<TToCastTo>((instance->*getter)());
        },
        hintsString);
}
template <class T>
ReflectVariable *IReflectable::ReflectVarEnum(const String &varName,
                                              std::function<void(T)> setter,
                                              std::function<T()> getter,
                                              const String &hintsString)
{
    return ReflectVar<T>(varName, setter, getter, hintsString);
}
template <class TClass, class T>
ReflectVariable *IReflectable::ReflectVarMemberEnum(const String &varName,
                                                    void (TClass::*setter)(T),
                                                    T (TClass::*getter)() const,
                                                    TClass *instance,
                                                    const String &hintsString)
{
    return ReflectVarMember<TClass, T, int>(
        varName, setter, getter, instance, hintsString);
}
}

#endif  // IREFLECTABLE_H
