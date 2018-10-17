#include "Bang/IReflectable.h"

#include "Bang/BPReflectedStruct.h"

using namespace Bang;

const BPReflectedStruct &IReflectable::GetReflectionInfo() const
{
    static BPReflectedStruct empty;
    return empty;
}
