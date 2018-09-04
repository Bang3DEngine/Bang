#ifndef METAFILESMANAGER_H
#define METAFILESMANAGER_H

#include "Bang/Map.h"
#include "Bang/UMap.h"
#include "Bang/GUID.h"
#include "Bang/Path.h"
#include "Bang/GUIDManager.h"

NAMESPACE_BANG_BEGIN

class MetaFilesManager
{
public:
    static void CreateMissingMetaFiles(const Path &directory);
    static void LoadMetaFilepathGUIDs(const Path &directory);

    static std::pair<Path, GUID> CreateMetaFileIfMissing(const Path &filepath);
    static bool HasMetaFile(const Path &filepath);
    static bool IsMetaFile(const Path &filepath);
    static void DuplicateMetaFile(const Path &filepath,
                                    const Path &dupFilepath);
    static GUIDManager *GetGUIDManager();
    static void RegisterMetaFilepath(const Path &metaFilepath);
    static void UnRegisterMetaFilepath(const Path &metaFilepath);

    static Path GetFilepath(const GUID& guid);
    static Path GetFilepath(const Path& importFilepath);
    static Path GetMetaFilepath(const Path &filepath);
    static Path GetMetaFilepath(const GUID& guid);
    static GUID GetGUID(const Path& filepath);

    static void OnFilepathRenamed(const Path &oldPath, const Path &newPath);


private:
    Map<GUID, Path> m_GUIDToFilepath;
    Map<Path, GUID> m_filepathToGUID;

    GUIDManager m_GUIDManager;

    MetaFilesManager();
    ~MetaFilesManager();

    static String GetMetaExtension();
    static MetaFilesManager* GetInstance();

    friend class Application;
};

NAMESPACE_BANG_END

#endif // METAFILESMANAGER_H