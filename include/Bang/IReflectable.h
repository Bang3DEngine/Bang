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

class ReflectStruct;

class IReflectable
{
public:
    const ReflectStruct &GetReflectionInfo() const;

protected:
    IReflectable() = default;
    virtual ~IReflectable() = default;

    template <class T>
    void ReflectVar(const String &varName,
                    std::function<void(T)> setter,
                    std::function<T()> getter,
                    const T &initValue = T());

    virtual void Reflect();
    ReflectStruct *GetReflectionInfoPtr() const;

private:
    mutable bool m_alreadyReflected = false;
    mutable ReflectStruct m_reflectionInfo;
};

template <class T>
void IReflectable::ReflectVar(const String &varName,
                              std::function<void(T)> setter,
                              std::function<T()> getter,
                              const T &initValue)
{
    ReflectVariable reflVar;
    reflVar.SetName(varName);
    reflVar.SetSetterT<T>(setter);
    reflVar.SetGetterT<T>(getter);
    reflVar.GetVariant() = Variant::From<T>(initValue);
    GetReflectionInfoPtr()->AddVariable(reflVar);
}
}

#endif  // IREFLECTABLE_H
