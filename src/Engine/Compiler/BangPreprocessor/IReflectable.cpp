#include "Bang/IReflectable.h"

#include "Bang/Assets.h"
#include "Bang/ReflectStruct.h"

using namespace Bang;

const ReflectStruct &IReflectable::GetReflectStruct() const
{
    // if (!m_alreadyReflected)
    {
        m_reflectStruct.Clear();
        const_cast<IReflectable *>(this)->Reflect();
        m_alreadyReflected = true;
    }
    return m_reflectStruct;
}

void IReflectable::Reflect()
{
    // Empty
}

ReflectStruct *IReflectable::GetReflectStructPtr() const
{
    return &m_reflectStruct;
}
