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

    virtual void Reflect() const;
    ReflectStruct *GetReflectionInfoPtr() const;

private:
    mutable bool m_alreadyReflected = false;
    mutable ReflectStruct m_reflectionInfo;
};
}

#endif  // IREFLECTABLE_H
