#ifndef GUIDMANAGER_H
#define GUIDMANAGER_H

#include "Bang/BangDefines.h"
#include "Bang/GUID.h"
#include "Bang/USet.h"

namespace Bang
{
class GUIDManager
{
public:
    static GUID GetNewGUID();
    static void RemoveGUID(const GUID &guid);
    static bool IsBeingUsed(const GUID &guid);

    static void CreateEmbeddedFileGUID(const GUID &baseGUID,
                                       GUID::GUIDType embeddedFileGUID,
                                       GUID *newGUID);

private:
    GUIDManager();

    USet<GUID> m_guids;

    static GUIDManager *GetInstance();

    friend class MetaFilesManager;
};
}

#endif  // IMPORTFILESMANAGER_H
