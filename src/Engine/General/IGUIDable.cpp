#include "Bang/IGUIDable.h"

#include "Bang/GUIDManager.h"

using namespace Bang;

IGUIDable::IGUIDable()
{
}

IGUIDable::~IGUIDable()
{
}

void IGUIDable::SetGUID(const GUID &guid)
{
    m_GUID = guid;
    GUIDManager::RemoveGUID(GetGUID());
}

const GUID &IGUIDable::GetGUID() const
{
    return m_GUID;
}

GUID &IGUIDable::GetGUID()
{
    return m_GUID;
}
