#include "Bang/IReflectable.h"

#include "Bang/ReflectStruct.h"

using namespace Bang;

const ReflectStruct &IReflectable::GetReflectionInfo() const
{
    if (!m_alreadyReflected)
    {
        const_cast<IReflectable *>(this)->Reflect();
        m_alreadyReflected = true;
    }
    return m_reflectionInfo;
}

void IReflectable::Reflect()
{
    // Empty
}

ReflectStruct *IReflectable::GetReflectionInfoPtr() const
{
    return &m_reflectionInfo;
}
