#ifndef IREFLECTABLE_H
#define IREFLECTABLE_H

#include "Bang/BangDefines.h"

namespace Bang
{
#define BP_REFLECT_VARIABLE(...)
#define BP_REFLECT_CLASS(...)
#define BP_REFLECT_STRUCT(...)
#define BP_REFLECT_DEFINITIONS(...)

class BPReflectedStruct;

class IReflectable
{
public:
    virtual const BPReflectedStruct &GetReflectionInfo() const;

protected:
    IReflectable()
    {
    }
    virtual ~IReflectable()
    {
    }
};
}

#endif  // IREFLECTABLE_H
