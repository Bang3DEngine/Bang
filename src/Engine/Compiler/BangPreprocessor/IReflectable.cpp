#include "Bang/IReflectable.h"

#include "Bang/BPReflectedStruct.h"

using namespace Bang;

const BPReflectedStruct &IReflectable::GetReflectionInfo() const
{
    return m_reflectionInfo;
}

void IReflectable::Reflect() const
{
    // Empty
}

BPReflectedStruct *IReflectable::GetReflectionInfoPtr() const
{
    return &m_reflectionInfo;
}
