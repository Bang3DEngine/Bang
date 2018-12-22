#include "Bang/GUIDManager.h"

#include "Bang/Assert.h"
#include "Bang/MetaFilesManager.h"
#include "Bang/USet.tcc"

using namespace Bang;

GUIDManager::GUIDManager()
{
}

GUID GUIDManager::GetNewGUID()
{
    GUID guid;

    do
    {
        guid = GUID::GetRandomGUID();
    } while (GUIDManager::IsBeingUsed(guid));

    GUIDManager::GetInstance()->m_guids.Add(guid);
    return guid;
}

void GUIDManager::RemoveGUID(const GUID &guid)
{
    GUIDManager::GetInstance()->m_guids.Remove(guid);
}

bool GUIDManager::IsBeingUsed(const GUID &guid)
{
    return GUIDManager::GetInstance()->m_guids.Contains(guid);
}

void GUIDManager::CreateEmbeddedFileGUID(const GUID &baseGUID,
                                         GUID::GUIDType embeddedFileGUID,
                                         GUID *newGUID)
{
    ASSERT_MSG(embeddedFileGUID > 0,
               "Inner file GUID's must be greater than zero!");

    *newGUID = baseGUID;
    newGUID->SetEmbeddedAssetGUID(embeddedFileGUID);
}

GUIDManager *GUIDManager::GetInstance()
{
    return MetaFilesManager::GetGUIDManager();
}
